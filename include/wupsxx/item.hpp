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

#include <wups.h>


namespace wups::config {


    struct SimplePadData : WUPSConfigSimplePadData {

        WUPS_CONFIG_SIMPLE_INPUT buttons_repeat;


        SimplePadData(const WUPSConfigSimplePadData& base) noexcept;


        bool pressed_or_repeated(unsigned mask) const noexcept;

    };


    enum class FocusChange {
        Keep,
        Lose,
    };


    class item {

        WUPSConfigItemHandle handle;

        bool focused;

    public:

        item(const std::string& label);

        // Disallow moving, since the callbacks store the `this` pointer.
        item(item&&) = delete;

        virtual ~item();

        // Gives up ownership of the handle.
        void release() noexcept;


        virtual int get_display(char* buf, std::size_t size) const;

        virtual int get_focused_display(char* buf, std::size_t size) const;

        // return `true` if item allows the focus change to occur
        virtual bool on_focus_request(bool new_focus) const;

        virtual void restore();

        virtual void on_close();

        virtual FocusChange on_input(const SimplePadData& input);

        virtual FocusChange on_input(const WUPSConfigComplexPadData& input);


        bool has_focus() const noexcept;

        virtual void set_focus(bool new_focus);


        friend class category;

    };

} // namespace wups::config

#endif
