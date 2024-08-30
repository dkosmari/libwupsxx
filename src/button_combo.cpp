/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <stdexcept>
#include <string_view>
#include <vector>


#include "wupsxx/button_combo.hpp"

#include "cafe_glyphs.h"


using std::array;
using std::string;
using std::string_view;
using std::uint32_t;

namespace wups::config {


    namespace {
        string
        concat(const string& a,
               const string& b,
               const char* sep = "+")
        {
            if (a.empty())
                return b;
            if (b.empty())
                return a;
            return a + sep + b;
        }


        std::vector<string_view>
        split_view(const string& str,
                   const string& sep = "+ ")
        {
            std::vector<string_view> result;
            using size_type = string::size_type;

            size_type begin_pos = 0;
            size_type end_pos = 0;

            for (;;) {

                begin_pos = str.find_first_not_of(sep, end_pos);

                // if string ends in separators, stop
                if (begin_pos == string::npos)
                    break;

                end_pos = str.find_first_of(sep, begin_pos);

                // if no more separators, append remaining of str and stop
                if (end_pos == string::npos) {
                    result.emplace_back(str.data() + begin_pos,
                                        str.data() + str.size());
                    break;
                }

                result.emplace_back(str.data() + begin_pos,
                                    str.data() + end_pos);

            }

            return result;
        }



        struct vpad_button_entry {
            VPADButtons button;
            const char* name;
            const char* glyph;
        };


#define E(x, y, z) vpad_button_entry{x, y, z}
        const auto vpad_button_entries = {
            E(VPAD_BUTTON_L,       "VPAD_BUTTON_L",       CAFE_GLYPH_GAMEPAD_BTN_L),
            E(VPAD_BUTTON_R,       "VPAD_BUTTON_R",       CAFE_GLYPH_GAMEPAD_BTN_R),
            E(VPAD_BUTTON_ZL,      "VPAD_BUTTON_ZL",      CAFE_GLYPH_GAMEPAD_BTN_ZL),
            E(VPAD_BUTTON_ZR,      "VPAD_BUTTON_ZR",      CAFE_GLYPH_GAMEPAD_BTN_ZR),
            E(VPAD_BUTTON_UP,      "VPAD_BUTTON_UP",      CAFE_GLYPH_GAMEPAD_BTN_UP),
            E(VPAD_BUTTON_DOWN,    "VPAD_BUTTON_DOWN",    CAFE_GLYPH_GAMEPAD_BTN_DOWN),
            E(VPAD_BUTTON_LEFT,    "VPAD_BUTTON_LEFT",    CAFE_GLYPH_GAMEPAD_BTN_LEFT),
            E(VPAD_BUTTON_RIGHT,   "VPAD_BUTTON_RIGHT",   CAFE_GLYPH_GAMEPAD_BTN_RIGHT),
            E(VPAD_BUTTON_A,       "VPAD_BUTTON_A",       CAFE_GLYPH_GAMEPAD_BTN_A),
            E(VPAD_BUTTON_B,       "VPAD_BUTTON_B",       CAFE_GLYPH_GAMEPAD_BTN_B),
            E(VPAD_BUTTON_X,       "VPAD_BUTTON_X",       CAFE_GLYPH_GAMEPAD_BTN_X),
            E(VPAD_BUTTON_Y,       "VPAD_BUTTON_Y",       CAFE_GLYPH_GAMEPAD_BTN_Y),
            E(VPAD_BUTTON_PLUS,    "VPAD_BUTTON_PLUS",    CAFE_GLYPH_GAMEPAD_BTN_PLUS),
            E(VPAD_BUTTON_MINUS,   "VPAD_BUTTON_MINUS",   CAFE_GLYPH_GAMEPAD_BTN_MINUS),
            E(VPAD_BUTTON_STICK_L, "VPAD_BUTTON_STICK_L", CAFE_GLYPH_GAMEPAD_BTN_STICK_L),
            E(VPAD_BUTTON_STICK_R, "VPAD_BUTTON_STICK_R", CAFE_GLYPH_GAMEPAD_BTN_STICK_R),
            E(VPAD_BUTTON_HOME,    "VPAD_BUTTON_HOME",    CAFE_GLYPH_GAMEPAD_BTN_HOME),
            E(VPAD_BUTTON_TV,      "VPAD_BUTTON_TV",      CAFE_GLYPH_GAMEPAD_BTN_TV),
            E(VPAD_BUTTON_SYNC,    "VPAD_BUTTON_SYNC",    "SYNC"),

            E(VPAD_STICK_L_EMULATION_UP,    "VPAD_STICK_L_EMULATION_UP",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_UP),
            E(VPAD_STICK_L_EMULATION_DOWN,  "VPAD_STICK_L_EMULATION_DOWN",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_DOWN),
            E(VPAD_STICK_L_EMULATION_LEFT,  "VPAD_STICK_L_EMULATION_LEFT",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_LEFT),
            E(VPAD_STICK_L_EMULATION_RIGHT, "VPAD_STICK_L_EMULATION_RIGHT",
              CAFE_GLYPH_GAMEPAD_STICK_L CAFE_GLYPH_ARROW_RIGHT),

            E(VPAD_STICK_R_EMULATION_UP,    "VPAD_STICK_R_EMULATION_UP",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_UP),
            E(VPAD_STICK_R_EMULATION_DOWN,  "VPAD_STICK_R_EMULATION_DOWN",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_DOWN),
            E(VPAD_STICK_R_EMULATION_LEFT,  "VPAD_STICK_R_EMULATION_LEFT",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_LEFT),
            E(VPAD_STICK_R_EMULATION_RIGHT, "VPAD_STICK_R_EMULATION_RIGHT",
              CAFE_GLYPH_GAMEPAD_STICK_R CAFE_GLYPH_ARROW_RIGHT),

        };
#undef E


