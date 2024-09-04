/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>            // clamp(), min()
#include <cstdio>               // snprintf()

#include "wupsxx/text_item.hpp"

#include "wupsxx/cafe_glyphs.h"


namespace {

    const std::string left_glyph = CAFE_GLYPH_BTN_LEFT " ";
    const std::string right_glyph = " " CAFE_GLYPH_BTN_RIGHT;

}


namespace wups::config {

    text_item::text_item(const std::string& label,
                         const std::string& text,
                         std::size_t max_width) :
        item{label},
        text{text},
        max_width{std::min<std::size_t>(max_width, 79)}
    {}


    std::unique_ptr<text_item>
    text_item::create(const std::string& label,
                      const std::string& text,
                      std::size_t max_width)
    {
        return std::make_unique<text_item>(label, text, max_width);
    }


    void
    text_item::get_display(char* buf,
                           std::size_t size)
        const
    {
        // TODO: process this in std::u32string

        // Note: `buf` is a C string, it needs a null terminator at the end,
        // so the effective `width` is one less than `size`.
        std::size_t width = std::min(size - 1, max_width);

        if (width >= text.size()) {
            // Easy case: text fits, just show it all.
            std::snprintf(buf, size, "%s", text.c_str());
            return;
        }

        const char* ellipsis = "…";

        std::string prefix;
        if (first > 0)
            prefix = ellipsis;
        if (width < prefix.size()) // sanity check
            return;
        width -= prefix.size();

        std::size_t last = first + width;
        std::string suffix;
        if (last < text.size())
            suffix = ellipsis;
        if (width < suffix.size()) // sanity check
            return;
        width -= suffix.size();

        std::string slice = text.substr(first, width);

        std::snprintf(buf, size,
                      "%s%s%s",
                      prefix.c_str(),
                      slice.c_str(),
                      suffix.c_str());
    }


    void
    text_item::get_focused_display(char* buf,
                                   std::size_t size)
        const
    {
        // TODO: process this in std::u32string

        // Note: `buf` is a C string, it needs a null terminator at the end,
        // so the effective `width` is one less than `size`.
        std::size_t width = std::min(size - 1, max_width);

        if (width >= text.size()) {
            // Easy case: text fits, just show it all.
            std::snprintf(buf, size, "%s", text.c_str());
            return;
        }

        std::string prefix;
        if (first > 0)
            prefix = left_glyph;
        if (width < prefix.size()) // sanity check
            return;
        width -= prefix.size();

        std::size_t last = first + width;
        std::string suffix;
        if (last < text.size())
            suffix = right_glyph;
        if (width < suffix.size()) // sanity check
            return;
        width -= suffix.size();

        std::string slice = text.substr(first, width);

        std::snprintf(buf, size,
                      "%s%s%s",
                      prefix.c_str(),
                      slice.c_str(),
                      suffix.c_str());
    }


    bool
    text_item::on_focus_request(bool new_focus)
        const
    {
        // Don't let small text be focused, there's no scrolling.
        if (new_focus && text.size() <= max_width)
            return false;
        return true;
    }


    focus_status
    text_item::on_input(const simple_pad_data& input)
    {
        // Only process inputs if text is not fully visible.
        if (text.size() > max_width) {

            // Handle text scrolling

            const std::size_t max_first = text.size() - max_width + left_glyph.size();

            if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT))
                if (first > 0)
                    --first;

            if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_RIGHT))
                if (first < max_first)
                    ++first;

            if (input.buttons_d & WUPS_CONFIG_BUTTON_L)
                first = 0;

            if (input.buttons_d & WUPS_CONFIG_BUTTON_R)
                first = max_first;
        }

        return item::on_input(input);
    }

} // namespace wups::config
