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


#define ENTRY1(x)                           \
        vpad_button_entry{                  \
            VPAD_BUTTON_ ## x,              \
            "VPAD_BUTTON_" #x,              \
            CAFE_GLYPH_GAMEPAD_BTN_ ## x    \
        }

#define ENTRY2(x, y)            \
        vpad_button_entry{      \
            VPAD_BUTTON_ ## x,  \
            "VPAD_BUTTON_" #x,  \
            y                   \
        }

#define ENTRY3(x, y)                                                \
        vpad_button_entry{                                          \
            VPAD_STICK_ ## x ## _EMULATION_ ## y,                   \
            "VPAD_STICK_" #x "_EMULATION_" #y,                      \
            CAFE_GLYPH_GAMEPAD_STICK_ ## x CAFE_GLYPH_ARROW_ ## y   \
        }

        const array vpad_button_entries{
            ENTRY1(A),
            ENTRY1(B),
            ENTRY1(X),
            ENTRY1(Y),
            ENTRY1(LEFT),
            ENTRY1(RIGHT),
            ENTRY1(UP),
            ENTRY1(DOWN),
            ENTRY1(ZL),
            ENTRY1(ZR),
            ENTRY1(L),
            ENTRY1(R),
            ENTRY1(PLUS),
            ENTRY1(MINUS),
            ENTRY1(HOME),
            ENTRY2(SYNC, "SYNC"),
            ENTRY1(STICK_R),
            ENTRY1(STICK_L),
            ENTRY1(TV),
            ENTRY3(R, LEFT),
            ENTRY3(R, RIGHT),
            ENTRY3(R, UP),
            ENTRY3(R, DOWN),
            ENTRY3(L, LEFT),
            ENTRY3(L, RIGHT),
            ENTRY3(L, UP),
            ENTRY3(L, DOWN),
        };

#undef ENTRY1
#undef ENTRY2
#undef ENTRY3


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


#define ENTRY1(x)                           \
        wpad_core_button_entry{             \
            WPAD_BUTTON_ ## x,              \
            "WPAD_BUTTON_" #x,              \
            CAFE_GLYPH_WIIMOTE_BTN_ ## x    \
        }

#define ENTRY2(x)                           \
        wpad_core_button_entry{             \
            WPAD_BUTTON_ ## x,              \
            "WPAD_BUTTON_" #x,              \
            CAFE_GLYPH_NUNCHUK_BTN_ ## x    \
        }

        const array wpad_core_button_entries{
            ENTRY1(LEFT),
            ENTRY1(RIGHT),
            ENTRY1(DOWN),
            ENTRY1(UP),
            ENTRY1(PLUS),
            ENTRY1(2),
            ENTRY1(1),
            ENTRY1(B),
            ENTRY1(A),
            ENTRY1(MINUS),
            ENTRY2(Z),
            ENTRY2(C),
            ENTRY1(HOME),
        };

#undef ENTRY1
#undef ENTRY2


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


#define ENTRY1(x)                           \
        wpad_nunchuk_button_entry{          \
            WPAD_NUNCHUK_BUTTON_ ## x,      \
            "WPAD_NUNCHUK_BUTTON_" #x,      \
            CAFE_GLYPH_NUNCHUK_BTN_ ## x    \
        }

#define ENTRY2(x)                                           \
        wpad_nunchuk_button_entry{                          \
            WPAD_NUNCHUK_STICK_EMULATION_ ## x,             \
            "WPAD_NUNCHUK_STICK_EMULATION_" #x ,            \
            CAFE_GLYPH_NUNCHUK_STICK CAFE_GLYPH_ARROW_ ## x \
        }

        const array wpad_nunchuk_button_entries{
            ENTRY1(Z),
            ENTRY1(C),
            ENTRY2(LEFT),
            ENTRY2(RIGHT),
            ENTRY2(DOWN),
            ENTRY2(UP),
        };

#undef ENTRY1
#undef ENTRY2


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

#define ENTRY1(x)                           \
        wpad_classic_button_entry{          \
            WPAD_CLASSIC_BUTTON_ ## x,      \
            "WPAD_CLASSIC_BUTTON_" #x,      \
            CAFE_GLYPH_CLASSIC_BTN_ ## x    \
        }

