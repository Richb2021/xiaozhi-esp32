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

class GwScreens {
public:
    static GwScreens& GetInstance() { static GwScreens s; return s; }
    void Build(lv_obj_t* screen, lv_obj_t* voice_container);
    void UpdateDashboard(const cJSON* root);      // call with the LVGL lock held
    void SetClock(const char* hhmm, const char* date);
    void SetBattery(int pct, bool charging);
    void SetHubState(bool online, bool lan);
    void ShowToast(const char* level, const char* title, const char* body);
    void GoTo(int tile);                          // 0 face, 1 dashboard, 2 voice
    lv_obj_t* voice_tile() const { return voice_tile_; }
    // Full-screen video overlay (call with the LVGL lock held)
    void ShowVideoFrame(std::unique_ptr<LvglImage> frame);
    void HideVideo();
    void OnVideoTap(void (*cb)()) { video_tap_cb_ = cb; }
private:
    void BuildFace(lv_obj_t* tile);
    void BuildDashboard(lv_obj_t* tile);
    void RenderDashboard();
    static lv_obj_t* Card(lv_obj_t* parent, uint32_t bg, int radius);
    static lv_obj_t* Dot(lv_obj_t* parent, uint32_t color, int size);
    static uint32_t StatusColor(const DashboardJob& j);
    lv_obj_t *tileview_ = nullptr, *face_tile_ = nullptr, *dash_tile_ = nullptr, *voice_tile_ = nullptr;
    lv_obj_t *face_clock_ = nullptr, *face_date_ = nullptr, *face_batt_ = nullptr, *face_hub_dot_ = nullptr,
             *face_hub_label_ = nullptr, *face_jobs_pill_ = nullptr, *face_wait_pill_ = nullptr;
    lv_obj_t *dash_clock_ = nullptr, *hc_card_ = nullptr, *hc_status_ = nullptr, *hc_sub_ = nullptr,
             *jobs_header_ = nullptr, *jobs_list_ = nullptr, *boxes_grid_ = nullptr;
    lv_obj_t *toast_ = nullptr, *toast_title_ = nullptr, *toast_body_ = nullptr, *toast_kind_ = nullptr;
    lv_timer_t* toast_timer_ = nullptr;
    lv_obj_t* video_img_ = nullptr;
    std::unique_ptr<LvglImage> video_frames_[2];
    int video_frame_idx_ = 0;
    void (*video_tap_cb_)() = nullptr;
    DashboardModel model_;
};