/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_BUTTON_COMBO_HPP
#define WUPSXX_BUTTON_COMBO_HPP

#include <cstdint>
#include <string>
#include <variant>

#include <padscore/wpad.h>
#include <vpad/input.h>


struct WPADStatus;


namespace wups::config {

    struct vpad_buttons {
        // combination of VPAD_BUTTON_* values.
        std::uint32_t buttons = 0;
    };



    struct wpad_core_buttons {
        // combination of WPAD_BUTTON_*
        std::uint16_t buttons = 0;
    };


    struct wpad_nunchuk_buttons {
        // combination of WPAD_NUNCHUK_BUTTON_Z and WPAD_NUNCHUK_BUTTON_C
        std::uint16_t buttons = 0;
    };


    struct wpad_classic_buttons {
        // combination of WPAD_CLASSIC_BUTTON_*
        std::uint16_t buttons = 0;
    };


    struct wpad_pro_buttons {
        std::uint32_t buttons = 0;
    };


    struct wpad_buttons {
        wpad_core_buttons core;
        std::variant<std::monostate,
                     wpad_nunchuk_buttons,
                     wpad_classic_buttons,
                     wpad_pro_buttons> ext;
    };


    using button_combo = std::variant<std::monostate,
                                      vpad_buttons,
                                      wpad_buttons>;


    std::string to_string(const button_combo& bc);

    std::string to_glyph(const button_combo& bc);

    button_combo button_combo_from_string(const std::string& str);


    // helper functions for detecting vpad/wpad combos

    // Call this from your VPADRead() hook.
    void update_vpad(VPADChan channel, const VPADStatus* status) noexcept;

    // Call this from your WPADRead() hook.
    void update_wpad(WPADChan channel, const WPADStatus* status) noexcept;


    bool was_triggered(VPADChan channel, const button_combo& combo) noexcept;
    bool was_triggered(WPADChan channel, const button_combo& combo) noexcept;


} // namespace wups::config

#endif