        string
        vpad_combo_to_string(uint32_t combo)
        {
            string result;
            for (auto e : vpad_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        vpad_combo_to_glyph(uint32_t combo)
        {
            string result;
            for (auto e : vpad_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_core_button_entry {
            WPADButton button;
            const char* name;
            const char* glyph;
        };


        // Note: don't include nunchuk in core buttons.
#define E(x, y, z) wpad_core_button_entry{ x, y, z }
        const auto wpad_core_button_entries = {
            E(WPAD_BUTTON_UP,    "WPAD_BUTTON_UP",    CAFE_GLYPH_WIIMOTE_BTN_UP),
            E(WPAD_BUTTON_DOWN,  "WPAD_BUTTON_DOWN",  CAFE_GLYPH_WIIMOTE_BTN_DOWN),
            E(WPAD_BUTTON_LEFT,  "WPAD_BUTTON_LEFT",  CAFE_GLYPH_WIIMOTE_BTN_LEFT),
            E(WPAD_BUTTON_RIGHT, "WPAD_BUTTON_RIGHT", CAFE_GLYPH_WIIMOTE_BTN_RIGHT),
            E(WPAD_BUTTON_A,     "WPAD_BUTTON_A",     CAFE_GLYPH_WIIMOTE_BTN_A),
            E(WPAD_BUTTON_B,     "WPAD_BUTTON_B",     CAFE_GLYPH_WIIMOTE_BTN_B),
            E(WPAD_BUTTON_MINUS, "WPAD_BUTTON_MINUS", CAFE_GLYPH_WIIMOTE_BTN_MINUS),
            E(WPAD_BUTTON_HOME,  "WPAD_BUTTON_HOME",  CAFE_GLYPH_WIIMOTE_BTN_HOME),
            E(WPAD_BUTTON_PLUS,  "WPAD_BUTTON_PLUS",  CAFE_GLYPH_WIIMOTE_BTN_PLUS),
            E(WPAD_BUTTON_1,     "WPAD_BUTTON_1",     CAFE_GLYPH_WIIMOTE_BTN_1),
            E(WPAD_BUTTON_2,     "WPAD_BUTTON_2",     CAFE_GLYPH_WIIMOTE_BTN_2),
        };
#undef E


        string
        wpad_core_combo_to_string(uint32_t combo)
        {
            string result;
            for (auto e : wpad_core_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_core_combo_to_glyph(uint32_t combo)
        {
            string result;
            for (auto e : wpad_core_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_nunchuk_button_entry {
            WPADNunchukButton button;
            const char* name;
            const char* glyph;
        };

        // Note: no emulated buttons
#define E(x, y, z) wpad_nunchuk_button_entry{ x, y, z }
        const array wpad_nunchuk_button_entries = {
            E(WPAD_NUNCHUK_BUTTON_Z, "WPAD_NUNCHUK_BUTTON_Z", CAFE_GLYPH_NUNCHUK_BTN_Z),
            E(WPAD_NUNCHUK_BUTTON_C, "WPAD_NUNCHUK_BUTTON_C", CAFE_GLYPH_NUNCHUK_BTN_C),
        };
#undef E


        string
        wpad_nunchuk_combo_to_string(uint32_t combo)
        {
            string result;
            for (auto e : wpad_nunchuk_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_nunchuk_combo_to_glyph(uint32_t combo)
        {
            string result;
            for (auto e : wpad_nunchuk_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_classic_button_entry {
            WPADClassicButton button;
            const char* name;
            const char* glyph;
        };

        // Note: no emulated buttons
#define E(x, y, z) wpad_classic_button_entry{x, y, z}
        const auto wpad_classic_button_entries = {
            E(WPAD_CLASSIC_BUTTON_L,     "WPAD_CLASSIC_BUTTON_L",     CAFE_GLYPH_CLASSIC_BTN_L),
            E(WPAD_CLASSIC_BUTTON_R,     "WPAD_CLASSIC_BUTTON_R",     CAFE_GLYPH_CLASSIC_BTN_R),
            E(WPAD_CLASSIC_BUTTON_ZL,    "WPAD_CLASSIC_BUTTON_ZL",    CAFE_GLYPH_CLASSIC_BTN_ZL),
            E(WPAD_CLASSIC_BUTTON_ZR,    "WPAD_CLASSIC_BUTTON_ZR",    CAFE_GLYPH_CLASSIC_BTN_ZR),
            E(WPAD_CLASSIC_BUTTON_UP,    "WPAD_CLASSIC_BUTTON_UP",    CAFE_GLYPH_CLASSIC_BTN_UP),
            E(WPAD_CLASSIC_BUTTON_DOWN,  "WPAD_CLASSIC_BUTTON_DOWN",  CAFE_GLYPH_CLASSIC_BTN_DOWN),
            E(WPAD_CLASSIC_BUTTON_LEFT,  "WPAD_CLASSIC_BUTTON_LEFT",  CAFE_GLYPH_CLASSIC_BTN_LEFT),
            E(WPAD_CLASSIC_BUTTON_RIGHT, "WPAD_CLASSIC_BUTTON_RIGHT", CAFE_GLYPH_CLASSIC_BTN_RIGHT),
            E(WPAD_CLASSIC_BUTTON_MINUS, "WPAD_CLASSIC_BUTTON_MINUS", CAFE_GLYPH_CLASSIC_BTN_MINUS),
            E(WPAD_CLASSIC_BUTTON_HOME,  "WPAD_CLASSIC_BUTTON_HOME",  CAFE_GLYPH_CLASSIC_BTN_HOME),
            E(WPAD_CLASSIC_BUTTON_PLUS,  "WPAD_CLASSIC_BUTTON_PLUS",  CAFE_GLYPH_CLASSIC_BTN_PLUS),
            E(WPAD_CLASSIC_BUTTON_A,     "WPAD_CLASSIC_BUTTON_A",     CAFE_GLYPH_CLASSIC_BTN_A),
            E(WPAD_CLASSIC_BUTTON_B,     "WPAD_CLASSIC_BUTTON_B",     CAFE_GLYPH_CLASSIC_BTN_B),
            E(WPAD_CLASSIC_BUTTON_X,     "WPAD_CLASSIC_BUTTON_X",     CAFE_GLYPH_CLASSIC_BTN_X),
            E(WPAD_CLASSIC_BUTTON_Y,     "WPAD_CLASSIC_BUTTON_Y",     CAFE_GLYPH_CLASSIC_BTN_Y),
        };
#undef E


        string
        wpad_classic_combo_to_string(uint32_t combo)
        {
            string result;
            for (auto e : wpad_classic_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_classic_combo_to_glyph(uint32_t combo)
        {
            string result;
            for (auto e : wpad_classic_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_pro_button_entry {
            WPADProButton button;
            const char* name;
            const char* glyph;
        };

        // Note: no emulated buttons.
#define E(x, y, z) wpad_pro_button_entry{ x, y, z }
        const auto wpad_pro_button_entries = {
            E(WPAD_PRO_TRIGGER_L,      "WPAD_PRO_TRIGGER_L",      CAFE_GLYPH_PRO_BTN_L),
            E(WPAD_PRO_TRIGGER_R,      "WPAD_PRO_TRIGGER_R",      CAFE_GLYPH_PRO_BTN_R),
            E(WPAD_PRO_TRIGGER_ZL,     "WPAD_PRO_TRIGGER_ZL",     CAFE_GLYPH_PRO_BTN_ZL),
            E(WPAD_PRO_TRIGGER_ZR,     "WPAD_PRO_TRIGGER_ZR",     CAFE_GLYPH_PRO_BTN_ZR),
            E(WPAD_PRO_BUTTON_UP,      "WPAD_PRO_BUTTON_UP",      CAFE_GLYPH_PRO_BTN_UP),
            E(WPAD_PRO_BUTTON_DOWN,    "WPAD_PRO_BUTTON_DOWN",    CAFE_GLYPH_PRO_BTN_DOWN),
            E(WPAD_PRO_BUTTON_LEFT,    "WPAD_PRO_BUTTON_LEFT",    CAFE_GLYPH_PRO_BTN_LEFT),
            E(WPAD_PRO_BUTTON_RIGHT,   "WPAD_PRO_BUTTON_RIGHT",   CAFE_GLYPH_PRO_BTN_RIGHT),
            E(WPAD_PRO_BUTTON_MINUS,   "WPAD_PRO_BUTTON_MINUS",   CAFE_GLYPH_PRO_BTN_MINUS),
            E(WPAD_PRO_BUTTON_HOME,    "WPAD_PRO_BUTTON_HOME",    CAFE_GLYPH_PRO_BTN_HOME),
            E(WPAD_PRO_BUTTON_PLUS,    "WPAD_PRO_BUTTON_PLUS",    CAFE_GLYPH_PRO_BTN_PLUS),
            E(WPAD_PRO_BUTTON_A,       "WPAD_PRO_BUTTON_A",       CAFE_GLYPH_PRO_BTN_A),
            E(WPAD_PRO_BUTTON_B,       "WPAD_PRO_BUTTON_B",       CAFE_GLYPH_PRO_BTN_B),
            E(WPAD_PRO_BUTTON_X,       "WPAD_PRO_BUTTON_X",       CAFE_GLYPH_PRO_BTN_X),
            E(WPAD_PRO_BUTTON_Y,       "WPAD_PRO_BUTTON_Y",       CAFE_GLYPH_PRO_BTN_Y),
            E(WPAD_PRO_BUTTON_STICK_L, "WPAD_PRO_BUTTON_STICK_L", CAFE_GLYPH_PRO_BTN_STICK_L),
            E(WPAD_PRO_BUTTON_STICK_R, "WPAD_PRO_BUTTON_STICK_R", CAFE_GLYPH_PRO_BTN_STICK_R),
        };
#undef E


        string
        wpad_pro_combo_to_string(uint32_t combo)
        {
            string result;
            for (auto e : wpad_pro_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_pro_combo_to_glyph(uint32_t combo)
        {
            string result;
            for (auto e : wpad_pro_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct to_string_visitor {

            string
            operator ()(std::monostate)
                const
            {
                return {};
            }

            string
            operator ()(const vpad_combo& vc)
                const
            {
                return vpad_combo_to_string(vc.buttons);
            }

            string
            operator ()(const wpad_combo& wc)
                const
            {
                string result = wpad_core_combo_to_string(wc.core_buttons);

                switch (wc.ext) {
                case WPAD_EXT_NUNCHUK:
                case WPAD_EXT_MPLUS_NUNCHUK:
                    return concat(result,
                                  wpad_nunchuk_combo_to_string(wc.ext_buttons));
                case WPAD_EXT_CLASSIC:
                case WPAD_EXT_MPLUS_CLASSIC:
                    return concat(result,
                                  wpad_classic_combo_to_string(wc.ext_buttons));
                case WPAD_EXT_PRO_CONTROLLER:
                    return concat(result,
                                  wpad_pro_combo_to_string(wc.ext_buttons));
                default:
                    return result;
                }
            }

        };

    } // namespace


    string
    to_string(const button_combo& bc)
    {
        return visit(to_string_visitor{}, bc);
    }


    struct to_glyph_visitor {

        string
        operator ()(std::monostate)
            const
        {
            return {};
        }


        string
        operator ()(const vpad_combo& vc)
            const
        {
            string prefix = CAFE_GLYPH_GAMEPAD " ";
            string result = vpad_combo_to_glyph(vc.buttons);
            if (!result.empty())
                return prefix + result;
            return result;
        }


        string
        operator ()(const wpad_combo& wc)
            const
        {
            string prefix = CAFE_GLYPH_WIIMOTE " ";
            string result = wpad_core_combo_to_glyph(wc.core_buttons);

            switch (wc.ext) {
            case WPAD_EXT_NUNCHUK:
            case WPAD_EXT_MPLUS_NUNCHUK:
                result = concat(result,
                                wpad_nunchuk_combo_to_glyph(wc.ext_buttons));
                break;
            case WPAD_EXT_CLASSIC:
            case WPAD_EXT_MPLUS_CLASSIC:
                result = concat(result,
                                wpad_classic_combo_to_glyph(wc.ext_buttons));
                break;
            case WPAD_EXT_PRO_CONTROLLER:
                result = concat(result,
                                wpad_pro_combo_to_glyph(wc.ext_buttons));
                break;
            default:
                ;
            }

            if (!result.empty())
                return prefix + result;
            return result;
        }

    };


    string
    to_glyph(const button_combo& bc)
    {
        return visit(to_glyph_visitor{}, bc);
    }


    button_combo
    button_combo_from_string(const string& str)
    {
        auto tokens = split_view(str, "+ \t\n\r");
        if (tokens.empty())
            return {};

        // sanity check: cannot mix vpad and wpad buttons
        unsigned num_vpad = 0;
        unsigned num_wpad = 0;

        for (auto token : tokens) {
            if (token.starts_with("VPAD_"))
                ++num_vpad;
            if (token.starts_with("WPAD_"))
                ++num_wpad;
        }

        if (num_vpad && num_wpad)
            throw std::runtime_error{"cannot use both VPAD and WPAD buttons in the same combo"};

        if (num_vpad + num_wpad != tokens.size())
            throw std::runtime_error{"invalid token detected"};

        if (num_vpad) {
            vpad_combo combo{};
            for (auto token : tokens)
                for (auto entry : vpad_button_entries)
                    if (entry.name == token)
                        combo.buttons |= entry.button;
            return combo;
        } else {
            wpad_combo combo{};
            unsigned num_nunchuk = 0;
            unsigned num_classic = 0;
            unsigned num_pro = 0;

            for (auto token : tokens) {

                if (token.starts_with("WPAD_BUTTON_"))
                    for (auto entry : wpad_core_button_entries)
                        if (entry.name == token)
                            combo.core_buttons |= entry.button;

                if (token.starts_with("WPAD_NUNCHUK_")) {
                    combo.ext = WPAD_EXT_NUNCHUK;
                    ++num_nunchuk;
                    for (auto entry : wpad_nunchuk_button_entries)
                        if (entry.name == token)
                            combo.ext_buttons |= entry.button;
                }

                if (token.starts_with("WPAD_CLASSIC_")) {
                    combo.ext = WPAD_EXT_CLASSIC;
                    ++num_classic;
                    for (auto entry : wpad_classic_button_entries)
                        if (entry.name == token)
                            combo.ext_buttons |= entry.button;
                }

                if (token.starts_with("WPAD_PRO_")) {
                    combo.ext = WPAD_EXT_PRO_CONTROLLER;
                    ++num_pro;
                    for (auto entry : wpad_pro_button_entries)
                        if (entry.name == token)
                            combo.ext_buttons |= entry.button;
                }

                unsigned num_extensions = 0;
                if (num_nunchuk)
                    ++num_extensions;
                if (num_classic)
                    ++num_extensions;
                if (num_pro)
                    ++num_extensions;
                if (num_extensions > 1)
                    throw std::runtime_error{"cannot mix multiple extensions in combo"};
            }
            return combo;
        }
    }


} // namespace wups::config
