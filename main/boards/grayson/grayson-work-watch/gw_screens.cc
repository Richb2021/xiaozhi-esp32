#include "gw_screens.h"
#include "gw_theme.h"
#include <esp_log.h>
#define TAG "GwScreens"
using namespace gw;

static lv_obj_t* Label(lv_obj_t* p, const char* txt, const lv_font_t* f, uint32_t color) {
    auto l = lv_label_create(p); lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, f, 0); lv_obj_set_style_text_color(l, C(color), 0); return l;
}
lv_obj_t* GwScreens::Card(lv_obj_t* parent, uint32_t bg, int radius) {
    auto c = lv_obj_create(parent);
    lv_obj_set_style_bg_color(c, C(bg), 0); lv_obj_set_style_bg_opa(c, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(c, radius, 0); lv_obj_set_style_border_width(c, 0, 0);
    lv_obj_set_style_pad_all(c, 12, 0); lv_obj_set_scrollbar_mode(c, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(c, LV_OBJ_FLAG_SCROLLABLE); return c;
}
lv_obj_t* GwScreens::Dot(lv_obj_t* parent, uint32_t color, int size) {
    auto d = lv_obj_create(parent); lv_obj_set_size(d, size, size);
    lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0); lv_obj_set_style_bg_color(d, C(color), 0);
    lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0); lv_obj_set_style_border_width(d, 0, 0);
    lv_obj_remove_flag(d, LV_OBJ_FLAG_SCROLLABLE); return d;
}
uint32_t GwScreens::StatusColor(const DashboardJob& j) {
    if (j.runtime_status == "waiting") return EMBER;
    if (j.status == "failed") return DANGER;
    if (j.status == "completed") return INK_DIM;
    return GRASS;
}

