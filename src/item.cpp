/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <chrono>
#include <cstdio>               // snprintf()

#include <whb/log.h>

#include "wupsxx/item.hpp"

#include "wupsxx/config_error.hpp"


using namespace std::literals;


#define REPORT_ERROR(e) \
    WHBLogPrintf("[libwupsxx] error in %s: %s\n", __PRETTY_FUNCTION__, e.what())


namespace wups::config {

    // SimplePadData

    SimplePadData::SimplePadData(const WUPSConfigSimplePadData& base)
        noexcept :
        WUPSConfigSimplePadData{base},
        buttons_repeat{}
    {}


    bool
    SimplePadData::pressed_or_repeated(unsigned mask)
        const noexcept
    {
        return (buttons_d | buttons_repeat) & mask;
    }


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

                // Here we implement a "repeat" function.
                using clock = std::chrono::steady_clock;
                using time_point = clock::time_point;

                constexpr auto repeat_delay = 500ms;
                static std::array<time_point, 16> pressed_time{}; // zero-initialized
                auto now = clock::now();

                unsigned repeat = 0;
                for (unsigned b = 0; b < 16; ++b) {
                    unsigned mask = 1u << b;
                    if (input.buttons_d & mask)
                        pressed_time[b] = now;

                    if (input.buttons_h & mask)
                        // if button was held long enough, flag it as being on a repeat state
                        if (now - pressed_time[b] >= repeat_delay)
                            repeat |= mask;

                    if (input.buttons_r & mask)
                        pressed_time[b] = {};
                }

                SimplePadData sinput = input;
                sinput.buttons_repeat = static_cast<WUPS_CONFIG_SIMPLE_INPUT>(repeat);

                if (it->on_input(sinput) == FocusChange::Lose)
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
                // TODO: implement "repeat" functionality for extended input too
                if (!it->has_focus())
                    return;

                if (it->on_input(input) == FocusChange::Lose)
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


    FocusChange
    item::on_input(const SimplePadData& input)
    {
        if (input.buttons_d & WUPS_CONFIG_BUTTON_X)
            restore();

        if (input.buttons_d & WUPS_CONFIG_BUTTON_B)
            return FocusChange::Lose;

        return FocusChange::Keep;
    }


    FocusChange
    item::on_input(const WUPSConfigComplexPadData& /*input*/)
    {
        return FocusChange::Keep;
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
