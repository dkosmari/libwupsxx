/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <chrono>
#include <cstdint>
#include <ranges>

#include <padscore/wpad.h>
#include <vpad/input.h>

#include "wupsxx/input.hpp"


using std::array;
using std::uint32_t;
using std::chrono::steady_clock;
using time_point = steady_clock::time_point;
using std::views::enumerate;

using namespace std::literals;


namespace wups {


    static_assert(complex_pad_data::max_wiimotes
                  == std::size(WUPSConfigComplexPadData{}.kpad.data),
                  "Max number of wiimotes changed in WUPS!");


    namespace {

        constexpr auto long_enough = 500ms;


        constexpr array simple_button_list{
            WUPS_CONFIG_BUTTON_UP, WUPS_CONFIG_BUTTON_DOWN,
            WUPS_CONFIG_BUTTON_LEFT, WUPS_CONFIG_BUTTON_RIGHT,
            WUPS_CONFIG_BUTTON_L, WUPS_CONFIG_BUTTON_R,
            WUPS_CONFIG_BUTTON_ZL, WUPS_CONFIG_BUTTON_ZR,
            WUPS_CONFIG_BUTTON_A, WUPS_CONFIG_BUTTON_B,
            WUPS_CONFIG_BUTTON_X, WUPS_CONFIG_BUTTON_Y,
            WUPS_CONFIG_BUTTON_PLUS, WUPS_CONFIG_BUTTON_MINUS,
            WUPS_CONFIG_BUTTON_STICK_L, WUPS_CONFIG_BUTTON_STICK_R,
        };


        constexpr array vpad_button_list{
            VPAD_BUTTON_UP, VPAD_BUTTON_DOWN,
            VPAD_BUTTON_LEFT, VPAD_BUTTON_RIGHT,
            VPAD_BUTTON_L, VPAD_BUTTON_R,
            VPAD_BUTTON_ZL, VPAD_BUTTON_ZR,
            VPAD_BUTTON_A, VPAD_BUTTON_B,
            VPAD_BUTTON_X, VPAD_BUTTON_Y,
            VPAD_BUTTON_PLUS, VPAD_BUTTON_MINUS,
            VPAD_BUTTON_STICK_L, VPAD_BUTTON_STICK_R,
            VPAD_BUTTON_HOME, VPAD_BUTTON_TV,
            VPAD_BUTTON_SYNC,
            VPAD_STICK_L_EMULATION_UP, VPAD_STICK_L_EMULATION_DOWN,
            VPAD_STICK_L_EMULATION_LEFT, VPAD_STICK_L_EMULATION_RIGHT,
            VPAD_STICK_R_EMULATION_UP, VPAD_STICK_R_EMULATION_DOWN,
            VPAD_STICK_R_EMULATION_LEFT, VPAD_STICK_R_EMULATION_RIGHT,
        };


        constexpr array core_button_list{
            WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT,
            WPAD_BUTTON_UP, WPAD_BUTTON_DOWN,
            WPAD_BUTTON_PLUS, WPAD_BUTTON_HOME, WPAD_BUTTON_MINUS,
            WPAD_BUTTON_A, WPAD_BUTTON_B,
            WPAD_BUTTON_1, WPAD_BUTTON_2,
        };


        constexpr array nunchuk_button_list{
            WPAD_NUNCHUK_BUTTON_Z, WPAD_NUNCHUK_BUTTON_C,
        };


        constexpr array classic_button_list{
            WPAD_CLASSIC_BUTTON_UP, WPAD_CLASSIC_BUTTON_DOWN,
            WPAD_CLASSIC_BUTTON_LEFT, WPAD_CLASSIC_BUTTON_RIGHT,
            WPAD_CLASSIC_BUTTON_L, WPAD_CLASSIC_BUTTON_R,
            WPAD_CLASSIC_BUTTON_ZL, WPAD_CLASSIC_BUTTON_ZR,
            WPAD_CLASSIC_BUTTON_A, WPAD_CLASSIC_BUTTON_B,
            WPAD_CLASSIC_BUTTON_X, WPAD_CLASSIC_BUTTON_Y,
            WPAD_CLASSIC_BUTTON_PLUS, WPAD_CLASSIC_BUTTON_HOME, WPAD_CLASSIC_BUTTON_MINUS,
        };


        constexpr array pro_button_list{
            WPAD_PRO_BUTTON_UP, WPAD_PRO_BUTTON_DOWN,
            WPAD_PRO_BUTTON_LEFT, WPAD_PRO_BUTTON_RIGHT,
            WPAD_PRO_TRIGGER_L, WPAD_PRO_TRIGGER_R,
            WPAD_PRO_TRIGGER_ZL, WPAD_PRO_TRIGGER_ZR,
            WPAD_PRO_BUTTON_A, WPAD_PRO_BUTTON_B,
            WPAD_PRO_BUTTON_X, WPAD_PRO_BUTTON_Y,
            WPAD_PRO_BUTTON_PLUS, WPAD_PRO_BUTTON_HOME, WPAD_PRO_BUTTON_MINUS,
            WPAD_PRO_BUTTON_STICK_L, WPAD_PRO_BUTTON_STICK_R,
        };

    } // namespace


