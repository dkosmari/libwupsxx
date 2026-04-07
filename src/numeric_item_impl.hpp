/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_NUMERIC_ITEM_IMPL_HPP
#define WUPSXX_NUMERIC_ITEM_IMPL_HPP

#include <algorithm>            // clamp()
#include <chrono>
#include <cstdio>               // snprintf()
#include <exception>

#include "wupsxx/numeric_item.hpp"

#include "wupsxx/cafe_glyphs.h"
#include "wupsxx/duration.hpp" // make to_string() overload visible


namespace wups {

    template<typename T>
    numeric_item<T>::numeric_item(option<T>& opt,
                                  const specs& options_) :
        var_item<T>{opt},
        min_value{opt.min_value},
        max_value{opt.max_value},
        options{options_}
    {}


    template<typename T>
    std::unique_ptr<numeric_item<T>>
    numeric_item<T>::create(option<T>& opt,
                            const specs& options)
    {
        return std::make_unique<numeric_item<T>>(opt, options);
    }


    template<typename T>
    void
    numeric_item<T>::get_display(char* buf, std::size_t size)
        const
    {
        if (options.format.empty()) {
            using std::to_string;
            using wups::to_string;
            const std::string variable_str = to_string(variable);
            std::snprintf(buf, size, "%s", variable_str.data());
        } else {
            std::snprintf(buf, size, options.format.data(), variable);
        }
    }


    template<typename T>
    void
    numeric_item<T>::get_focused_display(char* buf, std::size_t size)
        const
    {
        const char* slow_left = "";
        const char* slow_right = "";
        const char* fast_left = "";
        const char* fast_right = "";
        if (variable > min_value) {
            slow_left = CAFE_GLYPH_BTN_LEFT " ";
            fast_left = CAFE_GLYPH_BTN_L;
        } if (variable < max_value) {
            slow_right = " " CAFE_GLYPH_BTN_RIGHT;
            fast_right = CAFE_GLYPH_BTN_R;
        }

        if (options.format.empty()) {
            using std::to_string;
            using wups::to_string;
            const std::string variable_str = to_string(variable);
            std::snprintf(buf, size,
                          "%s%s" "%s" "%s%s",
                          fast_left,
                          slow_left,
                          variable_str.data(),
                          slow_right,
                          fast_right);
        } else {
            const std::string format = "%s%s" + options.format + "%s%s";
            std::snprintf(buf, size,
                          format.data(),
                          fast_left,
                          slow_left,
                          variable,
                          slow_right,
                          fast_right);
        }
    }


    template<typename T>
    focus_status
    numeric_item<T>::on_input(const simple_pad_data& input)
    {
        if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_LEFT))
            variable -= options.slow_increment;

        if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_RIGHT))
            variable += options.slow_increment;

        if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_L))
            variable -= options.fast_increment;

        if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_R))
            variable += options.fast_increment;

        variable = std::clamp(variable, min_value, max_value);

        if (options.round_to_format && !options.format.empty()) {
            char buf[64];
            std::snprintf(buf, sizeof buf, options.format.data(), variable);
            std::sscanf(buf, options.format.data(), &variable);
        }

        return var_item<T>::on_input(input);
    }

} // namespace wups

#endif
