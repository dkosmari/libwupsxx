/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_ITEM_HPP
#define WUPSXX_ITEM_HPP

#include <cstddef>              // size_t
#include <string>

#include <wups/config/WUPSConfigItem.h>

#include "input.hpp"


namespace wups::config {

    enum class focus_status {
        lose,
        keep,
        change_input,
    };


    enum class input_mode {
        simple,
        to_complex,
        complex
    };


    class item {

        WUPSConfigItemHandle handle;
        bool focused;
        input_mode current_mode;

    protected:

        item(const std::string& label);

        // Disallow moving, since the callbacks store the `this` pointer.
        item(item&&) = delete;

    public:

        virtual ~item();

        // Gives up ownership of the handle.
        void release() noexcept;

        // Destruction started from within WUPS.
        void on_delete() noexcept;


        virtual void get_display(char* buf, std::size_t size) const;

        virtual void get_focused_display(char* buf, std::size_t size) const;

        // return `true` if item allows the focus change to occur
        virtual bool on_focus_request(bool new_focus) const;

        virtual void on_focus_changed();

        virtual void restore_default();

        virtual void on_close();

        virtual focus_status on_input(const simple_pad_data& input);

        virtual focus_status on_input(const complex_pad_data& input);


        bool has_focus() const noexcept;

        virtual void set_focus(bool new_focus);


        input_mode get_input_mode() const noexcept;
        void set_input_mode(input_mode mode) noexcept;

        friend class category;

    };

} // namespace wups::config

#endif
