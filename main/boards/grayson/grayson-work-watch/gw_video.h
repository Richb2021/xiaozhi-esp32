#pragma once
#include <string>
#include <cstdint>
#include <cstddef>

// Downloads a short MJPEG AVI (PCM audio, 24 kHz mono) into PSRAM and plays it on the watch.
class GwVideo {
public:
    static GwVideo& GetInstance() { static GwVideo v; return v; }
    void Play(const std::string& url, const std::string& title, int seconds);
    void Stop();
    bool IsPlaying() const { return playing_; }

private:
    static void PlayTask(void* arg);
    void RunPlay();
    std::string url_, title_;
    volatile bool playing_ = false;
    uint8_t* buf_ = nullptr;
    size_t size_ = 0;
    void* player_ = nullptr;
};
