/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_INPUT_HPP
#define WUPSXX_INPUT_HPP

#include <array>
#include <chrono>
#include <cstdint>

#include <padscore/wpad.h>

#include <wups/config.h>


namespace wups::config {

    struct simple_pad_data : WUPSConfigSimplePadData {

        std::uint32_t buttons_repeat;
        std::chrono::steady_clock::time_point now;

        explicit
        simple_pad_data(const WUPSConfigSimplePadData& base) noexcept;


        bool pressed_or_repeated(std::uint32_t mask) const noexcept;

    private:

        void update_repeat() noexcept;

    };


    struct complex_pad_data : WUPSConfigComplexPadData {

        static constexpr
        unsigned max_wiimotes = 7;


        std::uint32_t vpad_repeat;
        std::array<std::uint32_t, max_wiimotes> kpad_core_repeat;
        std::array<std::uint32_t, max_wiimotes> kpad_ext_repeat;
        std::chrono::steady_clock::time_point now;

        explicit
        complex_pad_data(const WUPSConfigComplexPadData& base) noexcept;


    private:

        void update_repeat() noexcept;

        void update_repeat_wpad(unsigned w) noexcept;
        void update_repeat_nunchuk(unsigned w) noexcept;
        void update_repeat_classic(unsigned w) noexcept;
        void update_repeat_pro(unsigned w) noexcept;

    };


} // namespace wups::config

#endif