void GwScreens::Build(lv_obj_t* screen, lv_obj_t* voice_container) {
    tileview_ = lv_tileview_create(screen);
    lv_obj_set_size(tileview_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(tileview_, C(DEEP), 0); lv_obj_set_style_bg_opa(tileview_, LV_OPA_COVER, 0);
    lv_obj_set_scrollbar_mode(tileview_, LV_SCROLLBAR_MODE_OFF);
    face_tile_  = lv_tileview_add_tile(tileview_, 0, 0, LV_DIR_RIGHT);
    dash_tile_  = lv_tileview_add_tile(tileview_, 1, 0, LV_DIR_HOR);
    voice_tile_ = lv_tileview_add_tile(tileview_, 2, 0, LV_DIR_LEFT);
    for (auto t : {face_tile_, dash_tile_, voice_tile_}) { lv_obj_set_style_bg_color(t, C(DEEP), 0); lv_obj_set_style_bg_opa(t, LV_OPA_COVER, 0); }
    BuildFace(face_tile_);
    BuildDashboard(dash_tile_);
    lv_obj_set_parent(voice_container, voice_tile_);
    lv_obj_set_size(voice_container, LV_HOR_RES, LV_VER_RES);
    lv_obj_move_background(tileview_);
    lv_tileview_set_tile_by_index(tileview_, 1, 0, LV_ANIM_OFF);
}

void GwScreens::BuildFace(lv_obj_t* t) {
    lv_obj_set_style_pad_all(t, 30, 0); lv_obj_set_style_pad_top(t, 44, 0);
    lv_obj_set_flex_flow(t, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(t, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto top = lv_obj_create(t); lv_obj_set_size(top, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(top, LV_OPA_TRANSP, 0); lv_obj_set_style_border_width(top, 0, 0); lv_obj_set_style_pad_all(top, 0, 0);
    lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW); lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto hub = lv_obj_create(top); lv_obj_set_size(hub, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(hub, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hub, 0, 0); lv_obj_set_style_pad_all(hub, 0, 0); lv_obj_set_flex_flow(hub, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(hub, 8, 0); lv_obj_set_flex_align(hub, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    face_hub_dot_ = Dot(hub, DANGER, 10);
    face_hub_label_ = Label(hub, "hub · off", &font_space_mono_14, INK_DIM);
    face_batt_ = Label(top, "--%", &font_space_mono_14, INK_DIM);

    auto mid = lv_obj_create(t); lv_obj_set_size(mid, LV_PCT(100), LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(mid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(mid, 0, 0); lv_obj_set_style_pad_all(mid, 0, 0); lv_obj_set_flex_flow(mid, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(mid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); lv_obj_set_style_pad_row(mid, 14, 0);
    auto g = lv_image_create(mid); lv_image_set_src(g, &g_mark_160); lv_image_set_scale(g, 115);  // 160 px × 0.45 ≈ 72 px
    face_clock_ = Label(mid, "--:--", &font_archivo_black_96, INK);
    face_date_ = Label(mid, "", &font_space_mono_18, INK_DIM);

    auto bottom = lv_obj_create(t); lv_obj_set_size(bottom, LV_PCT(100), LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(bottom, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bottom, 0, 0); lv_obj_set_style_pad_all(bottom, 0, 0); lv_obj_set_flex_flow(bottom, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(bottom, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); lv_obj_set_style_pad_row(bottom, 10, 0);
    auto pills = lv_obj_create(bottom); lv_obj_set_size(pills, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(pills, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(pills, 0, 0); lv_obj_set_style_pad_all(pills, 0, 0); lv_obj_set_flex_flow(pills, LV_FLEX_FLOW_ROW); lv_obj_set_style_pad_column(pills, 10, 0);
    auto pill1 = Card(pills, PANEL, 999); lv_obj_set_style_pad_ver(pill1, 8, 0); lv_obj_set_style_pad_hor(pill1, 16, 0);
    face_jobs_pill_ = Label(pill1, "0 jobs", &font_space_mono_14, INK);
    auto pill2 = Card(pills, PANEL_LIGHT, 999); lv_obj_set_style_pad_ver(pill2, 8, 0); lv_obj_set_style_pad_hor(pill2, 16, 0);
    lv_obj_set_style_border_width(pill2, 1, 0); lv_obj_set_style_border_color(pill2, C(EMBER), 0);
    face_wait_pill_ = Label(pill2, "0 waiting", &font_space_mono_14, EMBER); lv_obj_add_flag(pill2, LV_OBJ_FLAG_HIDDEN);
    auto wm = Label(bottom, "G R A Y S O N   W O R K", &font_space_mono_14, GOLD);
    lv_obj_set_style_text_letter_space(wm, 2, 0);
}

void GwScreens::BuildDashboard(lv_obj_t* t) {
    lv_obj_set_style_pad_hor(t, 26, 0); lv_obj_set_style_pad_top(t, 40, 0); lv_obj_set_style_pad_bottom(t, 28, 0);
    lv_obj_set_flex_flow(t, LV_FLEX_FLOW_COLUMN); lv_obj_set_style_pad_row(t, 14, 0);
    auto head = lv_obj_create(t); lv_obj_set_size(head, LV_PCT(100), LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(head, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(head, 0, 0); lv_obj_set_style_pad_all(head, 0, 0); lv_obj_set_flex_flow(head, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto brand = lv_obj_create(head); lv_obj_set_size(brand, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(brand, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(brand, 0, 0); lv_obj_set_style_pad_all(brand, 0, 0); lv_obj_set_flex_flow(brand, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(brand, 10, 0); lv_obj_set_flex_align(brand, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto g = lv_image_create(brand); lv_image_set_src(g, &g_mark_160); lv_image_set_scale(g, 45);
    Label(brand, "Work", &font_archivo_bold_22, INK);
    dash_clock_ = Label(head, "--:--", &font_space_mono_18, INK_DIM);

    hc_card_ = Card(t, PANEL, 22); lv_obj_set_width(hc_card_, LV_PCT(100)); lv_obj_set_style_border_width(hc_card_, 1, 0); lv_obj_set_style_border_color(hc_card_, C(LINE), 0);
    lv_obj_set_flex_flow(hc_card_, LV_FLEX_FLOW_COLUMN); lv_obj_set_style_pad_row(hc_card_, 2, 0);
    Label(hc_card_, "24HOURCODER", &font_space_mono_14, INK_DIM);
    hc_status_ = Label(hc_card_, "—", &font_archivo_bold_22, GRASS);

    jobs_header_ = Label(t, "JOBS · 0", &font_space_mono_14, INK_DIM);
    jobs_list_ = lv_obj_create(t); lv_obj_set_size(jobs_list_, LV_PCT(100), LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(jobs_list_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(jobs_list_, 0, 0); lv_obj_set_style_pad_all(jobs_list_, 0, 0); lv_obj_set_flex_flow(jobs_list_, LV_FLEX_FLOW_COLUMN); lv_obj_set_style_pad_row(jobs_list_, 10, 0);

    Label(t, "BOXES", &font_space_mono_14, INK_DIM);
    boxes_grid_ = lv_obj_create(t); lv_obj_set_size(boxes_grid_, LV_PCT(100), LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(boxes_grid_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(boxes_grid_, 0, 0); lv_obj_set_style_pad_all(boxes_grid_, 0, 0); lv_obj_set_flex_flow(boxes_grid_, LV_FLEX_FLOW_ROW); lv_obj_set_style_pad_column(boxes_grid_, 8, 0);
    RenderDashboard();
}

void GwScreens::RenderDashboard() {
    if (!hc_status_) return;
    lv_label_set_text(hc_status_, model_.hc_paused ? "Paused" : (model_.hc_status.empty() ? "—" : model_.hc_status.c_str()));
    lv_obj_set_style_text_color(hc_status_, C(model_.hc_paused ? EMBER : (model_.hc_status == "working" ? GRASS : INK_DIM)), 0);
    lv_label_set_text_fmt(jobs_header_, "JOBS · %d", (int)model_.jobs.size());
    lv_obj_clean(jobs_list_);
    int shown = 0;
    for (auto& j : model_.jobs) {
        if (shown++ == 4) break;
        bool waiting = j.runtime_status == "waiting";
        auto row = Card(jobs_list_, waiting ? PANEL_LIGHT : PANEL, 18); lv_obj_set_width(row, LV_PCT(100));
        if (waiting) { lv_obj_set_style_border_width(row, 1, 0); lv_obj_set_style_border_color(row, C(EMBER), 0); }
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW); lv_obj_set_style_pad_column(row, 12, 0);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        Dot(row, StatusColor(j), 12);
        auto col = lv_obj_create(row); lv_obj_set_size(col, LV_PCT(85), LV_SIZE_CONTENT); lv_obj_set_style_bg_opa(col, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(col, 0, 0); lv_obj_set_style_pad_all(col, 0, 0); lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
        auto title = Label(col, (j.project + " · " + j.provider).c_str(), &font_archivo_bold_22, INK);
        lv_label_set_long_mode(title, LV_LABEL_LONG_DOT); lv_obj_set_width(title, LV_PCT(100));
        Label(col, (waiting ? "needs input" : j.id).c_str(), &font_space_mono_14, waiting ? EMBER : INK_DIM);
    }
    lv_obj_clean(boxes_grid_);
    for (auto& b : model_.boxes) {
        auto c = Card(boxes_grid_, PANEL, 16); lv_obj_set_flex_grow(c, 1); lv_obj_set_style_pad_all(c, 10, 0);
        lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN); lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); lv_obj_set_style_pad_row(c, 6, 0);
        Dot(c, b.ok ? GRASS : DANGER, 10);
        Label(c, b.box.c_str(), &font_space_mono_14, INK_DIM);
    }
    if (face_jobs_pill_) {
        lv_label_set_text_fmt(face_jobs_pill_, "%d jobs", (int)model_.jobs.size());
        int w = model_.waiting();
        lv_label_set_text_fmt(face_wait_pill_, "%d waiting", w);
        if (w) lv_obj_remove_flag(lv_obj_get_parent(face_wait_pill_), LV_OBJ_FLAG_HIDDEN); else lv_obj_add_flag(lv_obj_get_parent(face_wait_pill_), LV_OBJ_FLAG_HIDDEN);
    }
}

static std::string S(const cJSON* o, const char* k) { auto v = cJSON_GetObjectItem(o, k); return cJSON_IsString(v) ? v->valuestring : ""; }

void GwScreens::UpdateDashboard(const cJSON* root) {
    DashboardModel m;
    auto hc = cJSON_GetObjectItem(root, "hc");
    if (cJSON_IsObject(hc)) { m.hc_status = S(hc, "status"); m.hc_paused = cJSON_IsTrue(cJSON_GetObjectItem(hc, "paused")); }
    auto jobs = cJSON_GetObjectItem(root, "jobs"); cJSON* j;
    if (cJSON_IsArray(jobs)) cJSON_ArrayForEach(j, jobs) m.jobs.push_back({S(j, "id"), S(j, "provider"), S(j, "project"), S(j, "status"), S(j, "runtime_status")});
    auto boxes = cJSON_GetObjectItem(root, "boxes"); cJSON* b;
    if (cJSON_IsArray(boxes)) cJSON_ArrayForEach(b, boxes) m.boxes.push_back({S(b, "box"), cJSON_IsTrue(cJSON_GetObjectItem(b, "ok")) != 0});
    model_ = std::move(m);
    RenderDashboard();
}

void GwScreens::SetClock(const char* hhmm, const char* date) {
    if (face_clock_) lv_label_set_text(face_clock_, hhmm);
    if (face_date_) lv_label_set_text(face_date_, date);
    if (dash_clock_) lv_label_set_text(dash_clock_, hhmm);
}
void GwScreens::SetBattery(int pct, bool charging) {
    if (face_batt_) lv_label_set_text_fmt(face_batt_, "%s%d%%", charging ? "+" : "", pct);
}
void GwScreens::SetHubState(bool online, bool lan) {
    if (!face_hub_dot_) return;
    lv_obj_set_style_bg_color(face_hub_dot_, C(online ? GRASS : DANGER), 0);
    lv_label_set_text(face_hub_label_, online ? (lan ? "hub · lan" : "hub · away") : "hub · off");
}
void GwScreens::GoTo(int tile) { if (tileview_) lv_tileview_set_tile_by_index(tileview_, tile, 0, LV_ANIM_ON); }

void GwScreens::ShowToast(const char* level, const char* title, const char* body) { ESP_LOGI(TAG, "toast %s: %s — %s", level, title, body); }
