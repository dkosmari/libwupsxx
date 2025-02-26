/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
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

        std::uint32_t long_hold;
        std::chrono::steady_clock::time_point now;

        explicit
        simple_pad_data(const WUPSConfigSimplePadData& base) noexcept;


        bool pressed_or_long_held(std::uint32_t mask) const noexcept;

    private:

        void update_long_hold() noexcept;

    };


    struct complex_pad_data : WUPSConfigComplexPadData {

        static constexpr
        unsigned max_wiimotes = 7;


        std::uint32_t vpad_long_hold;
        std::array<std::uint32_t, max_wiimotes> kpad_core_long_hold;
        std::array<std::uint32_t, max_wiimotes> kpad_ext_long_hold;
        std::chrono::steady_clock::time_point now;

        explicit
        complex_pad_data(const WUPSConfigComplexPadData& base) noexcept;

    private:

        void update_long_hold() noexcept;

        void update_long_hold_kpad(unsigned w) noexcept;
        void update_long_hold_nunchuk(unsigned w) noexcept;
        void update_long_hold_classic(unsigned w) noexcept;
        void update_long_hold_pro(unsigned w) noexcept;

    };


} // namespace wups::config

#endif
