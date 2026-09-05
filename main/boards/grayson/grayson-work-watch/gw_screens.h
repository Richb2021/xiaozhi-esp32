#pragma once
#include <lvgl.h>
#include <cJSON.h>
#include <string>
#include <vector>
#include <memory>
#include "display/lvgl_display/lvgl_image.h"

struct DashboardJob { std::string id, provider, project, status, runtime_status; };
struct DashboardBox { std::string box; bool ok; };
struct DashboardModel {
    std::string hc_status; bool hc_paused = false;
    std::vector<DashboardJob> jobs; std::vector<DashboardBox> boxes;
    int waiting() const { int n = 0; for (auto& j : jobs) if (j.runtime_status == "waiting") n++; return n; }
};

// Conversation state as shown on the watch (mirrors the device state machine)
enum GwConvState { GW_IDLE = 0, GW_CONNECTING, GW_LISTENING, GW_SPEAKING };

class GwScreens {
public:
    static GwScreens& GetInstance() { static GwScreens s; return s; }
    // All methods must be called with the LVGL lock held.
    void Build(lv_obj_t* screen);
    void AdoptChat(lv_obj_t* bottom_bar, lv_obj_t* chat_label);   // stock chat text -> Voice tile
    void UpdateDashboard(const cJSON* root);
    void SetClock(const char* hhmm, const char* date);
    void SetBattery(int pct, bool charging);
    void SetHubState(bool online, bool lan);
    void SetConversationState(GwConvState s);
    void SetSleeping(bool sleeping);
    void ShowToast(const char* level, const char* title, const char* body);
    void GoTo(int tile);            // 0 face, 1 dashboard, 2 voice
    void NextTile();
    int  CurrentTile() const;
    void ShowVideoFrame(std::unique_ptr<LvglImage> frame);
    void HideVideo();
    void DumpGeometry();
    lv_obj_t* voice_tile() const { return voice_tile_; }
    void OnStopTap(void (*cb)()) { stop_cb_ = cb; }
    void OnSleepTap(void (*cb)()) { sleep_cb_ = cb; }
    void OnVideoTap(void (*cb)()) { video_tap_cb_ = cb; }
    void OnVoiceTap(void (*cb)()) { voice_tap_cb_ = cb; }

private:
    void BuildFace(lv_obj_t* tile);
    void BuildDashboard(lv_obj_t* tile);
    void BuildVoice(lv_obj_t* tile);
    void BuildOverlay(lv_obj_t* screen);
    void RenderDashboard();
    static lv_obj_t* Card(lv_obj_t* parent, uint32_t bg, int radius);
    static lv_obj_t* Row(lv_obj_t* parent, int gap);
    static lv_obj_t* Col(lv_obj_t* parent, int gap);
    static lv_obj_t* Dot(lv_obj_t* parent, uint32_t color, int size);
    static uint32_t StatusColor(const DashboardJob& j);

    lv_obj_t *tileview_ = nullptr, *face_tile_ = nullptr, *dash_tile_ = nullptr, *voice_tile_ = nullptr;
    // face
    lv_obj_t *face_clock_ = nullptr, *face_date_ = nullptr, *face_batt_ = nullptr, *face_hub_dot_ = nullptr,
             *face_hub_label_ = nullptr, *face_jobs_pill_ = nullptr, *face_wait_pill_ = nullptr, *face_wait_wrap_ = nullptr,
             *sleep_pill_ = nullptr, *sleep_label_ = nullptr;
    // dashboard
    lv_obj_t *dash_clock_ = nullptr, *hc_status_ = nullptr, *hc_wo_ = nullptr, *jobs_header_ = nullptr,
             *jobs_list_ = nullptr, *boxes_grid_ = nullptr;
    // voice
    lv_obj_t *voice_hint_ = nullptr, *voice_clock_ = nullptr, *bars_ = nullptr, *bar_[7] = {}, *stop_pill_ = nullptr,
             *chat_bar_ = nullptr;
    lv_timer_t* bars_timer_ = nullptr;
    // overlay
    lv_obj_t *indicator_ = nullptr, *indicator_dot_ = nullptr, *indicator_label_ = nullptr;
    lv_obj_t *toast_ = nullptr, *toast_title_ = nullptr, *toast_body_ = nullptr, *toast_kind_ = nullptr;
    lv_timer_t* toast_timer_ = nullptr;
    lv_obj_t* video_img_ = nullptr;
    std::unique_ptr<LvglImage> video_frames_[2];
    int video_frame_idx_ = 0;

    GwConvState conv_ = GW_IDLE;
    int prev_tile_ = 1;
    DashboardModel model_;
    void (*stop_cb_)() = nullptr;
    void (*sleep_cb_)() = nullptr;
    void (*video_tap_cb_)() = nullptr;
    void (*voice_tap_cb_)() = nullptr;
};
