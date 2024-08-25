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
                return it->get_display(buf, size);
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
                    return it->get_focused_display(buf, size);
                else
                    return it->get_display(buf, size);
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
                it->release(); // don't destroy the handle, it's already happening
                delete it;
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

                simple_pad_data sinput{input};

                if (it->on_input(sinput) == focus_status::lose)
                    it->set_focus(false);
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

                complex_pad_data cinput{input};

                if (it->on_input(cinput) == focus_status::lose)
                    it->set_focus(false);
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
                it->restore();
            }
            catch (std::exception& e) {
                REPORT_ERROR(e);
            }
        }

    } // namespace dispatchers


    item::item(const std::string& label) :
        focused{false}
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
        if (handle.handle)
            WUPSConfigAPI_Item_Destroy(handle);
    }


    void
    item::release()
        noexcept
    {
        handle = {};
    }


    int
    item::get_display(char* buf,
                      std::size_t size)
        const
    {
        std::snprintf(buf, size, "NOT IMPLEMENTED");
        return 0;
    }


    int
    item::get_focused_display(char* buf,
                              std::size_t size)
        const
    {
        return get_display(buf, size);
    }


    bool
    item::on_focus_request(bool /*new_focus*/)
        const
    {
        return true; // always allow changing focus state
    }


    void
    item::on_focus_changed()
    {}


    void
    item::restore()
    {}


    void
    item::on_close()
    {}


    focus_status
    item::on_input(const simple_pad_data& input)
    {
        if (input.buttons_d & WUPS_CONFIG_BUTTON_X)
            restore();

        // either A or B make it lose focus, confirming or canceling
        if (input.buttons_d & (WUPS_CONFIG_BUTTON_A | WUPS_CONFIG_BUTTON_B))
            return focus_status::lose;

        return focus_status::keep;
    }


    focus_status
    item::on_input(const complex_pad_data& /*input*/)
    {
        return focus_status::keep;
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

        if (on_focus_request(new_focus)) {
            focused = new_focus;
            on_focus_changed();
        }
    }


} // namespace wups::config
