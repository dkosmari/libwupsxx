/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>

#include "wupsxx/color_item.hpp"

#include "cafe_glyphs.h"


namespace wups::config {

    color_item::color_item(const std::string& label,
                           color& variable,
                           color default_value,
                           bool has_alpha) :
        var_item{label, variable, default_value},
        has_alpha{has_alpha},
        edit_idx{0}
    {}


    std::unique_ptr<color_item>
    color_item::create(const std::string& label,
                       color& variable,
                       color default_value,
                       bool has_alpha)
    {
        return std::make_unique<color_item>(label,
                                            variable, default_value,
                                            has_alpha);
    }


    int
    color_item::get_display(char* buf, std::size_t size)
        const
    {
        auto s = to_string(variable, has_alpha);
        std::snprintf(buf, size, "%s", s.c_str());
        return 0;
    }


    int
    color_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        const unsigned max_edit_idx = has_alpha ? 3 : 2;
        const char* left_right = CAFE_GLYPH_BTN_LEFT_RIGHT;
        if (edit_idx == 0)
            left_right = CAFE_GLYPH_BTN_RIGHT;
        if (edit_idx >= max_edit_idx)
            left_right = CAFE_GLYPH_BTN_LEFT;

        auto channel = variable[edit_idx];
        const char* up_down = CAFE_GLYPH_BTN_UP_DOWN;
        if (channel == 0)
            up_down = CAFE_GLYPH_BTN_UP;
        if (channel >= 0xff)
            up_down = CAFE_GLYPH_BTN_DOWN;

        auto s = to_string(variable, has_alpha);
        const char* left_bracket = "[";
        const char* right_bracket = "]";
        s.insert(1 + 2 + edit_idx * 2, right_bracket);
        s.insert(1 + 0 + edit_idx * 2, left_bracket);
        std::snprintf(buf, size,
                      "%s %s %s",
                      left_right,
                      s.c_str(),
                      up_down);
        return 0;
    }


    focus_status
    color_item::on_input(const simple_pad_data& input)
    {
        const unsigned max_edit_idx = has_alpha ? 3 : 2;

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT | WUPS_CONFIG_BUTTON_L))
            if (edit_idx > 0)
                --edit_idx;

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_RIGHT | WUPS_CONFIG_BUTTON_R))
            if (edit_idx < max_edit_idx)
                ++edit_idx;

        auto& channel = variable[edit_idx];

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_UP))
            if (channel < 0xff)
                ++channel;

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_DOWN))
            if (channel > 0)
                --channel;

        return var_item::on_input(input);
    }

} // namespace wups::config
