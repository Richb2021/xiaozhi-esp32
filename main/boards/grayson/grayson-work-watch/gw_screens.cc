#include "gw_screens.h"
#include "gw_theme.h"
#include <esp_log.h>
#include <cstring>
#include <cstdlib>
#define TAG "GwScreens"
using namespace gw;

// ---------- small builders ------------------------------------------------------------------
static lv_obj_t* Label(lv_obj_t* p, const char* txt, const lv_font_t* f, uint32_t color) {
    auto l = lv_label_create(p);
    lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, f, 0);
    lv_obj_set_style_text_color(l, C(color), 0);
    return l;
}
static void Bare(lv_obj_t* o) {   // invisible layout container
    lv_obj_set_style_bg_opa(o, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0);
    lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
}
lv_obj_t* GwScreens::Row(lv_obj_t* parent, int gap) {
    auto o = lv_obj_create(parent); Bare(o);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(o, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(o, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(o, gap, 0);
    return o;
}
lv_obj_t* GwScreens::Col(lv_obj_t* parent, int gap) {
    auto o = lv_obj_create(parent); Bare(o);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(o, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(o, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(o, gap, 0);
    return o;
}
lv_obj_t* GwScreens::Card(lv_obj_t* parent, uint32_t bg, int radius) {
    auto c = lv_obj_create(parent);
    lv_obj_set_size(c, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(c, C(bg), 0);
    lv_obj_set_style_bg_opa(c, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(c, radius, 0);
    lv_obj_set_style_border_width(c, 0, 0);
    lv_obj_set_style_pad_all(c, 12, 0);
    lv_obj_set_scrollbar_mode(c, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(c, LV_OBJ_FLAG_SCROLLABLE);
    return c;
}
lv_obj_t* GwScreens::Dot(lv_obj_t* parent, uint32_t color, int size) {
    auto d = lv_obj_create(parent);
    lv_obj_set_size(d, size, size);
    lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(d, C(color), 0);
    lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(d, 0, 0);
    lv_obj_set_style_pad_all(d, 0, 0);
    lv_obj_remove_flag(d, LV_OBJ_FLAG_SCROLLABLE);
    return d;
}
uint32_t GwScreens::StatusColor(const DashboardJob& j) {
    if (j.runtime_status == "waiting") return EMBER;
    if (j.status == "failed") return DANGER;
    if (j.status == "completed") return INK_DIM;
    return GRASS;
}
static lv_obj_t* MakePill(lv_obj_t* parent, uint32_t bg, const char* text, const lv_font_t* f, uint32_t fg, uint32_t border, lv_obj_t** label_out) {
    auto p = lv_obj_create(parent);
    lv_obj_set_size(p, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(p, C(bg), 0); lv_obj_set_style_bg_opa(p, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(p, 999, 0);
    lv_obj_set_style_pad_ver(p, 9, 0); lv_obj_set_style_pad_hor(p, 18, 0);
    lv_obj_set_style_border_width(p, border ? 1 : 0, 0);
    if (border) lv_obj_set_style_border_color(p, C(border), 0);
    lv_obj_set_scrollbar_mode(p, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(p, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(p, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(p, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(p, 8, 0);
    auto l = Label(p, text, f, fg);
    if (label_out) *label_out = l;
    return p;
}

// ---------- build ----------------------------------------------------------------------------
void GwScreens::Build(lv_obj_t* screen) {
    lv_obj_set_style_bg_color(screen, C(DEEP), 0);
    lv_obj_set_style_bg_image_src(screen, &gw_bg, 0);
    lv_obj_set_style_bg_image_opa(screen, LV_OPA_COVER, 0);

    tileview_ = lv_tileview_create(screen);
    lv_obj_set_size(tileview_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(tileview_, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(tileview_, LV_SCROLLBAR_MODE_OFF);
    face_tile_  = lv_tileview_add_tile(tileview_, 0, 0, LV_DIR_RIGHT);
    dash_tile_  = lv_tileview_add_tile(tileview_, 1, 0, LV_DIR_HOR);
    voice_tile_ = lv_tileview_add_tile(tileview_, 2, 0, LV_DIR_LEFT);
    for (auto t : {face_tile_, dash_tile_, voice_tile_}) {
        lv_obj_set_style_bg_opa(t, LV_OPA_TRANSP, 0);
        lv_obj_set_scrollbar_mode(t, LV_SCROLLBAR_MODE_OFF);
    }
    BuildFace(face_tile_);
    BuildDashboard(dash_tile_);
    BuildVoice(voice_tile_);
    BuildOverlay(screen);
    lv_obj_move_background(tileview_);
    lv_tileview_set_tile_by_index(tileview_, 1, 0, LV_ANIM_OFF);
    prev_tile_ = 1;
}

void GwScreens::BuildFace(lv_obj_t* t) {
    lv_obj_set_style_pad_hor(t, 30, 0); lv_obj_set_style_pad_top(t, 40, 0); lv_obj_set_style_pad_bottom(t, 30, 0);
    lv_obj_set_flex_flow(t, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(t, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // top row: hub state | battery
    auto top = Row(t, 0); lv_obj_set_width(top, LV_PCT(100));
    lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto hub = Row(top, 8);
    face_hub_dot_ = Dot(hub, DANGER, 10);
    face_hub_label_ = Label(hub, "hub off", &font_space_mono_16, INK_DIM);
    face_batt_ = Label(top, "USB", &font_space_mono_16, INK_DIM);

    // middle: G mark, clock, date
    auto mid = Col(t, 6);
    lv_obj_set_flex_align(mid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto g = lv_image_create(mid); lv_image_set_src(g, &g_mark_160); lv_image_set_scale(g, 115);   // ~72 px
    lv_obj_set_style_pad_bottom(g, 6, 0);
    face_clock_ = Label(mid, "00:00", &font_archivo_black_96, INK);
    lv_obj_set_style_text_letter_space(face_clock_, -3, 0);
    face_date_ = Label(mid, "", &font_space_mono_18, INK_DIM);

    // bottom: pills, sleep, wordmark
    auto bottom = Col(t, 12);
    lv_obj_set_flex_align(bottom, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto pills = Row(bottom, 10);
    auto jobs_wrap = MakePill(pills, PANEL, "0 jobs", &font_space_mono_16, INK, 0, &face_jobs_pill_);
    (void)jobs_wrap;
    face_wait_wrap_ = MakePill(pills, PANEL_LIGHT, "0 waiting", &font_space_mono_16, EMBER, EMBER, &face_wait_pill_);
    lv_obj_add_flag(face_wait_wrap_, LV_OBJ_FLAG_HIDDEN);
    sleep_pill_ = MakePill(bottom, PANEL, "SLEEP", &font_space_mono_16, INK_DIM, LINE, &sleep_label_);
    lv_obj_add_flag(sleep_pill_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(sleep_pill_, [](lv_event_t* e) {
        auto self = static_cast<GwScreens*>(lv_event_get_user_data(e));
        if (self->sleep_cb_) self->sleep_cb_();
    }, LV_EVENT_CLICKED, this);
    auto wmrow = Row(bottom, 10);
    auto l1 = lv_obj_create(wmrow); Bare(l1); lv_obj_set_size(l1, 26, 1); lv_obj_set_style_bg_color(l1, C(GOLD_DEEP), 0); lv_obj_set_style_bg_opa(l1, LV_OPA_COVER, 0);
    auto wm = Label(wmrow, "GRAYSON WORK", &font_space_mono_14, GOLD);
    lv_obj_set_style_text_letter_space(wm, 4, 0);
    auto l2 = lv_obj_create(wmrow); Bare(l2); lv_obj_set_size(l2, 26, 1); lv_obj_set_style_bg_color(l2, C(GOLD_DEEP), 0); lv_obj_set_style_bg_opa(l2, LV_OPA_COVER, 0);
}

void GwScreens::BuildDashboard(lv_obj_t* t) {
    lv_obj_set_style_pad_hor(t, 24, 0); lv_obj_set_style_pad_top(t, 36, 0); lv_obj_set_style_pad_bottom(t, 24, 0);
    lv_obj_set_flex_flow(t, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(t, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(t, 12, 0);

    auto head = Row(t, 0); lv_obj_set_width(head, LV_PCT(100));
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto brand = Row(head, 10);
    auto g = lv_image_create(brand); lv_image_set_src(g, &g_mark_160); lv_image_set_scale(g, 45);   // ~28 px
    Label(brand, "Work", &font_archivo_bold_26, INK);
    dash_clock_ = Label(head, "00:00", &font_space_mono_18, INK_DIM);

    auto hc = Card(t, PANEL, 22); lv_obj_set_width(hc, LV_PCT(100));
    lv_obj_set_style_border_width(hc, 1, 0); lv_obj_set_style_border_color(hc, C(LINE), 0);
    lv_obj_set_style_pad_all(hc, 14, 0); lv_obj_set_style_pad_hor(hc, 18, 0);
    lv_obj_set_flex_flow(hc, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hc, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto hcl = Col(hc, 2);
    auto hlab = Label(hcl, "24HOURCODER", &font_space_mono_14, INK_DIM); lv_obj_set_style_text_letter_space(hlab, 1, 0);
    hc_status_ = Label(hcl, "Offline", &font_archivo_bold_26, INK_DIM);
    hc_wo_ = Label(hc, "", &font_space_mono_16, INK_DIM);

    jobs_header_ = Label(t, "JOBS", &font_space_mono_14, INK_DIM); lv_obj_set_style_text_letter_space(jobs_header_, 1, 0);
    lv_obj_set_style_pad_left(jobs_header_, 4, 0);
    jobs_list_ = Col(t, 10); lv_obj_set_width(jobs_list_, LV_PCT(100));

    auto bh = Label(t, "BOXES", &font_space_mono_14, INK_DIM); lv_obj_set_style_text_letter_space(bh, 1, 0);
    lv_obj_set_style_pad_left(bh, 4, 0);
    boxes_grid_ = Row(t, 8); lv_obj_set_width(boxes_grid_, LV_PCT(100));
    RenderDashboard();
}

void GwScreens::BuildVoice(lv_obj_t* t) {
    lv_obj_set_style_pad_hor(t, 24, 0); lv_obj_set_style_pad_top(t, 36, 0); lv_obj_set_style_pad_bottom(t, 24, 0);
    lv_obj_add_flag(t, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(t, [](lv_event_t* e) {
        auto self = static_cast<GwScreens*>(lv_event_get_user_data(e));
        if (self->voice_tap_cb_) self->voice_tap_cb_();
    }, LV_EVENT_CLICKED, this);

    auto head = Row(t, 0); lv_obj_set_width(head, LV_PCT(100));
    lv_obj_align(head, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    auto vl = Label(head, "VOICE", &font_space_mono_14, INK_DIM); lv_obj_set_style_text_letter_space(vl, 1, 0);
    voice_clock_ = Label(head, "00:00", &font_space_mono_18, INK_DIM);

    // level bars (animated while listening)
    bars_ = Row(t, 5);
    lv_obj_align(bars_, LV_ALIGN_CENTER, 0, -60);
    lv_obj_set_height(bars_, 30);
    lv_obj_set_flex_align(bars_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    for (int i = 0; i < 7; i++) {
        bar_[i] = lv_obj_create(bars_); Bare(bar_[i]);
        lv_obj_set_size(bar_[i], 6, 10 + (i % 3) * 6);
        lv_obj_set_style_radius(bar_[i], 3, 0);
        lv_obj_set_style_bg_color(bar_[i], C(SKY), 0); lv_obj_set_style_bg_opa(bar_[i], LV_OPA_COVER, 0);
    }
    lv_obj_add_flag(bars_, LV_OBJ_FLAG_HIDDEN);

    voice_hint_ = Label(t, "TAP TO TALK  ·  SAY \"HEY CODER\"", &font_space_mono_14, INK_DIM);
    lv_obj_set_style_text_letter_space(voice_hint_, 1, 0);
    lv_obj_align(voice_hint_, LV_ALIGN_BOTTOM_MID, 0, 0);

    stop_pill_ = MakePill(t, DANGER, "STOP", &font_archivo_bold_22, INK, 0, nullptr);
    lv_obj_set_style_pad_hor(stop_pill_, 34, 0); lv_obj_set_style_pad_ver(stop_pill_, 12, 0);
    lv_obj_align(stop_pill_, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_add_flag(stop_pill_, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_HIDDEN));
    lv_obj_add_event_cb(stop_pill_, [](lv_event_t* e) {
        auto self = static_cast<GwScreens*>(lv_event_get_user_data(e));
        if (self->stop_cb_) self->stop_cb_();
    }, LV_EVENT_CLICKED, this);
}

void GwScreens::AdoptChat(lv_obj_t* bottom_bar, lv_obj_t* chat_label) {
    chat_bar_ = bottom_bar;
    if (!bottom_bar) return;
    lv_obj_set_parent(bottom_bar, voice_tile_);
    lv_obj_set_width(bottom_bar, LV_HOR_RES - 48);
    lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bottom_bar, 0, 0);
    lv_obj_set_style_pad_all(bottom_bar, 0, 0);
    lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, -110);
    if (chat_label) {
        lv_obj_set_style_text_font(chat_label, &font_archivo_bold_22, 0);
        lv_obj_set_style_text_color(chat_label, C(INK), 0);
        lv_obj_set_style_text_align(chat_label, LV_TEXT_ALIGN_CENTER, 0);
    }
}

void GwScreens::BuildOverlay(lv_obj_t* screen) {
    indicator_ = MakePill(screen, PANEL, "LISTENING", &font_space_mono_14, SKY, SKY, &indicator_label_);
    lv_obj_set_style_text_letter_space(indicator_label_, 1, 0);
    lv_obj_align(indicator_, LV_ALIGN_TOP_MID, 0, 10);
    indicator_dot_ = Dot(indicator_, SKY, 10);
    lv_obj_move_to_index(indicator_dot_, 0);
    lv_obj_add_flag(indicator_, LV_OBJ_FLAG_HIDDEN);
}

// ---------- dashboard ------------------------------------------------------------------------
void GwScreens::RenderDashboard() {
    if (!hc_status_) return;
    const char* st = model_.hc_paused ? "Paused" : (model_.hc_status.empty() ? "Offline" : model_.hc_status.c_str());
    std::string cap = st; if (!cap.empty()) cap[0] = toupper(cap[0]);
    lv_label_set_text(hc_status_, cap.c_str());
    lv_obj_set_style_text_color(hc_status_, C(model_.hc_paused ? EMBER : (model_.hc_status == "working" ? GRASS : INK_DIM)), 0);
    lv_label_set_text(hc_wo_, model_.jobs.empty() ? "" : model_.jobs.front().id.c_str());
    lv_label_set_text_fmt(jobs_header_, "JOBS  ·  %d", (int)model_.jobs.size());
    lv_obj_clean(jobs_list_);
    if (model_.jobs.empty()) {
        auto row = Card(jobs_list_, PANEL, 18); lv_obj_set_width(row, LV_PCT(100));
        Label(row, "Nothing running", &font_archivo_bold_22, INK_DIM);
    }
    int shown = 0;
    for (auto& j : model_.jobs) {
        if (shown++ == 3) break;
        bool waiting = j.runtime_status == "waiting";
        auto row = Card(jobs_list_, waiting ? PANEL_LIGHT : PANEL, 18); lv_obj_set_width(row, LV_PCT(100));
        lv_obj_set_style_pad_all(row, 12, 0); lv_obj_set_style_pad_hor(row, 16, 0);
        if (waiting) { lv_obj_set_style_border_width(row, 1, 0); lv_obj_set_style_border_color(row, C(EMBER), 0); }
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW); lv_obj_set_style_pad_column(row, 12, 0);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        Dot(row, StatusColor(j), 12);
        auto col = Col(row, 1); lv_obj_set_flex_grow(col, 1); lv_obj_set_width(col, LV_PCT(85));
        auto title = Label(col, (j.project + "  ·  " + j.provider).c_str(), &font_archivo_bold_22, INK);
        lv_label_set_long_mode(title, LV_LABEL_LONG_DOT); lv_obj_set_width(title, LV_PCT(100));
        Label(col, waiting ? "needs input" : j.id.c_str(), &font_space_mono_16, waiting ? EMBER : INK_DIM);
    }
    lv_obj_clean(boxes_grid_);
    const char* defaults[] = {"baldur", "mac", "qwen", "book"};
    std::vector<DashboardBox> boxes = model_.boxes;
    if (boxes.empty()) for (auto n : defaults) boxes.push_back({n, false});
    for (auto& b : boxes) {
        auto c = Card(boxes_grid_, PANEL, 16); lv_obj_set_flex_grow(c, 1); lv_obj_set_style_pad_all(c, 10, 0);
        lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN); lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(c, 6, 0);
        Dot(c, b.ok ? GRASS : DANGER, 10);
        std::string n = b.box; if (n == "qwenbox") n = "qwen"; if (n == "macbook") n = "book";
        Label(c, n.c_str(), &font_space_mono_14, INK_DIM);
    }
    if (face_jobs_pill_) {
        lv_label_set_text_fmt(face_jobs_pill_, "%d jobs", (int)model_.jobs.size());
        int w = model_.waiting();
        lv_label_set_text_fmt(face_wait_pill_, "%d waiting", w);
        if (w) lv_obj_remove_flag(face_wait_wrap_, LV_OBJ_FLAG_HIDDEN); else lv_obj_add_flag(face_wait_wrap_, LV_OBJ_FLAG_HIDDEN);
    }
}

static std::string S(const cJSON* o, const char* k) { auto v = cJSON_GetObjectItem(o, k); return cJSON_IsString(v) ? v->valuestring : ""; }

void GwScreens::UpdateDashboard(const cJSON* root) {
    DashboardModel m;
    auto hc = cJSON_GetObjectItem(root, "hc");
    if (cJSON_IsObject(hc)) { m.hc_status = S(hc, "status"); m.hc_paused = cJSON_IsTrue(cJSON_GetObjectItem(hc, "paused")) != 0; }
    auto jobs = cJSON_GetObjectItem(root, "jobs"); cJSON* j;
    if (cJSON_IsArray(jobs)) cJSON_ArrayForEach(j, jobs) m.jobs.push_back({S(j, "id"), S(j, "provider"), S(j, "project"), S(j, "status"), S(j, "runtime_status")});
    auto boxes = cJSON_GetObjectItem(root, "boxes"); cJSON* b;
    if (cJSON_IsArray(boxes)) cJSON_ArrayForEach(b, boxes) m.boxes.push_back({S(b, "box"), cJSON_IsTrue(cJSON_GetObjectItem(b, "ok")) != 0});
    model_ = std::move(m);
    RenderDashboard();
}

// ---------- status ---------------------------------------------------------------------------
void GwScreens::SetClock(const char* hhmm, const char* date) {
    if (face_clock_) lv_label_set_text(face_clock_, hhmm);
    if (face_date_) lv_label_set_text(face_date_, date);
    if (dash_clock_) lv_label_set_text(dash_clock_, hhmm);
    if (voice_clock_) lv_label_set_text(voice_clock_, hhmm);
}
void GwScreens::SetBattery(int pct, bool charging) {
    if (!face_batt_) return;
    if (pct <= 0 && !charging) lv_label_set_text(face_batt_, "USB");
    else lv_label_set_text_fmt(face_batt_, "%s%d%%", charging ? "+" : "", pct);
}
void GwScreens::SetHubState(bool online, bool lan) {
    if (!face_hub_dot_) return;
    lv_obj_set_style_bg_color(face_hub_dot_, C(online ? GRASS : INK_DIM), 0);
    lv_label_set_text(face_hub_label_, online ? (lan ? "hub  lan" : "hub  away") : "hub");
}
void GwScreens::SetSleeping(bool sleeping) {
    if (!sleep_pill_) return;
    lv_label_set_text(sleep_label_, sleeping ? "SLEEPING  ·  TAP TO WAKE" : "SLEEP");
    lv_obj_set_style_text_color(sleep_label_, C(sleeping ? EMBER : INK_DIM), 0);
    lv_obj_set_style_border_color(sleep_pill_, C(sleeping ? EMBER : LINE), 0);
}

static void BarsTick(lv_timer_t* tm) {
    auto self = static_cast<lv_obj_t**>(lv_timer_get_user_data(tm));
    for (int i = 0; i < 7; i++) lv_obj_set_height(self[i], 8 + rand() % 24);
}

void GwScreens::SetConversationState(GwConvState s) {
    if (s == conv_) return;
    bool was_active = conv_ != GW_IDLE, active = s != GW_IDLE;
    conv_ = s;
    if (indicator_) {
        if (!active) lv_obj_add_flag(indicator_, LV_OBJ_FLAG_HIDDEN);
        else {
            const char* txt = s == GW_CONNECTING ? "CONNECTING" : s == GW_LISTENING ? "LISTENING" : "SPEAKING";
            uint32_t col = s == GW_SPEAKING ? GOLD : s == GW_LISTENING ? SKY : INK_DIM;
            lv_label_set_text(indicator_label_, txt);
            lv_obj_set_style_text_color(indicator_label_, C(col), 0);
            lv_obj_set_style_border_color(indicator_, C(col), 0);
            lv_obj_set_style_bg_color(indicator_dot_, C(col), 0);
            lv_obj_remove_flag(indicator_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(indicator_);
        }
    }
    if (stop_pill_) { if (active) lv_obj_remove_flag(stop_pill_, LV_OBJ_FLAG_HIDDEN); else lv_obj_add_flag(stop_pill_, LV_OBJ_FLAG_HIDDEN); }
    if (voice_hint_) { if (active) lv_obj_add_flag(voice_hint_, LV_OBJ_FLAG_HIDDEN); else lv_obj_remove_flag(voice_hint_, LV_OBJ_FLAG_HIDDEN); }
    if (bars_) {
        bool listen = s == GW_LISTENING;
        if (listen) {
            lv_obj_remove_flag(bars_, LV_OBJ_FLAG_HIDDEN);
            if (!bars_timer_) bars_timer_ = lv_timer_create(BarsTick, 120, bar_);
        } else {
            lv_obj_add_flag(bars_, LV_OBJ_FLAG_HIDDEN);
            if (bars_timer_) { lv_timer_delete(bars_timer_); bars_timer_ = nullptr; }
        }
    }
    if (active && !was_active) { prev_tile_ = CurrentTile(); GoTo(2); }
    else if (!active && was_active) { GoTo(prev_tile_); }
}

int GwScreens::CurrentTile() const {
    if (!tileview_) return 1;
    auto t = lv_tileview_get_tile_active(tileview_);
    return t == face_tile_ ? 0 : t == voice_tile_ ? 2 : 1;
}
void GwScreens::GoTo(int tile) { if (tileview_) lv_tileview_set_tile_by_index(tileview_, tile, 0, LV_ANIM_ON); }
void GwScreens::NextTile() { GoTo((CurrentTile() + 1) % 3); }

// ---------- toast ----------------------------------------------------------------------------
void GwScreens::ShowToast(const char* level, const char* title, const char* body) {
    auto screen = lv_screen_active();
    if (!toast_) {
        toast_ = Card(screen, PANEL, 28); lv_obj_set_size(toast_, LV_HOR_RES - 36, LV_SIZE_CONTENT);
        lv_obj_align(toast_, LV_ALIGN_BOTTOM_MID, 0, -26); lv_obj_set_style_pad_all(toast_, 18, 0);
        lv_obj_set_style_border_width(toast_, 2, 0); lv_obj_set_style_border_color(toast_, C(GOLD), 0);
        lv_obj_set_flex_flow(toast_, LV_FLEX_FLOW_COLUMN); lv_obj_set_style_pad_row(toast_, 8, 0);
        toast_kind_ = Label(toast_, "", &font_space_mono_14, GRASS);
        toast_title_ = Label(toast_, "", &font_archivo_bold_22, INK); lv_label_set_long_mode(toast_title_, LV_LABEL_LONG_WRAP); lv_obj_set_width(toast_title_, LV_PCT(100));
        toast_body_ = Label(toast_, "", &font_space_mono_16, INK_DIM); lv_label_set_long_mode(toast_body_, LV_LABEL_LONG_WRAP); lv_obj_set_width(toast_body_, LV_PCT(100));
        lv_obj_add_flag(toast_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(toast_, [](lv_event_t* e) { lv_obj_add_flag(static_cast<lv_obj_t*>(lv_event_get_target(e)), LV_OBJ_FLAG_HIDDEN); }, LV_EVENT_CLICKED, nullptr);
    }
    uint32_t kind_color = GRASS; const char* kind = "DONE";
    if (strcmp(level, "warn") == 0) { kind_color = EMBER; kind = "NEEDS INPUT"; }
    else if (strcmp(level, "error") == 0) { kind_color = DANGER; kind = "PROBLEM"; }
    else if (strcmp(level, "note") == 0) { kind_color = INK_DIM; kind = "NOTE"; }
    lv_label_set_text(toast_kind_, kind); lv_obj_set_style_text_color(toast_kind_, C(kind_color), 0);
    lv_label_set_text(toast_title_, title); lv_label_set_text(toast_body_, body);
    if (!body || !*body) lv_obj_add_flag(toast_body_, LV_OBJ_FLAG_HIDDEN); else lv_obj_remove_flag(toast_body_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(toast_, LV_OBJ_FLAG_HIDDEN); lv_obj_move_foreground(toast_);
    if (toast_timer_) lv_timer_delete(toast_timer_);
    toast_timer_ = lv_timer_create([](lv_timer_t* t) { auto self = static_cast<GwScreens*>(lv_timer_get_user_data(t)); lv_obj_add_flag(self->toast_, LV_OBJ_FLAG_HIDDEN); self->toast_timer_ = nullptr; lv_timer_delete(t); }, 12000, this);
    lv_timer_set_repeat_count(toast_timer_, 1);
}

// ---------- video overlay --------------------------------------------------------------------
void GwScreens::ShowVideoFrame(std::unique_ptr<LvglImage> frame) {
    if (!video_img_) {
        auto screen = lv_screen_active();
        video_img_ = lv_image_create(screen);
        lv_obj_set_style_bg_color(video_img_, C(DEEP), 0);
        lv_obj_set_style_bg_opa(video_img_, LV_OPA_COVER, 0);
        lv_obj_set_size(video_img_, LV_HOR_RES, LV_VER_RES);
        lv_obj_center(video_img_);
        lv_image_set_inner_align(video_img_, LV_IMAGE_ALIGN_CENTER);
        lv_obj_add_flag(video_img_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(video_img_, [](lv_event_t* e) {
            auto self = static_cast<GwScreens*>(lv_event_get_user_data(e));
            if (self->video_tap_cb_) self->video_tap_cb_();
        }, LV_EVENT_CLICKED, this);
    }
    int idx = video_frame_idx_;
    video_frames_[idx] = std::move(frame);
    video_frame_idx_ = (idx + 1) % 2;
    auto dsc = video_frames_[idx]->image_dsc();
    lv_image_set_src(video_img_, dsc);
    if (dsc->header.w > 0) lv_image_set_scale(video_img_, (uint32_t)LV_HOR_RES * 256 / dsc->header.w);
    lv_obj_remove_flag(video_img_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(video_img_);
}
void GwScreens::HideVideo() {
    if (video_img_) { lv_obj_add_flag(video_img_, LV_OBJ_FLAG_HIDDEN); lv_image_set_src(video_img_, nullptr); }
    video_frames_[0].reset(); video_frames_[1].reset();
}

// ---------- diagnostics ----------------------------------------------------------------------
static void DumpObj(const char* name, lv_obj_t* o) {
    if (!o) { ESP_LOGI(TAG, "%s: null", name); return; }
    lv_obj_update_layout(o);
    ESP_LOGI(TAG, "%s: x=%d y=%d w=%d h=%d children=%u", name, (int)lv_obj_get_x(o), (int)lv_obj_get_y(o),
             (int)lv_obj_get_width(o), (int)lv_obj_get_height(o), (unsigned)lv_obj_get_child_count(o));
}
void GwScreens::DumpGeometry() {
    DumpObj("face_tile", face_tile_); DumpObj("dash_tile", dash_tile_); DumpObj("voice_tile", voice_tile_);
    DumpObj("face_clock", face_clock_); DumpObj("jobs_list", jobs_list_); DumpObj("chat_bar", chat_bar_);
}
