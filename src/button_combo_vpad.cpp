/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>

#include "wupsxx/button_combo.hpp"

#include "wupsxx/cafe_glyphs.h"

#include "utils.hpp"


using std::array;
using std::string;


namespace wups::utils::vpad {

    struct button_state : detail::button_state_32 {};

    array<button_state, 2> states;


    struct entry {
        VPADButtons button;
        const char* name;
        const char* glyph;
    };


    const auto button_entries = {
        entry{VPAD_BUTTON_L,       "VPAD_BUTTON_L",       CAFE_GLYPH_GAMEPAD_BTN_L},
        entry{VPAD_BUTTON_R,       "VPAD_BUTTON_R",       CAFE_GLYPH_GAMEPAD_BTN_R},
        entry{VPAD_BUTTON_ZL,      "VPAD_BUTTON_ZL",      CAFE_GLYPH_GAMEPAD_BTN_ZL},
        entry{VPAD_BUTTON_ZR,      "VPAD_BUTTON_ZR",      CAFE_GLYPH_GAMEPAD_BTN_ZR},
        entry{VPAD_BUTTON_UP,      "VPAD_BUTTON_UP",      CAFE_GLYPH_GAMEPAD_BTN_UP},
        entry{VPAD_BUTTON_DOWN,    "VPAD_BUTTON_DOWN",    CAFE_GLYPH_GAMEPAD_BTN_DOWN},
        entry{VPAD_BUTTON_LEFT,    "VPAD_BUTTON_LEFT",    CAFE_GLYPH_GAMEPAD_BTN_LEFT},
        entry{VPAD_BUTTON_RIGHT,   "VPAD_BUTTON_RIGHT",   CAFE_GLYPH_GAMEPAD_BTN_RIGHT},
        entry{VPAD_BUTTON_A,       "VPAD_BUTTON_A",       CAFE_GLYPH_GAMEPAD_BTN_A},
        entry{VPAD_BUTTON_B,       "VPAD_BUTTON_B",       CAFE_GLYPH_GAMEPAD_BTN_B},
        entry{VPAD_BUTTON_X,       "VPAD_BUTTON_X",       CAFE_GLYPH_GAMEPAD_BTN_X},
        entry{VPAD_BUTTON_Y,       "VPAD_BUTTON_Y",       CAFE_GLYPH_GAMEPAD_BTN_Y},
        entry{VPAD_BUTTON_PLUS,    "VPAD_BUTTON_PLUS",    CAFE_GLYPH_GAMEPAD_BTN_PLUS},
        entry{VPAD_BUTTON_MINUS,   "VPAD_BUTTON_MINUS",   CAFE_GLYPH_GAMEPAD_BTN_MINUS},
        entry{VPAD_BUTTON_STICK_L, "VPAD_BUTTON_STICK_L", CAFE_GLYPH_GAMEPAD_BTN_STICK_L},
        entry{VPAD_BUTTON_STICK_R, "VPAD_BUTTON_STICK_R", CAFE_GLYPH_GAMEPAD_BTN_STICK_R},
        entry{VPAD_BUTTON_HOME,    "VPAD_BUTTON_HOME",    CAFE_GLYPH_GAMEPAD_BTN_HOME},
        entry{VPAD_BUTTON_TV,      "VPAD_BUTTON_TV",      CAFE_GLYPH_GAMEPAD_BTN_TV},
        entry{VPAD_BUTTON_SYNC,    "VPAD_BUTTON_SYNC",    "SYNC"},

        entry{VPAD_STICK_L_EMULATION_UP,    "VPAD_STICK_L_EMULATION_UP",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_UP},
        entry{VPAD_STICK_L_EMULATION_DOWN,  "VPAD_STICK_L_EMULATION_DOWN",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_DOWN},
        entry{VPAD_STICK_L_EMULATION_LEFT,  "VPAD_STICK_L_EMULATION_LEFT",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_LEFT},
        entry{VPAD_STICK_L_EMULATION_RIGHT, "VPAD_STICK_L_EMULATION_RIGHT",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_RIGHT},

        entry{VPAD_STICK_R_EMULATION_UP,    "VPAD_STICK_R_EMULATION_UP",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_UP},
        entry{VPAD_STICK_R_EMULATION_DOWN,  "VPAD_STICK_R_EMULATION_DOWN",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_DOWN},
        entry{VPAD_STICK_R_EMULATION_LEFT,  "VPAD_STICK_R_EMULATION_LEFT",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_LEFT},
        entry{VPAD_STICK_R_EMULATION_RIGHT, "VPAD_STICK_R_EMULATION_RIGHT",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_RIGHT},
    };


    button_set::button_set(const std::vector<std::string_view>& args)
    {
        for (auto token : args)
            for (auto entry : button_entries)
                if (entry.name == token)
                    buttons |= entry.button;
    }


    bool
    button_set::contains(VPADButtons btn)
        const noexcept
    {
        return btn & buttons;
    }


    string
    to_string(const button_set& bs)
    {
        string result;
        for (auto e : button_entries)
            if (e.button & bs.buttons)
                result = concat(result, e.name);
        return result;
    }


    string
    to_glyph(const button_set& bs, bool prefix)
    {
        string result;
        for (auto e : button_entries)
            if (e.button & bs.buttons)
                result = concat(result, e.glyph);
        if (!prefix || result.empty())
            return result;
        return CAFE_GLYPH_GAMEPAD " " + result;
    }


    bool
    update(VPADChan chan,
           const VPADStatus& status)
        noexcept
    {
        unsigned idx = chan;
        if (idx >= states.size())
            return false;
        if (status.error)
            return false;
        states[idx].hold    = status.hold;
        states[idx].trigger = status.trigger;
        states[idx].release = status.release;
        return true;
    }


    bool
    triggered(VPADChan channel,
              const button_combo& combo)
        noexcept
    {
        if (!holds_alternative<button_set>(combo))
            return false;

        if (channel < 0 || channel >= states.size())
            return false;

        const auto& state = states[channel];
        auto& vb = get<button_set>(combo);

        // Test 1: hold must be equal to combo
        if (state.hold != vb.buttons)
            return false;

        // Test 2: at least one triggered button must match combo
        return state.trigger & vb.buttons;
    }


} // namespace wups::utils::vpad
