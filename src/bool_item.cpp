/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <exception>

#include "wupsxx/bool_item.hpp"

#include "nintendo_glyphs.h"


namespace wups::config {

    bool_item::bool_item(const std::string& label,
                         bool& variable,
                         bool default_value,
                         const std::string& true_str,
                         const std::string& false_str) :
        var_item{label, variable, default_value},
        true_str{true_str},
        false_str{false_str}
    {}


    std::unique_ptr<bool_item>
    bool_item::create(const std::string& label,
                      bool& variable,
                      bool default_value,
                      const std::string& true_str,
                      const std::string& false_str)
    {
        return std::make_unique<bool_item>(label,
                                           variable, default_value,
                                           true_str, false_str);
    }


    int
    bool_item::get_display(char* buf, std::size_t size)
        const
    {
        std::snprintf(buf, size, "%s",
                      variable ? true_str.c_str() : false_str.c_str());
        return 0;
    }


    int
    bool_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        const char* str = variable ? true_str.c_str() : false_str.c_str();

        std::snprintf(buf, size,
                      "%s %s %s",
                      NIN_GLYPH_BTN_DPAD_LEFT,
                      str,
                      NIN_GLYPH_BTN_DPAD_RIGHT);
        return 0;
    }


    FocusChange
    bool_item::on_input(const SimplePadData& input)
    {
        // Allow toggling wit left or right.
        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT | WUPS_CONFIG_BUTTON_RIGHT))
            variable = !variable;

        return var_item::on_input(input);
    }

} // namespace wups::config
