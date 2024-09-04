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

#include "wupsxx/cafe_glyphs.h"


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


    void
    bool_item::get_display(char* buf, std::size_t size)
        const
    {
        std::snprintf(buf, size, "%s",
                      variable ? true_str.c_str() : false_str.c_str());
    }


    void
    bool_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        const char* str = variable ? true_str.c_str() : false_str.c_str();
        std::snprintf(buf, size,
                      "%s %s %s",
                      CAFE_GLYPH_BTN_LEFT,
                      str,
                      CAFE_GLYPH_BTN_RIGHT);
    }


    focus_status
    bool_item::on_input(const simple_pad_data& input)
    {
        // Allow toggling with left or right.
        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT | WUPS_CONFIG_BUTTON_RIGHT))
            variable = !variable;

        return var_item::on_input(input);
    }

} // namespace wups::config