#define ENTRY2(x, y)                                                \
        wpad_classic_button_entry{                                  \
            WPAD_CLASSIC_STICK_ ## x ## _EMULATION_ ## y,           \
           "WPAD_CLASSIC_STICK_" #x "_EMULATION_" #y,               \
            CAFE_GLYPH_CLASSIC_STICK_ ## x CAFE_GLYPH_ARROW_ ## y   \
        }


        const array wpad_classic_button_entries{
            ENTRY1(UP),
            ENTRY1(LEFT),
            ENTRY1(ZR),
            ENTRY1(X),
            ENTRY1(A),
            ENTRY1(Y),
            ENTRY1(B),
            ENTRY1(ZL),
            ENTRY1(R),
            ENTRY1(PLUS),
            ENTRY1(HOME),
            ENTRY1(MINUS),
            ENTRY1(L),
            ENTRY1(DOWN),
            ENTRY1(RIGHT),
            ENTRY2(L, LEFT),
            ENTRY2(L, RIGHT),
            ENTRY2(L, DOWN),
            ENTRY2(L, UP),
            ENTRY2(R, LEFT),
            ENTRY2(R, RIGHT),
            ENTRY2(R, DOWN),
            ENTRY2(R, UP),
        };

#undef ENTRY1
#undef ENTRY2


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


#define ENTRY1(x)                       \
        wpad_pro_button_entry{          \
            WPAD_PRO_BUTTON_ ## x,      \
            "WPAD_PRO_BUTTON_" #x,      \
            CAFE_GLYPH_PRO_BTN_ ## x    \
        }

#define ENTRY2(x)                       \
        wpad_pro_button_entry{          \
            WPAD_PRO_TRIGGER_ ## x,     \
            "WPAD_PRO_TRIGGER_" #x,     \
            CAFE_GLYPH_PRO_BTN_ ##x,    \
        }

#define ENTRY3(x, y)                                            \
        wpad_pro_button_entry{                                  \
            WPAD_PRO_STICK_ ## x ## _EMULATION_ ## y,           \
            "WPAD_PRO_STICK_" #x "_EMULATION_" #y,              \
            CAFE_GLYPH_PRO_STICK_ ## x CAFE_GLYPH_ARROW_ ## y   \
        }


        const array wpad_pro_button_entries{
            ENTRY1(UP),
            ENTRY1(DOWN),
            ENTRY1(LEFT),
            ENTRY1(RIGHT),
            ENTRY1(A),
            ENTRY1(B),
            ENTRY1(X),
            ENTRY1(Y),
            ENTRY1(MINUS),
            ENTRY1(HOME),
            ENTRY1(PLUS),
            ENTRY1(STICK_L),
            ENTRY1(STICK_R),
            ENTRY2(L),
            ENTRY2(R),
            ENTRY2(ZL),
            ENTRY2(ZR),
            ENTRY3(L, UP),
            ENTRY3(L, DOWN),
            ENTRY3(L, LEFT),
            ENTRY3(L, RIGHT),
            ENTRY3(R, UP),
            ENTRY3(R, DOWN),
            ENTRY3(R, LEFT),
            ENTRY3(R, RIGHT),
        };

#undef ENTRY1
#undef ENTRY2
#undef ENTRY3


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
            return vpad_combo_to_glyph(vc.buttons);
        }


        string
        operator ()(const wpad_combo& wc)
            const
        {
            string result = wpad_core_combo_to_glyph(wc.core_buttons);

            switch (wc.ext) {
            case WPAD_EXT_NUNCHUK:
            case WPAD_EXT_MPLUS_NUNCHUK:
                return concat(result,
                              wpad_nunchuk_combo_to_glyph(wc.ext_buttons));
            case WPAD_EXT_CLASSIC:
            case WPAD_EXT_MPLUS_CLASSIC:
                return concat(result,
                              wpad_classic_combo_to_glyph(wc.ext_buttons));
            case WPAD_EXT_PRO_CONTROLLER:
                return concat(result,
                              wpad_pro_combo_to_glyph(wc.ext_buttons));
            default:
                return result;
            }

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
