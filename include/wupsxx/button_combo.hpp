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


namespace wups::utils {

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



    // Utility functions for detecting vpad/wpad combos

    // Call this from your VPADRead() hook.
    // Return true if no error.
    bool vpad_update(VPADChan channel, const VPADStatus& status) noexcept;

    // Call this from your WPADRead() hook.
    // Return true if not error.
    bool wpad_update(WPADChan channel, const WPADStatus* status) noexcept;


    [[nodiscard]]
    bool vpad_triggered(VPADChan channel, const button_combo& combo) noexcept;

    [[nodiscard]]
    bool wpad_triggered(WPADChan channel, const button_combo& combo) noexcept;



    // Utility functions to track wpad trigger/release status, since the WPAD* API doesn't
    // do it.

    namespace detail {

        struct button_state_16 {
            std::uint16_t hold    = 0;
            std::uint16_t trigger = 0;
            std::uint16_t release = 0;
        };

        struct button_state_32 {
            std::uint32_t hold    = 0;
            std::uint32_t trigger = 0;
            std::uint32_t release = 0;
        };

    } // namespace detail


    struct wpad_core_button_state    : detail::button_state_16 {};
    struct wpad_nunchuk_button_state : detail::button_state_16 {};
    struct wpad_classic_button_state : detail::button_state_16 {};
    struct wpad_pro_button_state     : detail::button_state_32 {};

    using wpad_ext_button_state = std::variant<std::monostate,
                                               wpad_nunchuk_button_state,
                                               wpad_classic_button_state,
                                               wpad_pro_button_state>;

    struct wpad_button_state {
        wpad_core_button_state core;
        wpad_ext_button_state  ext;
    };


    // Retrieve the button state as it's tracked internally to detect combos.
    [[nodiscard]]
    const wpad_button_state& wpad_get_button_state(WPADChan channel);


} // namespace wups::utils

#endif
