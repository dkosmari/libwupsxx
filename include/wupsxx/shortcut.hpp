/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_SHORTCUT_HPP
#define WUPSXX_SHORTCUT_HPP

/*
 * This is a wrapper for libbuttoncombo.
 */

#include <concepts>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <padscore/wpad.h>
#include <vpad/input.h>

#include <buttoncombo/defines.h>


namespace wups::shortcut {


    using error_code = ButtonComboModule_Error;
    using ctr_set = ButtonComboModule_ControllerTypes;
    using btn_set = ButtonComboModule_Buttons;
    using handle = ButtonComboModule_ComboHandle;


    struct error : std::runtime_error {

        error_code code;

        error(error_code status);
        error(const std::string& msg, error_code status);

    };


    void
    initialize();


    void
    finalize();


    struct combo {

        ctr_set controllers = {};
        btn_set buttons = {};

        constexpr combo() noexcept = default;

        combo(const std::string& arg);

        static
        combo
        from_vpad(std::uint32_t buttons) noexcept;

        static
        combo
        from_wpad_core(std::uint32_t buttons) noexcept;

        static
        combo
        from_wpad_nunchuk(std::uint32_t core, std::uint32_t ext) noexcept;

        static
        combo
        from_wpad_classic(std::uint32_t core, std::uint32_t ext) noexcept;

        static
        combo
        from_wpad_pro(std::uint32_t ext) noexcept;


        bool
        is_empty() const noexcept;

    };


    bool
    is_available(const combo& c);


    bool
    is_conflicted(handle h);


    std::pair<handle, bool>
    create(const std::string& label,
           const combo& c,
           std::function<void(ctr_set, handle)>);


    // Note: will set the argument to null.
    void
    destroy(handle& handle);


    // Return true is updated combo has conflict.
    bool
    update(handle& handle, const combo& c);


    std::string
    to_string(const combo& c);


    std::string
    to_glyph(const combo& c);


} // namespace wups::shortcut

#endif
