/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>

#include <whb/log.h>

#include "wupsxx/button_combo_item.hpp"

#include "cafe_glyphs.h"


namespace wups::config {


    button_combo_item::button_combo_item(const std::string& label,
                                         button_combo& variable,
                                         const button_combo& default_value) :
        var_item{label, variable, default_value},
        pressed_anything{false},
        reading_combo{false}
    {}


    std::unique_ptr<button_combo_item>
    button_combo_item::create(const std::string& label,
                              button_combo& variable,
                              const button_combo& default_value)
    {
        return make_unique<button_combo_item>(label, variable, default_value);
    }


    int
    button_combo_item::get_display(char* buf, std::size_t size)
        const
    {
        std::string str = to_glyph(variable);
        std::snprintf(buf, size, "%s", str.c_str());
        return 0;
    }


    int
    button_combo_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        if (reading_combo) {
            std::string str = to_glyph(variable);

            if (str.empty())
                str = "(waiting for buttons...)";

            std::snprintf(buf, size, "%s", str.c_str());
        } else {
            std::snprintf(buf, size,
                          "%s=confirm, %s=cancel, %s=default",
                          CAFE_GLYPH_BTN_A,
                          CAFE_GLYPH_BTN_B,
                          CAFE_GLYPH_BTN_X);
        }

        return 0;
    }


    void
    button_combo_item::on_focus_changed()
    {
        var_item::on_focus_changed();

        if (has_focus()) {
            // start reading combo when focused
            pressed_anything = false;
            reading_combo = true;
            variable = {};
        }
    }


    focus_status
    button_combo_item::on_input(const simple_pad_data& input)
    {
        if (input.buttons_d)
            pressed_anything = true;

        // ignore simple inputs when reading the combo
        if (reading_combo)
            return focus_status::keep;

        // We handle reset-to-default here: always confirm and lose focus.
        if (input.buttons_d & WUPS_CONFIG_BUTTON_X) {
            confirm_change();
            restore();
            return focus_status::lose;
        }

        // let var_item/item classes handle confirm/cancel with A/B
        return var_item::on_input(input);
    }


    focus_status
    button_combo_item::on_input(const complex_pad_data& input)
    {
        // ignore complex input when not reading combo
        if (!reading_combo)
            return focus_status::keep;

        /*
         * Track if we are still reading the combo input:
         *
         * - If user never pressed anything since focusing the item,
         *   keep reading the combo.
         *
         * - If user already pressed something, stop reading as soon as all buttons are
         *   released.
         */
        reading_combo = !pressed_anything;

        using std::get;

        if (input.vpad.vpadError == VPAD_READ_SUCCESS) {
            auto& status = input.vpad.data;
            if (status.hold) {
                reading_combo = true;
                if (input.vpad_repeat) {
                    if (!holds_alternative<vpad_combo>(variable))
                        variable = vpad_combo{};
                    get<vpad_combo>(variable).buttons |= input.vpad_repeat;
                }
            }
        }


        auto ensure_var_is_wpad = [this]() -> wpad_combo&
        {
            if (!holds_alternative<wpad_combo>(variable))
                variable = wpad_combo{};
            return get<wpad_combo>(variable);
        };


        for (unsigned w = 0; w < 7; ++w) {
            // if kpad[w] is valid
            if (input.kpad.kpadError[w] == KPAD_ERROR_OK) {

                auto& status = input.kpad.data[w];
                auto& core_repeat = input.kpad_core_repeat[w];
                auto& ext_repeat = input.kpad_ext_repeat[w];

                if (status.hold) {
                    reading_combo = true;
                    if (core_repeat) {
                        auto & var = ensure_var_is_wpad();
                        var.core_buttons |= core_repeat;
                    }
                }

                switch (status.extensionType) {
                case WPAD_EXT_NUNCHUK:
                case WPAD_EXT_MPLUS_NUNCHUK:
                    if (status.nunchuk.hold) {
                        reading_combo = true;
                        if (ext_repeat) {
                            auto& var = ensure_var_is_wpad();
                            var.ext = static_cast<WPADExtensionType>(status.extensionType);
                            var.ext_buttons |= ext_repeat;
                        }
                    }
                    break;
                case WPAD_EXT_CLASSIC:
                case WPAD_EXT_MPLUS_CLASSIC:
                    if (status.classic.hold) {
                        reading_combo = true;
                        if (ext_repeat) {
                            auto& var = ensure_var_is_wpad();
                            var.ext = static_cast<WPADExtensionType>(status.extensionType);
                            var.ext_buttons |= ext_repeat;
                        }
                    }
                    break;
                case WPAD_EXT_PRO_CONTROLLER:
                    if (status.pro.hold) {
                        reading_combo = true;
                        if (ext_repeat) {
                            auto& var = ensure_var_is_wpad();
                            var.ext = static_cast<WPADExtensionType>(status.extensionType);
                            var.ext_buttons |= ext_repeat;
                        }
                    }
                    break;
                } // switch (status.extensionType)

            } // if kpad[w] is valid

        }

        /*
         * If no button is being held anywhere, reading_combo remains false,
         * so we switch to the simple input handler.
         */

        return focus_status::keep;
    }



} // wups::config
