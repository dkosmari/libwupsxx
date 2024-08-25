/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <cstdio>
#include <optional>
#include <ranges>

#include "wupsxx/color_item.hpp"

#include "cafe_glyphs.h"


using std::array;
using std::views::enumerate;
using std::optional;


namespace wups::config {

    color_item::color_item(const std::string& label,
                           color& variable,
                           color default_value,
                           bool has_alpha) :
        var_item{label, variable, default_value},
        has_alpha{has_alpha},
        mode{mode_t::rgb},
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



    namespace {

        std::string
        get_rgb_str(color c,
                    bool has_alpha,
                    optional<unsigned> edit_idx = {})
        {
            auto make_comp_str = [=](unsigned idx,
                                     const std::string& label)
                -> std::string
            {
                if (edit_idx && *edit_idx == idx)
                    return label + "[" + std::to_string(c[idx]) + "]";
                else
                    return label + std::to_string(c[idx]);
            };

            std::string result = has_alpha ? "rgba(" : "rgb(";
            for (auto [idx, label] : enumerate(array{"", ", ", ", "}))
                result += make_comp_str(idx, label);

            if (has_alpha)
                result += make_comp_str(3, ", ");

            return result + ")";
        }


        std::string
        comp_to_hex(std::uint8_t val)
        {
            char buf[3];
            std::snprintf(buf, sizeof buf, "%02X", unsigned{val});
            return buf;
        }


        std::string
        get_hex_str(color c,
                    bool has_alpha,
                    optional<unsigned> edit_idx = {})
        {
            const unsigned num_components = has_alpha ? 4 : 3;
            std::string result = "#";
            for (unsigned idx = 0; idx < num_components; ++idx)
                if (edit_idx && *edit_idx == idx)
                    result += "[" + comp_to_hex(c[idx]) + "]";
                else
                    result += comp_to_hex(c[idx]);
            return result;
        }


    } // namespace


    int
    color_item::get_display(char* buf, std::size_t size)
        const
    {
        std::string str;
        switch (mode) {
        case mode_t::rgb:
            str = get_rgb_str(variable, has_alpha);
            break;
        case mode_t::hex:
            str = get_hex_str(variable, has_alpha);
            break;
        }
        std::snprintf(buf, size, "%s", str.c_str());
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
        if (edit_idx == max_edit_idx)
            left_right = CAFE_GLYPH_BTN_LEFT;

        const char* up_down = CAFE_GLYPH_BTN_UP_DOWN;
        if (variable[edit_idx] == 0)
            up_down = CAFE_GLYPH_BTN_UP;
        if (variable[edit_idx] == 0xff)
            up_down = CAFE_GLYPH_BTN_DOWN;

        std::string str;
        switch (mode) {
        case mode_t::rgb:
            str = get_rgb_str(variable, has_alpha, edit_idx);
            break;
        case mode_t::hex:
            str = get_hex_str(variable, has_alpha, edit_idx);
            break;
        }

        std::snprintf(buf, size,
                      "%s %s %s",
                      left_right,
                      str.c_str(),
                      up_down);

        return 0;
    }


    focus_status
    color_item::on_input(const simple_pad_data& input)
    {
        const unsigned max_edit_idx = has_alpha ? 3 : 2;

        if (input.buttons_d & WUPS_CONFIG_BUTTON_Y) {
            if (mode == mode_t::rgb)
                mode = mode_t::hex;
            else
                mode = mode_t::rgb;
        }

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT)) {
            if (edit_idx > 0)
                --edit_idx;
        }

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_RIGHT)) {
            if (edit_idx < max_edit_idx)
                ++edit_idx;
        }

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