    // simple_pad_data

    simple_pad_data::simple_pad_data(const WUPSConfigSimplePadData& base)
        noexcept :
        WUPSConfigSimplePadData{base},
        long_hold{0},
        now{steady_clock::now()}
    {
        update_long_hold();
    }


    void
    simple_pad_data::update_long_hold()
        noexcept
    {
        static array<time_point, simple_button_list.size()> pressed_moment;

        for (auto [idx, button] : enumerate(simple_button_list)) {
            if (buttons_d & button)
                pressed_moment[idx] = now;

            if (buttons_h & button)
                if (now - pressed_moment[idx] >= long_enough)
                    long_hold |= button;

            if (buttons_r & button)
                pressed_moment[idx] = {};
        }
    }


    bool
    simple_pad_data::pressed_or_long_held(std::uint32_t mask)
        const noexcept
    {
        return (buttons_d | long_hold) & mask;
    }



    // complex_pad_data

    complex_pad_data::complex_pad_data(const WUPSConfigComplexPadData& base)
        noexcept :
        WUPSConfigComplexPadData{base},
        vpad_long_hold{0},
        kpad_core_long_hold{},
        kpad_ext_long_hold{},
        now{steady_clock::now()}
    {
        update_long_hold();
    }


    void
    complex_pad_data::update_long_hold()
        noexcept
    {
        // First, handle VPAD
        static array<time_point, vpad_button_list.size()> pressed_time;
        if (vpad.vpadError == VPAD_READ_SUCCESS) {

            for (auto [idx, button] : enumerate(vpad_button_list)) {
                const VPADStatus& status = vpad.data;

                if (status.trigger & button)
                    pressed_time[idx] = now;

                if (status.hold & button)
                    if (now - pressed_time[idx] >= long_enough)
                        vpad_long_hold |= button;

                if (status.release & button)
                    pressed_time[idx] = {};
            }

        }

        // Now handle each wiimote
        for (unsigned w = 0; w < max_wiimotes; ++w)
            if (kpad.kpadError[w] == KPAD_ERROR_OK)
                update_long_hold_kpad(w);

    }


    void
    complex_pad_data::update_long_hold_kpad(unsigned w)
        noexcept
    {
        using core_times = array<time_point, core_button_list.size()>;
        static array<core_times, max_wiimotes> pressed_time;

        const KPADStatus& status = kpad.data[w];

        for (auto [idx, button] : enumerate(core_button_list)) {
            auto& pressed = pressed_time[w][idx];

            if (status.trigger & button)
                pressed = now;

            if (status.hold & button)
                if (now - pressed >= long_enough)
                    kpad_core_long_hold[w] |= button;

            if ((status.release & button) || !(status.hold & button))
                pressed = {};
        }


        switch (status.extensionType) {

        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
            update_long_hold_nunchuk(w);
            break;

        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            update_long_hold_classic(w);
            break;

        case WPAD_EXT_PRO_CONTROLLER:
            update_long_hold_pro(w);
            break;

        } // switch (status.extensionType)
    }


    void
    complex_pad_data::update_long_hold_nunchuk(unsigned w)
        noexcept
    {
        using nunchuk_times = array<time_point, nunchuk_button_list.size()>;
        static array<nunchuk_times, max_wiimotes> pressed_time;

        auto& status = kpad.data[w].nunchuk;

        for (auto [idx, button] : enumerate(nunchuk_button_list)) {
            auto& pressed = pressed_time[w][idx];

            if (status.trigger & button)
                pressed = now;

            if (status.hold & button)
                if (now - pressed >= long_enough)
                    kpad_ext_long_hold[w] |= button;

            if (status.release & button || !(status.hold & button))
                pressed = {};
        }
    }


    void
    complex_pad_data::update_long_hold_classic(unsigned w)
        noexcept
    {
        using classic_times = array<time_point, classic_button_list.size()>;
        static array<classic_times, max_wiimotes> pressed_time;

        auto& status = kpad.data[w].classic;

        for (auto [idx, button] : enumerate(classic_button_list)) {
            auto& pressed = pressed_time[w][idx];

            if (status.trigger & button)
                pressed = now;

            if (status.hold & button)
                if (now - pressed >= long_enough)
                    kpad_ext_long_hold[w] |= button;

            if (status.release & button || !(status.hold & button))
                pressed = {};
        }
    }


    void
    complex_pad_data::update_long_hold_pro(unsigned w)
        noexcept
    {
        using pro_times = array<time_point, pro_button_list.size()>;
        static array<pro_times, max_wiimotes> pressed_time;

        auto& status = kpad.data[w].pro;

        for (auto [idx, button] : enumerate(pro_button_list)) {
            auto& pressed = pressed_time[w][idx];

            if (status.trigger & button)
                pressed = now;

            if (status.hold & button)
                if (now - pressed >= long_enough)
                    kpad_ext_long_hold[w] |= button;

            if (status.release & button || !(status.hold & button))
                pressed = {};
        }
    }


} // namespace wups
