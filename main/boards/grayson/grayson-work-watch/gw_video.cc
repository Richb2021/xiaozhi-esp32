#include "gw_video.h"
#include "gw_screens.h"
#include "board.h"
#include "audio_codec.h"
#include "display.h"
#include "display/lvgl_display/lvgl_image.h"
#include "jpeg_to_image.h"

#include <avi_player.h>
#include "http.h"
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>
#include <vector>

#define TAG "GwVideo"
static constexpr size_t kMaxBytes = 6 * 1024 * 1024;

static void video_cb(frame_data_t* f, void* arg) {
    if (f->type != FRAME_TYPE_VIDEO || f->video_info.frame_format != FORMAT_MJEPG) {
        return;
    }
    uint8_t* out = nullptr;
    size_t out_len = 0, w = 0, h = 0, stride = 0;
    if (jpeg_to_image(f->data, f->data_bytes, &out, &out_len, &w, &h, &stride) != ESP_OK || out == nullptr) {
        ESP_LOGW(TAG, "frame decode failed (%u bytes)", (unsigned)f->data_bytes);
        return;
    }
    auto img = std::make_unique<LvglAllocatedImage>(out, out_len, (int)w, (int)h, (int)stride, LV_COLOR_FORMAT_RGB565);
    auto display = Board::GetInstance().GetDisplay();
    DisplayLockGuard lock(display);
    GwScreens::GetInstance().ShowVideoFrame(std::move(img));
}

static void audio_cb(frame_data_t* f, void* arg) {
    if (f->type != FRAME_TYPE_AUDIO) {
        return;
    }
    auto codec = Board::GetInstance().GetAudioCodec();
    std::vector<int16_t> pcm(reinterpret_cast<int16_t*>(f->data), reinterpret_cast<int16_t*>(f->data) + f->data_bytes / 2);
    codec->OutputData(pcm);
}

static void audio_clock_cb(uint32_t rate, uint32_t bits, uint32_t ch, void* arg) {
    auto codec = Board::GetInstance().GetAudioCodec();
    if ((int)rate != codec->output_sample_rate() || ch != 1 || bits != 16) {
        ESP_LOGW(TAG, "AVI audio %u Hz/%u ch/%u bit, codec wants %d Hz mono 16 bit", (unsigned)rate, (unsigned)ch, (unsigned)bits, codec->output_sample_rate());
    }
}

static void end_cb(void* arg) {
    GwVideo::GetInstance().Stop();
}

void GwVideo::Play(const std::string& url, const std::string& title, int seconds) {
    if (playing_) {
        ESP_LOGW(TAG, "already playing, ignoring %s", title.c_str());
        return;
    }
    url_ = url;
    title_ = title;
    playing_ = true;
    if (xTaskCreatePinnedToCore(PlayTask, "gw_video", 8192, this, 5, nullptr, 1) != pdPASS) {
        ESP_LOGE(TAG, "failed to start video task");
        playing_ = false;
    }
}

void GwVideo::PlayTask(void* arg) {
    static_cast<GwVideo*>(arg)->RunPlay();
    vTaskDelete(nullptr);
}

void GwVideo::RunPlay() {
    auto display = Board::GetInstance().GetDisplay();
    buf_ = static_cast<uint8_t*>(heap_caps_malloc(kMaxBytes, MALLOC_CAP_SPIRAM));
    if (!buf_) {
        DisplayLockGuard lock(display);
        GwScreens::GetInstance().ShowToast("error", "Video", "Out of memory");
        playing_ = false;
        return;
    }
    auto network = Board::GetInstance().GetNetwork();
    auto http = network->CreateHttp(0);
    http->SetTimeout(15000);
    bool ok = http->Open("GET", url_) && http->GetStatusCode() == 200;
    if (ok) {
        size_ = 0;
        int n = 0;
        while (size_ < kMaxBytes && (n = http->Read(reinterpret_cast<char*>(buf_ + size_), kMaxBytes - size_)) > 0) {
            size_ += n;
        }
        if (n < 0) ok = false;
    }
    http->Close();
    if (!ok || size_ == 0) {
        ESP_LOGE(TAG, "download failed: %s", url_.c_str());
        DisplayLockGuard lock(display);
        GwScreens::GetInstance().ShowToast("error", "Video", "Download failed");
        heap_caps_free(buf_); buf_ = nullptr;
        playing_ = false;
        return;
    }
    ESP_LOGI(TAG, "downloaded %u bytes for %s", (unsigned)size_, title_.c_str());

    avi_player_config_t pc = {};
    pc.buffer_size = 64 * 1024;
    pc.video_cb = video_cb;
    pc.audio_cb = audio_cb;
    pc.audio_set_clock_cb = audio_clock_cb;
    pc.avi_play_end_cb = end_cb;
    pc.priority = 5;
    pc.coreID = 1;
    pc.stack_size = 8192;
    pc.stack_in_psram = true;
    avi_player_handle_t h = nullptr;
    if (avi_player_init(pc, &h) != ESP_OK) {
        ESP_LOGE(TAG, "avi_player_init failed");
        heap_caps_free(buf_); buf_ = nullptr;
        playing_ = false;
        return;
    }
    player_ = h;
    {
        DisplayLockGuard lock(display);
        GwScreens::GetInstance().OnVideoTap([]() { GwVideo::GetInstance().Stop(); });
        GwScreens::GetInstance().ShowToast("info", title_.c_str(), size_ >= kMaxBytes ? "Showing the first part only" : "Playing");
    }
    Board::GetInstance().GetAudioCodec()->EnableOutput(true);
    avi_player_play_from_memory(h, buf_, size_);
}

void GwVideo::Stop() {
    if (!playing_ && !player_) {
        return;
    }
    playing_ = false;
    if (player_) {
        avi_player_play_stop(static_cast<avi_player_handle_t>(player_));
        avi_player_deinit(static_cast<avi_player_handle_t>(player_));
        player_ = nullptr;
    }
    if (buf_) {
        heap_caps_free(buf_);
        buf_ = nullptr;
    }
    auto display = Board::GetInstance().GetDisplay();
    DisplayLockGuard lock(display);
    GwScreens::GetInstance().HideVideo();
}
