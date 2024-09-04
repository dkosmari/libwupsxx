/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_BUTTON_ITEM_HPP
#define WUPSXX_BUTTON_ITEM_HPP

#include <atomic>
#include <string>
#include <memory>

#include "item.hpp"


namespace wups::config {

    struct button_item : item {


        enum class state {
            normal,
            started,
            finished,
        };

        std::atomic<state> current_state;
        std::string status_msg;


        button_item(const std::string& label);


        static
        std::unique_ptr<button_item>
        create(const std::string& label);


        virtual
        void
        get_display(char* buf, std::size_t size) const override;


        virtual
        void
        get_focused_display(char* buf, std::size_t size) const override;


        virtual
        bool
        on_focus_request(bool new_focus) const override;


        virtual
        void
        on_focus_changed() override;


        virtual
        focus_status
        on_input(const simple_pad_data& input) override;


        virtual
        void
        on_started();


        virtual
        void
        on_finished();


        virtual
        void
        on_cancel();

    };


} // namespace wups::config

#endif
