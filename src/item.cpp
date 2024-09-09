/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>               // snprintf()

#include <whb/log.h>

#include <wups/config_api.h>

#include "wupsxx/item.hpp"

#include "wupsxx/config_error.hpp"


#define REPORT_ERROR(e) \
    WHBLogPrintf("[libwupsxx] error in %s(): %s\n", __func__, e.what())


namespace wups::config {

    // Constant used by both input functions:
    // How long a button must be held until it's in a "repeat" state.


    namespace glue {

        int32_t
        get_display(void* ctx, char* buf, int32_t size)
            noexcept
        {
            try {
                auto it = static_cast<const item*>(ctx);
                it->get_display(buf, size);
                return 0;
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
                return -1;
            }
        }


        int32_t
        get_selected_display(void* ctx, char* buf, int32_t size)
            noexcept
        {
            try {
                auto it = static_cast<const item*>(ctx);
                if (it->has_focus())
                    it->get_focused_display(buf, size);
                else
                    it->get_display(buf, size);
                return 0;
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
                return -1;
            }
        }


        bool
        is_movement_allowed(void* ctx)
            noexcept
        {
            try {
                auto it = static_cast<const item*>(ctx);
                return !it->has_focus();
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
                return true;
            }
        }


        void
        on_close(void* ctx)
            noexcept
        {
            try {
                auto it = static_cast<item*>(ctx);
                it->on_close();
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
            }
        }


        void
        on_delete(void* ctx)
            noexcept
        {
            try {
                auto it = static_cast<item*>(ctx);
                it->on_delete();
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
            }
        }


        void
        on_input(void* ctx, WUPSConfigSimplePadData input)
            noexcept
        {
            auto it = static_cast<item*>(ctx);

            try {
                if (!it->has_focus()) {
                    // when not focused, the only useful input is A to focus it
                    if (input.buttons_d & WUPS_CONFIG_BUTTON_A)
                        it->set_focus(true);
                    return;
                }

                if (it->get_input_mode() != input_mode::simple)
                    return;

                simple_pad_data sinput{input};
                auto res = it->on_input(sinput);
                switch (res) {
                case focus_status::lose:
                    it->set_focus(false);
                    break;
                case focus_status::keep:
                    break;
                case focus_status::change_input:
                    it->set_input_mode(input_mode::to_complex);
                    break;
                }
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
                it->set_focus(false);
            }
        }


        void
        on_input_ex(void* ctx, WUPSConfigComplexPadData input)
            noexcept
        {
            auto it = static_cast<item*>(ctx);
            try {
                // don't handle focus-in from extended input, use simple input for that
                if (!it->has_focus())
                    return;

                if (it->get_input_mode() == input_mode::to_complex) {
                    // ignore this input, will process the next one
                    it->set_input_mode(input_mode::complex);
                    return;
                }

                if (it->get_input_mode() != input_mode::complex)
                    return;

                complex_pad_data cinput{input};
                auto res = it->on_input(cinput);
                switch (res) {
                case focus_status::lose:
                    it->set_focus(false);
                    break;
                case focus_status::keep:
                    break;
                case focus_status::change_input:
                    it->set_input_mode(input_mode::simple);
                    break;
                }
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
                it->set_focus(false);
            }
        }


        void
        on_selected(void* /*ctx*/, bool /*is_selected*/)
            noexcept
        {}


        void
        restore_default(void* ctx)
            noexcept
        {
            try {
                auto it = static_cast<item*>(ctx);
                it->restore_default();
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
            }
        }

    } // namespace glue


    item::item(const std::string& label) :
        focused{false},
        current_mode{input_mode::simple}
    {
        WUPSConfigAPIItemOptionsV2 options {
            .displayName = label.c_str(),
            .context = this,
            .callbacks = {
                // Note: do not sort, must be initialized in the order of declaration.
                .getCurrentValueDisplay         = glue::get_display,
                .getCurrentValueSelectedDisplay = glue::get_selected_display,
                .onSelected                     = glue::on_selected,
                .restoreDefault                 = glue::restore_default,
                .isMovementAllowed              = glue::is_movement_allowed,
                .onCloseCallback                = glue::on_close,
                .onInput                        = glue::on_input,
                .onInputEx                      = glue::on_input_ex,
                .onDelete                       = glue::on_delete,
            }
        };

        auto status = WUPSConfigAPI_Item_Create(options, &handle);
        if (status != WUPSCONFIG_API_RESULT_SUCCESS)
            throw config_error{status, "could not create config item \"" + label + "\""};
    }


    item::~item()
    {
        if (handle.handle) {
            /* How to get here:
             *   - Destructor was called from user code.
             */
            auto h = handle;
            release(); // this prevents WUPS API from looping back here
            WUPSConfigAPI_Item_Destroy(h);
        }
    }


    void
    item::release()
        noexcept
    {
        handle = {};
    }


    void
    item::on_delete()
        noexcept
    {
        if (!handle.handle) {
            /* How to get here:
             *   - user code destroyed wups::item;
             *   - item::~item() sees a valid handle:
             *     - calls release();
             *     - calls WUPSConfigAPI_Item_Destroy();
             *       - WUPS calls the on_delete() callback, ends up in here.
             */
            return;
        } else {
            /* How to get here:
             *   - WUPS wants to destroy the item:
             *     - calls on_delete() callback, ends up here.
             */
            release(); // this avoids calling WUPSConfigAPI_Item_Destroy() on the destructor.
            delete this;
        }
    }


    void
    item::get_display(char* buf,
                      std::size_t size)
        const
    {
        std::snprintf(buf, size, "NOT IMPLEMENTED");
    }


    void
    item::get_focused_display(char* buf,
                              std::size_t size)
        const
    {
        get_display(buf, size);
    }


    bool
    item::on_focus_request(bool /*new_focus*/)
        const
    {
        return true;
    }


    void
    item::on_focus_changed()
    {}


    void
    item::restore_default()
    {}


    void
    item::on_close()
    {}


    focus_status
    item::on_input(const simple_pad_data& /*input*/)
    {
        return focus_status::lose;
    }


    focus_status
    item::on_input(const complex_pad_data& /*input*/)
    {
        return focus_status::lose;
    }


    bool
    item::has_focus()
        const noexcept
    {
        return focused;
    }


    void
    item::set_focus(bool new_focus)
    {
        if (focused == new_focus)
            return;

        // allow derived class to refuse changing focus
        if (on_focus_request(new_focus)) {
            focused = new_focus;
            if (focused) // always enter focus on simple mode
                current_mode = input_mode::simple;
            on_focus_changed();
        }
    }


    input_mode
    item::get_input_mode()
        const noexcept
    {
        return current_mode;
    }


    void
    item::set_input_mode(input_mode mode)
        noexcept
    {
        current_mode = mode;
    }


} // namespace wups::config
