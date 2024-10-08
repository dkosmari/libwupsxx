/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>

// #include <whb/log.h> // DEBUG

#include "wupsxx/button_combo_item.hpp"

#include "wupsxx/cafe_glyphs.h"

#include "utils.hpp"


using wups::utils::button_combo;
namespace vpad = wups::utils::vpad;
namespace wpad = wups::utils::wpad;


namespace wups::config {


    button_combo_item::button_combo_item(const std::string& label,
                                         button_combo& variable,
                                         const button_combo& default_value) :
        var_item{label, variable, default_value},
        state{state_t::waiting}
    {}


    std::unique_ptr<button_combo_item>
    button_combo_item::create(const std::string& label,
                              button_combo& variable,
                              const button_combo& default_value)
    {
        return make_unique<button_combo_item>(label, variable, default_value);
    }


    void
    button_combo_item::get_display(char* buf, std::size_t size)
        const
    {
        std::string str = to_glyph(variable);
        std::snprintf(buf, size, "%s", str.c_str());
    }


    void
    button_combo_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        switch (state) {
        case state_t::waiting:
            std::snprintf(buf, size, "(waiting for buttons...)");
            break;
        case state_t::reading:
            std::snprintf(buf, size, "%s (reading...)",
                          to_glyph(variable).c_str());
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


    namespace {

        // Returns true if an extension has no button set
        struct wpad_ext_buttons_is_clear_visitor {
            bool operator ()(std::monostate) const
            { return true; }

            bool operator ()(const wpad::nunchuk::button_set& bs) const
            { return bs.buttons == 0; }

            bool operator ()(const wpad::classic::button_set& bs) const
            { return bs.buttons == 0; }

            bool operator ()(const wpad::pro::button_set& bs) const
            { return bs.buttons == 0; }
        };

    } // namespace


    void
    button_combo_item::on_focus_changed()
    {
        var_item::on_focus_changed();

        if (has_focus()) {
            // disable TV Remote while we read button combos
            VPADSetTVMenuInvalid(VPAD_CHAN_0, true);
            variable = {};
            state = state_t::waiting;
        } else {
            // enable TV Remote when we lose focus
            VPADSetTVMenuInvalid(VPAD_CHAN_0, false);

            if (auto* combo = get_if<vpad::button_set>(&variable)) {
                // if the combo is empty, set variable back to monostate
                if (combo->buttons == 0)
                    variable = {};
            }

            if (auto* combo = get_if<wpad::button_set>(&variable)) {
                // if no ext button was used, reset ext back to monostate
                if (visit(wpad_ext_buttons_is_clear_visitor{},
                          combo->ext))
                    combo->ext = {};
                // if core combo is empty and no extension, set variable back to monostate
                if (combo->core.buttons == 0 && holds_alternative<std::monostate>(combo->ext))
                    variable = {};
            }
        }
    }


    focus_status
    button_combo_item::on_input(const simple_pad_data& input)
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

        // let var_item class handle confirm/cancel with A/B
        return var_item::on_input(input);
    }


    focus_status
    button_combo_item::on_input(const complex_pad_data& input)
    {
        if (state != state_t::waiting && state != state_t::reading)
            return focus_status::keep;

        unsigned total_held = 0;

        if (input.vpad.vpadError == VPAD_READ_SUCCESS) {
            auto& status = input.vpad.data;
            if (status.trigger && state == state_t::waiting)
                state = state_t::reading;
            if (status.hold) {
                ++total_held;
                if (input.vpad_repeat) {
                    auto& combo = utils::ensure<vpad::button_set>(variable);
                    combo.buttons |= input.vpad_repeat;
                }
            }
        }


        for (unsigned w = 0; w < 7; ++w) {
            // if kpad[w] is valid
            if (input.kpad.kpadError[w] == KPAD_ERROR_OK) {

                auto& status = input.kpad.data[w];
                auto& core_repeat = input.kpad_core_repeat[w];
                auto& ext_repeat = input.kpad_ext_repeat[w];

                // Handle core buttons here, it's common to all extensions
                if (status.trigger && state == state_t::waiting)
                    state = state_t::reading;
                if (status.hold) {
                    ++total_held;
                    if (core_repeat) {
                        auto& combo = utils::ensure<wpad::button_set>(variable);
                        combo.core.buttons |= core_repeat;
                    }
                }

                switch (status.extensionType) {

                case WPAD_EXT_CORE:
                case WPAD_EXT_MPLUS:
                    if (core_repeat) {
                        // user is trying to set combo from wiimote with no ext
                        auto& combo = utils::ensure<wpad::button_set>(variable);
                        combo.ext = {}; // clear the extension in the combo
                    }
                    break;

                case WPAD_EXT_NUNCHUK:
                case WPAD_EXT_MPLUS_NUNCHUK:
                    if (status.nunchuk.trigger && state == state_t::waiting)
                        state = state_t::reading;
                    if (status.nunchuk.hold) {
                        ++total_held;
                        if (ext_repeat) {
                            auto& combo = utils::ensure<wpad::button_set>(variable);
                            auto& xcombo = utils::ensure<wpad::nunchuk::button_set>(combo.ext);
                            xcombo.buttons |= ext_repeat;
                        }
                    }
                    break;

                case WPAD_EXT_CLASSIC:
                case WPAD_EXT_MPLUS_CLASSIC:
                    if (status.classic.trigger && state == state_t::waiting)
                        state = state_t::reading;
                    if (status.classic.hold) {
                        ++total_held;
                        if (ext_repeat) {
                            auto& combo = utils::ensure<wpad::button_set>(variable);
                            auto& xcombo = utils::ensure<wpad::classic::button_set>(combo.ext);
                            xcombo.buttons |= ext_repeat;
                        }
                    }
                    break;

                case WPAD_EXT_PRO_CONTROLLER:
                    if (status.pro.trigger && state == state_t::waiting)
                        state = state_t::reading;
                    if (status.pro.hold) {
                        ++total_held;
                        if (ext_repeat) {
                            auto& combo = utils::ensure<wpad::button_set>(variable);
                            auto& xcombo = utils::ensure<wpad::pro::button_set>(combo.ext);
                            xcombo.buttons |= ext_repeat;
                            // Note: core buttons are not reported, so clear them out
                            combo.core.buttons = 0;
                        }
                    }
                    break;

                } // switch (status.extensionType)

            } // if kpad[w] is valid

        } // for each wiimote

        if (total_held == 0 && state == state_t::reading) {
            // user released all buttons after entering reading mode
            state = state_t::confirming;
            return focus_status::change_input; // use simple input now
        }

        return focus_status::keep;
    }



} // wups::config
