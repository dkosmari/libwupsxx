/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>

// #include <whb/log.h> // DEBUG

#include <buttoncombo/api.h>

#include "wupsxx/shortcut_item.hpp"

#include "wupsxx/cafe_glyphs.h"

#include "wupsxx/logger.hpp"
#include "utils.hpp"


namespace wups {

    namespace {

        void
        dummy_callback(shortcut::ctr_set, shortcut::handle, void*)
        {}

    }


    shortcut_item::shortcut_item(option<shortcut::combo>& opt,
                                 shortcut::handle combo_handle_) :
        var_item{opt},
        state{state_t::waiting},
        combo_handle{combo_handle_}
    {
        ButtonComboModule_GetButtonComboCallback(combo_handle, &old_callback);
        ButtonComboModule_CallbackOptions new_callback{
            .callback = dummy_callback,
            .context = nullptr
        };
        ButtonComboModule_UpdateButtonComboCallback(combo_handle, &new_callback);

        if (shortcut::is_conflicted(combo_handle))
            message = "Conflict!";
    }


    shortcut_item::~shortcut_item()
        noexcept
    {
        ButtonComboModule_UpdateButtonComboCallback(combo_handle, &old_callback);
    }


    std::unique_ptr<shortcut_item>
    shortcut_item::create(option<shortcut::combo>& opt,
                              shortcut::handle combo_handle)
    {
        return std::make_unique<shortcut_item>(opt, combo_handle);
    }


    void
    shortcut_item::get_display(char* buf, std::size_t size)
        const
    {
        std::string str;
        if (!variable.buttons)
            str = "(Disabled: no combo set)";
        else
            str = to_glyph(variable);

        if (!message.empty())
            str += " (" + message + ")";
        std::snprintf(buf, size, "%s", str.data());
    }


    void
    shortcut_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        switch (state) {
        case state_t::waiting:
            std::snprintf(buf, size, "(waiting for buttons...)");
            break;
        case state_t::reading:
            std::snprintf(buf, size, "%s (reading...)",
                          to_glyph(variable).data());
            break;
        case state_t::confirming:
            std::snprintf(buf, size,
                          "(%s=confirm   %s=cancel   %s=default)",
                          CAFE_GLYPH_BTN_A,
                          CAFE_GLYPH_BTN_B,
                          CAFE_GLYPH_BTN_X "/" CAFE_GLYPH_WIIMOTE_BTN_2);
            break;
        }
    }


    void
    shortcut_item::on_focus_changed()
    {
        var_item::on_focus_changed();

        // Just received focus, enter "waiting" state.
        if (has_focus()) {
            // clear combo
            variable = {};
            shortcut::update(combo_handle, variable);
            state = state_t::waiting;
        } else {
            // Just lost focus, now we check for conflicts
            if (shortcut::update(combo_handle, variable))
                message = "Conflict!";
            else
                message = "";
        }
    }


    focus_status
    shortcut_item::on_input(const simple_pad_data& input)
    {
        if (state == state_t::waiting)
            return focus_status::change_input; // let complex input handle waiting

        // sanity check, should not be here until we're in the confirming state
        if (state != state_t::confirming)
            return focus_status::keep;

        // We handle restore-default here: always confirm and lose focus.
        if (input.buttons_d & WUPS_CONFIG_BUTTON_X) {
            restore_default();
            confirm_change();
            return focus_status::lose;
        }

        // Let the `var_item` class handle confirm/cancel with A/B
        return var_item::on_input(input);
    }


    focus_status
    shortcut_item::on_input(const complex_pad_data& input)
    {
        if (state != state_t::waiting && state != state_t::reading)
            return focus_status::keep;

        shortcut::combo new_value;
        bool any_button_down = false;

        if (input.vpad.vpadError == VPAD_READ_SUCCESS) {
            auto& status = input.vpad.data;
            if (status.trigger && state == state_t::waiting)
                state = state_t::reading;
            if (status.hold) {
                any_button_down = true;
                new_value = shortcut::combo::from_vpad(input.vpad_long_hold);
            }
        }

        for (unsigned w = 0; w < 7; ++w) {
            if (input.kpad.kpadError[w] == KPAD_ERROR_OK) {

                auto& status = input.kpad.data[w];
                auto& core_long_hold = input.kpad_core_long_hold[w];
                auto& ext_long_hold = input.kpad_ext_long_hold[w];

                if (status.trigger && state == state_t::waiting)
                    state = state_t::reading;

                switch (status.extensionType) {

                    case WPAD_EXT_CORE:
                    case WPAD_EXT_MPLUS:
                        if (status.hold) {
                            any_button_down = true;
                            new_value = shortcut::combo::from_wpad_core(core_long_hold);
                        }
                        break;

                    case WPAD_EXT_NUNCHUK:
                    case WPAD_EXT_MPLUS_NUNCHUK:
                        if (status.nunchuk.trigger && state == state_t::waiting)
                            state = state_t::reading;
                        if (status.hold || status.nunchuk.hold) {
                            any_button_down = true;
                            new_value = shortcut::combo::from_wpad_nunchuk(core_long_hold,
                                                                               ext_long_hold);
                        }
                        break;

                    case WPAD_EXT_CLASSIC:
                    case WPAD_EXT_MPLUS_CLASSIC:
                        if (status.classic.trigger && state == state_t::waiting)
                            state = state_t::reading;
                        if (status.hold || status.classic.hold) {
                            any_button_down = true;
                            new_value = shortcut::combo::from_wpad_classic(core_long_hold,
                                                                               ext_long_hold);
                        }
                        break;

                    case WPAD_EXT_PRO_CONTROLLER:
                        if (status.pro.trigger && state == state_t::waiting)
                            state = state_t::reading;
                        if (status.pro.hold) {
                            any_button_down = true;
                            new_value = shortcut::combo::from_wpad_pro(ext_long_hold);
                        }
                        break;

                } // switch (status.extensionType)

            } // if kpad[w] is valid

        } // for each wiimote


        if (new_value.buttons) {
            // If input is still coming from the same controller, just merge the buttons...
            if (variable.controllers == new_value.controllers)
                variable.buttons |= new_value.buttons;
            else // ... otherwise replace it.
                variable = new_value;
        }

        if (!any_button_down && state == state_t::reading) {
            // After all buttons were released while in `reading` state, enter
            // `confirming` state and switch back to simple input.
            state = state_t::confirming;
            return focus_status::change_input;
        }

        return focus_status::keep;
    }


    std::unique_ptr<shortcut_item>
    make_item(option<shortcut::combo>& opt,
              shortcut::handle combo_handle)
    {
        return shortcut_item::create(opt, combo_handle);
    }

} // wups
