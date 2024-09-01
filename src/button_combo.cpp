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
#include <utility>              // pair<>
#include <vector>


#include "wupsxx/button_combo.hpp"

#include "cafe_glyphs.h"

#include "wpad_status.h"


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


        // return a reference to a variant entry, initialized if necessary
        template<typename T,
                 typename... Ts>
        T&
        get_init(std::variant<Ts...>& v)
            noexcept
        {
            if (!holds_alternative<T>(v))
                v = T{};
            return get<T>(v);
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
            uint16_t button;
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
        wpad_core_combo_to_string(uint16_t combo)
        {
            string result;
            for (auto e : wpad_core_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_core_combo_to_glyph(uint16_t combo)
        {
            string result;
            for (auto e : wpad_core_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_nunchuk_button_entry {
            uint16_t button;
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
        wpad_nunchuk_combo_to_string(uint16_t combo)
        {
            string result;
            for (auto e : wpad_nunchuk_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_nunchuk_combo_to_glyph(uint16_t combo)
        {
            string result;
            for (auto e : wpad_nunchuk_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_classic_button_entry {
            uint16_t button;
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
        wpad_classic_combo_to_string(uint16_t combo)
        {
            string result;
            for (auto e : wpad_classic_button_entries)
                if (e.button & combo)
                    result = concat(result, e.name);
            return result;
        }


        string
        wpad_classic_combo_to_glyph(uint16_t combo)
        {
            string result;
            for (auto e : wpad_classic_button_entries)
                if (e.button & combo)
                    result = concat(result, e.glyph);
            return result;
        }


        struct wpad_pro_button_entry {
            uint32_t button;
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



        struct wpad_ext_buttons_to_string_visitor {

            string
            operator ()(std::monostate)
                const
            {
                return {};
            }

            string
            operator ()(const wpad_nunchuk_buttons& nb)
                const
            {
                return wpad_nunchuk_combo_to_string(nb.buttons);
            }

            string
            operator ()(const wpad_classic_buttons& cb)
                const
            {
                return wpad_classic_combo_to_string(cb.buttons);
            }

            string
            operator ()(const wpad_pro_buttons& pb)
                const
            {
                return wpad_pro_combo_to_string(pb.buttons);
            }

        };


        struct button_combo_to_string_visitor {

            string
            operator ()(std::monostate)
                const
            {
                return {};
            }

            string
            operator ()(const vpad_buttons& vb)
                const
            {
                return vpad_combo_to_string(vb.buttons);
            }

            string
            operator ()(const wpad_buttons& wb)
                const
            {
                string core = wpad_core_combo_to_string(wb.core.buttons);
                string ext = visit(wpad_ext_buttons_to_string_visitor{},
                                   wb.ext);
                return concat(core, ext);
            }

        };

    } // namespace


    string
    to_string(const button_combo& bc)
    {
        return visit(button_combo_to_string_visitor{},
                     bc);
    }


    struct wpad_ext_buttons_to_glyph_visitor {

        string
        operator ()(std::monostate)
            const
        {
            return {};
        }

        string
        operator ()(const wpad_nunchuk_buttons& nb)
            const
        {
            return wpad_nunchuk_combo_to_glyph(nb.buttons);
        }

        string
        operator ()(const wpad_classic_buttons& cb)
            const
        {
            return wpad_classic_combo_to_glyph(cb.buttons);
        }

        string
        operator ()(const wpad_pro_buttons& pb)
            const
        {
            return wpad_pro_combo_to_glyph(pb.buttons);
        }

    };


    struct button_combo_to_glyph_visitor {

        string
        operator ()(std::monostate)
            const
        {
            return {};
        }


        string
        operator ()(const vpad_buttons& vb)
            const
        {
            string result = vpad_combo_to_glyph(vb.buttons);
            if (result.empty())
                return {};
            return CAFE_GLYPH_GAMEPAD " " + result;

        }


        string
        operator ()(const wpad_buttons& wb)
            const
        {
            string core = wpad_core_combo_to_glyph(wb.core.buttons);
            string ext = visit(wpad_ext_buttons_to_glyph_visitor{},
                               wb.ext);
            string combined = concat(core, ext);
            if (combined.empty())
                return {};
            return CAFE_GLYPH_WIIMOTE " " + combined;
        }

    };


    string
    to_glyph(const button_combo& bc)
    {
        return visit(button_combo_to_glyph_visitor{},
                     bc);
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
            vpad_buttons vb{};
            for (auto token : tokens)
                for (auto entry : vpad_button_entries)
                    if (entry.name == token)
                        vb.buttons |= entry.button;
            return vb;
        } else {
            wpad_buttons wb{};
            unsigned num_nunchuk = 0;
            unsigned num_classic = 0;
            unsigned num_pro = 0;

            for (auto token : tokens) {

                if (token.starts_with("WPAD_BUTTON_"))
                    for (auto entry : wpad_core_button_entries)
                        if (entry.name == token)
                            wb.core.buttons |= entry.button;

                if (token.starts_with("WPAD_NUNCHUK_")) {
                    auto& nunchuk = get_init<wpad_nunchuk_buttons>(wb.ext);
                    ++num_nunchuk;
                    for (auto entry : wpad_nunchuk_button_entries)
                        if (entry.name == token)
                            nunchuk.buttons |= entry.button;
                }

                if (token.starts_with("WPAD_CLASSIC_")) {
                    auto& classic = get_init<wpad_classic_buttons>(wb.ext);
                    ++num_classic;
                    for (auto entry : wpad_classic_button_entries)
                        if (entry.name == token)
                            classic.buttons |= entry.button;
                }

                if (token.starts_with("WPAD_PRO_")) {
                    auto& pro = get_init<wpad_pro_buttons>(wb.ext);
                    ++num_pro;
                    for (auto entry : wpad_pro_button_entries)
                        if (entry.name == token)
                            pro.buttons |= entry.button;
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
            return wb;
        }
    }



    namespace {

        struct state16 {
            uint16_t hold    = 0;
            uint16_t trigger = 0;
            uint16_t release = 0;
        };

        struct state32 {
            uint32_t hold = 0;
            uint32_t trigger = 0;
            uint32_t release = 0;
        };


        struct vpad_state : state32 {};

        array<vpad_state, 2> vpad_states;


        struct wpad_core_state    : state16 {};
        struct wpad_nunchuk_state : state16 {};
        struct wpad_classic_state : state16 {};
        struct wpad_pro_state     : state32 {};


        struct wpad_state {
            wpad_core_state core;

            std::variant<std::monostate,
                         wpad_nunchuk_state,
                         wpad_classic_state,
                         wpad_pro_state> ext;
        };


        array<wpad_state, 7> wpad_states;


        template<typename T>
        std::pair<T, T>
        calc_trigger_release(T old_hold, T new_hold)
            noexcept
        {
            uint16_t diff    = old_hold ^ new_hold;
            uint16_t trigger = diff & new_hold;
            uint16_t release = diff & old_hold;
            return {trigger, release};
        }


        constexpr uint16_t wpad_core_mask =
                      WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT |
                      WPAD_BUTTON_DOWN | WPAD_BUTTON_UP |
                      WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS |
                      WPAD_BUTTON_A | WPAD_BUTTON_B |
                      WPAD_BUTTON_1 | WPAD_BUTTON_2 |
                      WPAD_BUTTON_HOME;

        constexpr uint16_t wpad_nunchuk_mask = WPAD_BUTTON_Z | WPAD_BUTTON_C;


        void
        update_wpad_core_common(WPADChan channel,
                                const WPADStatus& status)
            noexcept
        {
            auto& core = wpad_states[channel].core;

            uint16_t old_hold = core.hold;
            uint16_t new_hold = status.buttons & wpad_core_mask;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            core.hold    = new_hold;
            core.trigger = trigger;
            core.release = release;
        }


        void
        update_wpad_core(WPADChan channel,
                         const WPADStatus* status)
            noexcept
        {
            update_wpad_core_common(channel, *status);
            wpad_states[channel].ext = {};
        }


        void
        update_wpad_nunchuk(WPADChan channel,
                            const WPADNunchukStatus* status)
            noexcept
        {
            update_wpad_core_common(channel, status->core);

            auto& nunchuk = get_init<wpad_nunchuk_state>(wpad_states[channel].ext);

            uint16_t old_hold = nunchuk.hold;
            uint16_t new_hold = status->core.buttons & wpad_nunchuk_mask;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            nunchuk.hold    = new_hold;
            nunchuk.trigger = trigger;
            nunchuk.release = release;
        }


        void
        update_wpad_classic(WPADChan channel,
                            const WPADClassicStatus* status)
            noexcept
        {
            update_wpad_core_common(channel, status->core);

            auto& classic = get_init<wpad_classic_state>(wpad_states[channel].ext);

            uint16_t old_hold = classic.hold;
            uint16_t new_hold = status->ext.buttons;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            classic.hold    = new_hold;
            classic.trigger = trigger;
            classic.release = release;
        }


        void
        update_wpad_pro(WPADChan channel,
                        const WPADProStatus* status)
            noexcept
        {
            wpad_states[channel].core = {};

            auto& pro = get_init<wpad_pro_state>(wpad_states[channel].ext);

            uint32_t old_hold = pro.hold;
            uint32_t new_hold = status->ext.buttons;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            pro.hold    = new_hold;
            pro.trigger = trigger;
            pro.release = release;
        }


        void
        update_wpad_mplus(WPADChan channel,
                          const WPADMPlusStatus* status)
            noexcept
        {
            switch (status->core.extensionType) {
            case WPAD_EXT_MPLUS:
                update_wpad_core(channel,
                                 reinterpret_cast<const WPADStatus*>(status));
                break;
            case WPAD_EXT_MPLUS_NUNCHUK:
                update_wpad_nunchuk(channel,
                                    reinterpret_cast<const WPADNunchukStatus*>(status));
                break;
            case WPAD_EXT_MPLUS_CLASSIC:
                update_wpad_classic(channel,
                                    reinterpret_cast<const WPADClassicStatus*>(status));
                break;
            }
        }

    } // namespace


    bool
    vpad_update(VPADChan chan,
                const VPADStatus& status)
        noexcept
    {
        unsigned idx = chan;
        if (idx >= vpad_states.size())
            return false;
        if (status.error)
            return false;
        vpad_states[idx].hold    = status.hold;
        vpad_states[idx].trigger = status.trigger;
        vpad_states[idx].release = status.release;
        return true;
    }


    bool
    wpad_update(WPADChan channel,
                const WPADStatus* status)
        noexcept
    {
        if (channel >= wpad_states.size())
            return false;
        if (!status)
            return false;

        if (status->error)
            return false;

        WPADDataFormat fmt = WPADGetDataFormat(channel);

        switch (fmt) {

        case WPAD_FMT_CORE:
        case WPAD_FMT_CORE_ACC:
        case WPAD_FMT_CORE_ACC_DPD:
            update_wpad_core(channel, status);
            break;

        case WPAD_FMT_NUNCHUK:
        case WPAD_FMT_NUNCHUK_ACC:
        case WPAD_FMT_NUNCHUK_ACC_DPD:
            update_wpad_nunchuk(channel,
                                reinterpret_cast<const WPADNunchukStatus*>(status));
            break;

        case WPAD_FMT_CLASSIC:
        case WPAD_FMT_CLASSIC_ACC:
        case WPAD_FMT_CLASSIC_ACC_DPD:
        case WPAD_FMT_GUITAR:
        case WPAD_FMT_DRUM:
        case WPAD_FMT_TAIKO:
            update_wpad_classic(channel,
                                reinterpret_cast<const WPADClassicStatus*>(status));
            break;

        case WPAD_FMT_CORE_ACC_DPD_FULL:
            // TODO: not implemented
            break;

        case WPAD_FMT_TRAIN:
            // ?
            break;

        case WPAD_FMT_BALANCE_BOARD:
            // TODO: not implemented
            break;

        case WPAD_FMT_MPLUS:
            update_wpad_mplus(channel,
                              reinterpret_cast<const WPADMPlusStatus*>(status));
            break;

        case WPAD_FMT_PRO_CONTROLLER:
            update_wpad_pro(channel,
                            reinterpret_cast<const WPADProStatus*>(status));
            break;

        }

        return true;
    }


    // Returns true if a wpad state extension has no buttons held down
    struct wpad_state_ext_is_clear_visitor {
        bool operator ()(std::monostate) const
        { return true; }

        bool operator ()(const wpad_nunchuk_state& ns) const
        { return ns.hold == 0; }

        bool operator ()(const wpad_classic_state& cs) const
        { return cs.hold == 0; }

        bool operator ()(const wpad_pro_state& ps) const
        { return ps.hold == 0; }
    };


    struct wpad_combo_ext_check_visitor {

        const wpad_state& state;
        const bool core_triggered;

        wpad_combo_ext_check_visitor(const wpad_state& st,
                                     bool ct)
            noexcept :
            state(st),
            core_triggered{ct}
        {}

        // when wpad combo uses no extension buttons
        bool
        operator ()(std::monostate)
            const noexcept
        {
            // if no extension is plugged in, match depends on the core test
            if (!holds_alternative<std::monostate>(state.ext))
                return core_triggered;

            // there is an extension plugged in, ensure its buttons are all unpressed
            if (visit(wpad_state_ext_is_clear_visitor{},
                      state.ext))
                return core_triggered; // match depends on the core test

            // otherwise, at least one extension button is pressed: no match
            return false;
        }

        // when wpad combo has nunchuk buttons
        bool
        operator ()(const wpad_nunchuk_buttons& nb)
            const noexcept
        {
            if (!holds_alternative<wpad_nunchuk_state>(state.ext))
                return false;

            auto& ns = get<wpad_nunchuk_state>(state.ext);
            // fail early if buttons don't match
            if (ns.hold != nb.buttons)
                return false;

            // if the trigger happened on the core, there's a match
            if (core_triggered)
                return true;

            // otherwise the trigger must happen in the extension
            return ns.trigger & nb.buttons;
        }

        // when wpad combo has classic buttons
        bool
        operator ()(const wpad_classic_buttons& cb)
            const noexcept
        {
            if (!holds_alternative<wpad_classic_state>(state.ext))
                return false;

            auto& cs = get<wpad_classic_state>(state.ext);
            // fail early if buttons don't match
            if (cs.hold != cb.buttons)
                return false;

            // if the trigger happened on the core, there's a match
            if (core_triggered)
                return true;

            // otherwise the trigger must happen in the extension
            return cs.trigger & cb.buttons;
        }

        // when wpad combo is pro buttons
        bool
        operator ()(const wpad_pro_buttons& pb)
            const noexcept
        {
            if (!holds_alternative<wpad_pro_state>(state.ext))
                return false;

            auto ps = get<wpad_pro_state>(state.ext);
            // fail early if buttons don't match
            if (ps.hold != pb.buttons)
                return false;

            // ignore core match

            // otherwise the trigger must happen in the extension
            return ps.trigger & pb.buttons;
        }

    }; // struct wpad_combo_ext_check_visitor


    bool
    vpad_triggered(VPADChan channel,
                   const button_combo& combo)
        noexcept
    {
        if (!holds_alternative<vpad_buttons>(combo))
            return false;

        if (channel < 0 || channel >= vpad_states.size())
            return false;

        const auto& state = vpad_states[channel];
        auto& vb = get<vpad_buttons>(combo);

        // Test 1: hold must be equal to combo
        if (state.hold != vb.buttons)
            return false;

        // Test 2: at least one triggered button must match combo
        return state.trigger & vb.buttons;
    }


    bool
    wpad_triggered(WPADChan channel,
                   const button_combo& combo)
        noexcept
    {
        if (!holds_alternative<wpad_buttons>(combo))
            return false;

        if (channel < 0 || channel >= wpad_states.size())
            return false;

        const auto& state = wpad_states[channel];
        auto& wb = get<wpad_buttons>(combo);

        // Test 1: core hold must be equal to core combo
        if (state.core.hold != wb.core.buttons)
            return false;

        // Test 1.1: check if a core button was the trigger
        bool core_triggered = state.core.trigger & wb.core.buttons;

        // Test 2: check if the extension matches the combo
        return visit(wpad_combo_ext_check_visitor{state, core_triggered},
                     wb.ext);
    }


} // namespace wups::config
