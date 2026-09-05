#pragma once
#include "display/lvgl_display/lvgl_theme.h"
#include <lvgl.h>

LV_FONT_DECLARE(font_archivo_black_96);
LV_FONT_DECLARE(font_archivo_bold_22);
LV_FONT_DECLARE(font_space_mono_14);
LV_FONT_DECLARE(font_space_mono_18);
LV_IMAGE_DECLARE(g_mark_160);

namespace gw {
// Grayson Work palette (from the Grayson Games site theme)
constexpr uint32_t GOLD = 0xFFDF64, GOLD_DEEP = 0xD9B021, DEEP = 0x050D12, PANEL = 0x0C1C24,
                   PANEL_LIGHT = 0x122935, LINE = 0x1C3744, INK = 0xF4F7F8, INK_DIM = 0x8EA6B0,
                   GRASS = 0x3AD15A, EMBER = 0xFF7A45, DANGER = 0xFF5566, SKY = 0x5AD1FF;

inline lv_color_t C(uint32_t hex) { return lv_color_hex(hex); }

// Build and register the "grayson" LVGL theme (idempotent). Call before the display is
// constructed so LcdDisplay can pick it up from Settings("display").theme at startup.
LvglTheme* MakeTheme();
// Make "grayson" the persisted display theme if it is not already.
void SelectThemeInSettings();
}  // namespace gw
