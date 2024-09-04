/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>

#include "wupsxx/button_item.hpp"

#include "wupsxx/cafe_glyphs.h"


namespace wups::config {

    button_item::button_item(const std::string& label) :
        item{label},
        current_state{state::normal}
    {}


    std::unique_ptr<button_item>
    button_item::create(const std::string& label)
    {
        return std::make_unique<button_item>(label);
    }


    void
    button_item::get_display(char* buf, std::size_t size)
        const
    {
        if (!status_msg.empty())
            std::snprintf(buf, size,
                          "%s (Press " CAFE_GLYPH_BTN_A ")",
                          status_msg.c_str());
        else
            buf[0] = '\0';
    }


    void
    button_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        switch (current_state.load()) {
        case state::normal:
        case state::finished:
            if (status_msg.empty())
                std::snprintf(buf, size,
                              "(Press " CAFE_GLYPH_BTN_A ")");
            else
                std::snprintf(buf, size,
                              "%s (Press " CAFE_GLYPH_BTN_A ")",
                              status_msg.c_str());
            break;
        case state::started:
            if (status_msg.empty())
                std::snprintf(buf, size,
                              "(Press " CAFE_GLYPH_BTN_B " to cancel )");
            else
                std::snprintf(buf, size,
                              "%s (Press " CAFE_GLYPH_BTN_B " to cancel )",
                              status_msg.c_str());
            break;
        default:
            buf[0] = '\0';
        }
    }


    bool
    button_item::on_focus_request(bool)
        const
    {
        if (current_state == state::started)
            return false;
        return true;
    }


    void
    button_item::on_focus_changed()
    {
        if (has_focus()) {
            current_state = state::started;
            on_started();
        } else {
            current_state = state::normal;
        }
    }


    focus_status
    button_item::on_input(const simple_pad_data& input)
    {
        if (current_state == state::finished) {
            on_finished();
            return focus_status::lose;
        }

        if (input.buttons_d & WUPS_CONFIG_BUTTON_B)
            if (current_state == state::started)
                on_cancel();

        return focus_status::keep;
    }


    void
    button_item::on_started()
    {}


    void
    button_item::on_finished()
    {}


    void
    button_item::on_cancel()
    {}

} // namespace wups::config
