#include "gw_theme.h"
#include "settings.h"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(font_material_symbols_30_4);
LV_FONT_DECLARE(font_noto_emoji_30_4);

namespace gw {

LvglTheme* MakeTheme() {
    auto& mgr = LvglThemeManager::GetInstance();
    if (auto existing = mgr.GetTheme("grayson")) {
        return existing;
    }
    auto t = new LvglTheme("grayson");
    t->set_background_color(C(DEEP));
    t->set_text_color(C(INK));
    t->set_chat_background_color(C(DEEP));
    t->set_user_bubble_color(C(PANEL_LIGHT));
    t->set_assistant_bubble_color(C(PANEL));
    t->set_system_bubble_color(C(DEEP));
    t->set_system_text_color(C(INK_DIM));
    t->set_border_color(C(LINE));
    t->set_low_battery_color(C(DANGER));
    t->set_text_font(std::make_shared<LvglBuiltInFont>(&BUILTIN_TEXT_FONT));
    t->set_icon_font(std::make_shared<LvglBuiltInFont>(&BUILTIN_ICON_FONT));
    t->set_large_icon_font(std::make_shared<LvglBuiltInFont>(&font_material_symbols_30_4));
    t->set_emoji_font(std::make_shared<LvglBuiltInFont>(&font_noto_emoji_30_4));
    mgr.RegisterTheme("grayson", t);
    return t;
}

void SelectThemeInSettings() {
    Settings settings("display", true);
    if (settings.GetString("theme", "") != "grayson") {
        settings.SetString("theme", "grayson");
    }
}

}  // namespace gw
