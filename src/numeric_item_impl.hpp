/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_NUMERIC_ITEM_IMPL_HPP
#define WUPSXX_NUMERIC_ITEM_IMPL_HPP

#include <algorithm>            // clamp()
#include <chrono>
#include <cstdio>               // snprintf()
#include <exception>
#include <string.h>             // BSD strlcpy()

#include "wupsxx/numeric_item.hpp"

#include "wupsxx/cafe_glyphs.h"
#include "wupsxx/duration.hpp" // make to_string() overload visible



namespace wups::config {

    template<typename T>
    numeric_item<T>::numeric_item(const std::string& label,
                                  T& variable, T default_value,
                                  T min_value, T max_value,
                                  T fast_increment, T slow_increment) :
        var_item<T>{label, variable, default_value},
        min_value{min_value},
        max_value{max_value},
        fast_increment{fast_increment},
        slow_increment{slow_increment}
    {}


    template<typename T>
    std::unique_ptr<numeric_item<T>>
    numeric_item<T>::create(const std::string& label,
                            T& variable, T default_value,
                            T min_value, T max_value,
                            T fast_increment, T slow_increment)
    {
        return std::make_unique<numeric_item<T>>(label,
                                                 variable, default_value,
                                                 min_value, max_value,
                                                 fast_increment, slow_increment);
    }


    template<typename T>
    void
    numeric_item<T>::get_display(char* buf, std::size_t size)
        const
    {
        using std::to_string;
        using wups::config::to_string;
        std::string str = to_string(variable);
        ::strlcpy(buf, str.c_str(), size);
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
        using std::to_string;
        using wups::config::to_string;
        std::string str = to_string(variable);
        std::snprintf(buf, size,
                      "%s%s" "%s" "%s%s",
                      fast_left,
                      slow_left,
                      str.c_str(),
                      slow_right,
                      fast_right);
    }


    template<typename T>
    focus_status
    numeric_item<T>::on_input(const simple_pad_data& input)
    {
        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT))
            variable -= slow_increment;

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_RIGHT))
            variable += slow_increment;

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_L))
            variable -= fast_increment;

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_R))
            variable += fast_increment;

        variable = std::clamp(variable, min_value, max_value);

        return var_item<T>::on_input(input);
    }

} // namespace wups::config


#endif
