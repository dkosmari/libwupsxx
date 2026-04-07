/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <exception>

#include "wupsxx/bool_item.hpp"

#include "wupsxx/cafe_glyphs.h"


namespace wups {

    bool_item::bool_item(option<bool>& opt,
                         const specs& options_) :
        var_item{opt},
        options{options_}
    {}


    std::unique_ptr<bool_item>
    bool_item::create(option<bool>& opt,
                      const specs& options)
    {
        return std::make_unique<bool_item>(opt, options);
    }


    void
    bool_item::get_display(char* buf,
                           std::size_t size)
        const
    {
        const char* str = variable
                          ? options.true_label.data()
                          : options.false_label.data();
        std::snprintf(buf, size, "%s", str);
    }


    void
    bool_item::get_focused_display(char* buf,
                                   std::size_t size)
        const
    {
        const char* left  =  variable ? CAFE_GLYPH_BTN_LEFT  : CAFE_GLYPH_BTN_DPAD;
        const char* right = !variable ? CAFE_GLYPH_BTN_RIGHT : CAFE_GLYPH_BTN_DPAD;
        const char* str = variable
                          ? options.true_label.data()
                          : options.false_label.data();
        std::snprintf(buf, size,
                      "%s %s %s",
                      left,
                      str,
                      right);
    }


    focus_status
    bool_item::on_input(const simple_pad_data& input)
    {
        if (variable) {
            // left can turn it off
            if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_LEFT))
                variable = false;
        } else {
            // right can turn it on
            if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_RIGHT))
                variable = true;
        }

        return var_item::on_input(input);
    }


    std::unique_ptr<bool_item>
    make_item(option<bool>& opt,
              const bool_item::specs& options)
    {
        return bool_item::create(opt, options);
    }

} // namespace wups
