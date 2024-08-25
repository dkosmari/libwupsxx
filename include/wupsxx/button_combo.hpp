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


namespace wups::config {

    struct vpad_combo {
        // combination of VPAD_BUTTON_* values.
        std::uint32_t buttons = 0;
    };


    struct wpad_combo {
        // combination of WPAD_BUTTON_* values.
        std::uint32_t core_buttons = 0;

        WPADExtensionType ext = WPAD_EXT_CORE;

        // combination of WPAD_*_BUTTON, depends on ext
        std::uint32_t ext_buttons = 0;
    };


    using button_combo = std::variant<std::monostate,
                                      vpad_combo,
                                      wpad_combo>;


    std::string to_string(const button_combo& bc);

    std::string to_glyph(const button_combo& bc);

    button_combo button_combo_from_string(const std::string& str);


} // namespace wups::config

#endif
