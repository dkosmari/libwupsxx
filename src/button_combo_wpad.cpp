/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <stdexcept>
#include <string>

#include <padscore/wpad.h>

#include "wupsxx/button_combo.hpp"

#include "wupsxx/cafe_glyphs.h"
#include "wupsxx/logger.hpp"

#include "wpad_status.h"
#include "utils.hpp"


using namespace std::literals;

using std::array;
using std::string;

namespace logger = wups::logger;


namespace wups::utils::wpad {

    namespace core {

        struct entry {
            WPADButton button;
            const char* name;
            const char* glyph;
        };


        // Note: don't include nunchuk in core buttons.
        const auto button_entries = {
            entry{WPAD_BUTTON_UP,    "WPAD_BUTTON_UP",    CAFE_GLYPH_WIIMOTE_BTN_UP},
            entry{WPAD_BUTTON_DOWN,  "WPAD_BUTTON_DOWN",  CAFE_GLYPH_WIIMOTE_BTN_DOWN},
            entry{WPAD_BUTTON_LEFT,  "WPAD_BUTTON_LEFT",  CAFE_GLYPH_WIIMOTE_BTN_LEFT},
            entry{WPAD_BUTTON_RIGHT, "WPAD_BUTTON_RIGHT", CAFE_GLYPH_WIIMOTE_BTN_RIGHT},
            entry{WPAD_BUTTON_A,     "WPAD_BUTTON_A",     CAFE_GLYPH_WIIMOTE_BTN_A},
            entry{WPAD_BUTTON_B,     "WPAD_BUTTON_B",     CAFE_GLYPH_WIIMOTE_BTN_B},
            entry{WPAD_BUTTON_MINUS, "WPAD_BUTTON_MINUS", CAFE_GLYPH_WIIMOTE_BTN_MINUS},
            entry{WPAD_BUTTON_HOME,  "WPAD_BUTTON_HOME",  CAFE_GLYPH_WIIMOTE_BTN_HOME},
            entry{WPAD_BUTTON_PLUS,  "WPAD_BUTTON_PLUS",  CAFE_GLYPH_WIIMOTE_BTN_PLUS},
            entry{WPAD_BUTTON_1,     "WPAD_BUTTON_1",     CAFE_GLYPH_WIIMOTE_BTN_1},
            entry{WPAD_BUTTON_2,     "WPAD_BUTTON_2",     CAFE_GLYPH_WIIMOTE_BTN_2},
        };


        constexpr uint16_t button_mask =
            WPAD_BUTTON_UP   | WPAD_BUTTON_DOWN  |
            WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT |
            WPAD_BUTTON_A    | WPAD_BUTTON_B     |
            WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS |
            WPAD_BUTTON_1    | WPAD_BUTTON_2     |
            WPAD_BUTTON_HOME;


        string
        to_string(const button_set& bs)
        {
            string result;
            for (auto e : button_entries)
                if (e.button & bs.buttons)
                    result = utils::concat(result, e.name);
            return result;
        }


        string
        to_glyph(const button_set& bs)
        {
            string result;
            for (auto e : button_entries)
                if (e.button & bs.buttons)
                    result = utils::concat(result, e.glyph);
            return result;
        }

    } // namespace wups::utils::wpad::core


    namespace nunchuk {

        struct entry {
            WPADNunchukButton button;
            const char* name;
            const char* glyph;
        };


        // Note: no emulated buttons
        const array button_entries = {
            entry{WPAD_NUNCHUK_BUTTON_Z, "WPAD_NUNCHUK_BUTTON_Z", CAFE_GLYPH_NUNCHUK_BTN_Z},
            entry{WPAD_NUNCHUK_BUTTON_C, "WPAD_NUNCHUK_BUTTON_C", CAFE_GLYPH_NUNCHUK_BTN_C},
        };


        constexpr uint16_t button_mask = WPAD_BUTTON_Z | WPAD_BUTTON_C;


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
        to_glyph(const button_set& bs)
        {
            string result;
            for (auto e : button_entries)
                if (e.button & bs.buttons)
                    result = concat(result, e.glyph);
            return result;
        }

    } // namespace wups::utils::wpad::nunchuk


    namespace classic {

        struct entry {
            WPADClassicButton button;
            const char* name;
            const char* glyph;
        };


        // Note: no emulated buttons
        const auto button_entries = {
            entry{WPAD_CLASSIC_BUTTON_L,     "WPAD_CLASSIC_BUTTON_L",     CAFE_GLYPH_CLASSIC_BTN_L},
            entry{WPAD_CLASSIC_BUTTON_R,     "WPAD_CLASSIC_BUTTON_R",     CAFE_GLYPH_CLASSIC_BTN_R},
            entry{WPAD_CLASSIC_BUTTON_ZL,    "WPAD_CLASSIC_BUTTON_ZL",    CAFE_GLYPH_CLASSIC_BTN_ZL},
            entry{WPAD_CLASSIC_BUTTON_ZR,    "WPAD_CLASSIC_BUTTON_ZR",    CAFE_GLYPH_CLASSIC_BTN_ZR},
            entry{WPAD_CLASSIC_BUTTON_UP,    "WPAD_CLASSIC_BUTTON_UP",    CAFE_GLYPH_CLASSIC_BTN_UP},
            entry{WPAD_CLASSIC_BUTTON_DOWN,  "WPAD_CLASSIC_BUTTON_DOWN",  CAFE_GLYPH_CLASSIC_BTN_DOWN},
            entry{WPAD_CLASSIC_BUTTON_LEFT,  "WPAD_CLASSIC_BUTTON_LEFT",  CAFE_GLYPH_CLASSIC_BTN_LEFT},
            entry{WPAD_CLASSIC_BUTTON_RIGHT, "WPAD_CLASSIC_BUTTON_RIGHT", CAFE_GLYPH_CLASSIC_BTN_RIGHT},
            entry{WPAD_CLASSIC_BUTTON_MINUS, "WPAD_CLASSIC_BUTTON_MINUS", CAFE_GLYPH_CLASSIC_BTN_MINUS},
            entry{WPAD_CLASSIC_BUTTON_HOME,  "WPAD_CLASSIC_BUTTON_HOME",  CAFE_GLYPH_CLASSIC_BTN_HOME},
            entry{WPAD_CLASSIC_BUTTON_PLUS,  "WPAD_CLASSIC_BUTTON_PLUS",  CAFE_GLYPH_CLASSIC_BTN_PLUS},
            entry{WPAD_CLASSIC_BUTTON_A,     "WPAD_CLASSIC_BUTTON_A",     CAFE_GLYPH_CLASSIC_BTN_A},
            entry{WPAD_CLASSIC_BUTTON_B,     "WPAD_CLASSIC_BUTTON_B",     CAFE_GLYPH_CLASSIC_BTN_B},
            entry{WPAD_CLASSIC_BUTTON_X,     "WPAD_CLASSIC_BUTTON_X",     CAFE_GLYPH_CLASSIC_BTN_X},
            entry{WPAD_CLASSIC_BUTTON_Y,     "WPAD_CLASSIC_BUTTON_Y",     CAFE_GLYPH_CLASSIC_BTN_Y},
        };


        constexpr uint16_t button_mask =
                      WPAD_CLASSIC_BUTTON_L     | WPAD_CLASSIC_BUTTON_R     |
                      WPAD_CLASSIC_BUTTON_ZL    | WPAD_CLASSIC_BUTTON_ZR    |
                      WPAD_CLASSIC_BUTTON_UP    | WPAD_CLASSIC_BUTTON_DOWN  |
                      WPAD_CLASSIC_BUTTON_LEFT  | WPAD_CLASSIC_BUTTON_RIGHT |
                      WPAD_CLASSIC_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_PLUS  |
                      WPAD_CLASSIC_BUTTON_A     | WPAD_CLASSIC_BUTTON_B     |
                      WPAD_CLASSIC_BUTTON_X     | WPAD_CLASSIC_BUTTON_Y     |
                      WPAD_CLASSIC_BUTTON_HOME;


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
        to_glyph(const button_set& bs)
        {
            string result;
            for (auto e : button_entries)
                if (e.button & bs.buttons)
                    result = concat(result, e.glyph);
            return result;
        }

    } // namespace wups::utils::wpad::classic


    namespace pro {

        struct entry {
            WPADProButton button;
            const char* name;
            const char* glyph;
        };


        // Note: no emulated buttons.
        const auto button_entries = {
            entry{WPAD_PRO_TRIGGER_L,      "WPAD_PRO_TRIGGER_L",      CAFE_GLYPH_PRO_BTN_L},
            entry{WPAD_PRO_TRIGGER_R,      "WPAD_PRO_TRIGGER_R",      CAFE_GLYPH_PRO_BTN_R},
            entry{WPAD_PRO_TRIGGER_ZL,     "WPAD_PRO_TRIGGER_ZL",     CAFE_GLYPH_PRO_BTN_ZL},
            entry{WPAD_PRO_TRIGGER_ZR,     "WPAD_PRO_TRIGGER_ZR",     CAFE_GLYPH_PRO_BTN_ZR},
            entry{WPAD_PRO_BUTTON_UP,      "WPAD_PRO_BUTTON_UP",      CAFE_GLYPH_PRO_BTN_UP},
            entry{WPAD_PRO_BUTTON_DOWN,    "WPAD_PRO_BUTTON_DOWN",    CAFE_GLYPH_PRO_BTN_DOWN},
            entry{WPAD_PRO_BUTTON_LEFT,    "WPAD_PRO_BUTTON_LEFT",    CAFE_GLYPH_PRO_BTN_LEFT},
            entry{WPAD_PRO_BUTTON_RIGHT,   "WPAD_PRO_BUTTON_RIGHT",   CAFE_GLYPH_PRO_BTN_RIGHT},
            entry{WPAD_PRO_BUTTON_MINUS,   "WPAD_PRO_BUTTON_MINUS",   CAFE_GLYPH_PRO_BTN_MINUS},
            entry{WPAD_PRO_BUTTON_HOME,    "WPAD_PRO_BUTTON_HOME",    CAFE_GLYPH_PRO_BTN_HOME},
            entry{WPAD_PRO_BUTTON_PLUS,    "WPAD_PRO_BUTTON_PLUS",    CAFE_GLYPH_PRO_BTN_PLUS},
            entry{WPAD_PRO_BUTTON_A,       "WPAD_PRO_BUTTON_A",       CAFE_GLYPH_PRO_BTN_A},
            entry{WPAD_PRO_BUTTON_B,       "WPAD_PRO_BUTTON_B",       CAFE_GLYPH_PRO_BTN_B},
            entry{WPAD_PRO_BUTTON_X,       "WPAD_PRO_BUTTON_X",       CAFE_GLYPH_PRO_BTN_X},
            entry{WPAD_PRO_BUTTON_Y,       "WPAD_PRO_BUTTON_Y",       CAFE_GLYPH_PRO_BTN_Y},
            entry{WPAD_PRO_BUTTON_STICK_L, "WPAD_PRO_BUTTON_STICK_L", CAFE_GLYPH_PRO_BTN_STICK_L},
            entry{WPAD_PRO_BUTTON_STICK_R, "WPAD_PRO_BUTTON_STICK_R", CAFE_GLYPH_PRO_BTN_STICK_R},
        };


        constexpr uint32_t button_mask =
            WPAD_PRO_BUTTON_UP      | WPAD_PRO_BUTTON_DOWN    |
            WPAD_PRO_BUTTON_LEFT    | WPAD_PRO_BUTTON_RIGHT   |
            WPAD_PRO_TRIGGER_L      | WPAD_PRO_TRIGGER_R      |
            WPAD_PRO_TRIGGER_ZL     | WPAD_PRO_TRIGGER_ZR     |
            WPAD_PRO_BUTTON_A       | WPAD_PRO_BUTTON_B       |
            WPAD_PRO_BUTTON_X       | WPAD_PRO_BUTTON_Y       |
            WPAD_PRO_BUTTON_PLUS    | WPAD_PRO_BUTTON_MINUS   |
            WPAD_PRO_BUTTON_STICK_L | WPAD_PRO_BUTTON_STICK_R |
            WPAD_PRO_BUTTON_HOME;


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
        to_glyph(const button_set& bs)
        {
            string result;
            for (auto e : button_entries)
                if (e.button & bs.buttons)
                    result = concat(result, e.glyph);
            return result;
        }

    } // namespace wups::utils::wpad::pro


    button_set::button_set(const std::vector<std::string_view>& args)
    {
        using utils::ensure;

        unsigned num_nunchuk = 0;
        unsigned num_classic = 0;
        unsigned num_pro = 0;

        for (auto token : args) {

            if (token.starts_with("WPAD_BUTTON_"))
                for (auto entry : core::button_entries)
                    if (entry.name == token)
                        core.buttons |= entry.button;

            if (token.starts_with("WPAD_NUNCHUK_")) {
                auto& nunchuk = ensure<nunchuk::button_set>(ext);
                ++num_nunchuk;
                for (auto entry : nunchuk::button_entries)
                    if (entry.name == token)
                        nunchuk.buttons |= entry.button;
            }

            if (token.starts_with("WPAD_CLASSIC_")) {
                auto& classic = ensure<classic::button_set>(ext);
                ++num_classic;
                for (auto entry : classic::button_entries)
                    if (entry.name == token)
                        classic.buttons |= entry.button;
            }

            if (token.starts_with("WPAD_PRO_")) {
                auto& pro = ensure<pro::button_set>(ext);
                ++num_pro;
                for (auto entry : pro::button_entries)
                    if (entry.name == token)
                        pro.buttons |= entry.button;
            }
        }

        // Ensure the user didn't mix buttons from multiple extensions.
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


    bool
    button_set::contains(WPADButton btn)
        const noexcept
    {
        return btn & core.buttons & core::button_mask;
    }


    bool
    button_set::contains(WPADNunchukButton btn)
        const noexcept
    {
        auto ptr = get_if<nunchuk::button_set>(&ext);
        if (!ptr)
            return false;
        return btn & ptr->buttons & nunchuk::button_mask;
    }


    bool
    button_set::contains(WPADClassicButton btn)
        const noexcept
    {
        auto ptr = get_if<classic::button_set>(&ext);
        if (!ptr)
            return false;
        return btn & ptr->buttons & classic::button_mask;
    }


    bool
    button_set::contains(WPADProButton btn)
        const noexcept
    {
        auto ptr = get_if<pro::button_set>(&ext);
        if (!ptr)
            return false;
        return btn & ptr->buttons & pro::button_mask;
    }


    string
    to_string(const button_set& bs)
    {
        string core = to_string(bs.core);

        auto visitor = utils::overloaded{
            [](std::monostate) -> string { return ""s; },
            [](const auto& xbs) -> string { return to_string(xbs); }
        };
        string ext = visit(visitor, bs.ext);
        return concat(core, ext);
    }


    string
    to_glyph(const button_set& bs, bool prefix)
    {
        string core = to_glyph(bs.core);

        auto visitor = utils::overloaded{
            [](std::monostate) -> string { return ""s; },
            [](const auto& xbs) -> string { return to_glyph(xbs); }
        };
        string ext = visit(visitor, bs.ext);
        string result = concat(core, ext);
        if (!prefix || result.empty())
            return result;
        return CAFE_GLYPH_WIIMOTE " " + result;
    }


    namespace {

        template<typename T>
        std::pair<T, T>
        calc_trigger_release(T old_hold, T new_hold)
            noexcept
        {
            T diff    = old_hold ^ new_hold;
            T trigger = diff & new_hold;
            T release = diff & old_hold;
            return {trigger, release};
        }


        array<button_state, 7> states;


        void
        update_core_common(WPADChan channel,
                           const WPADStatus& status)
            noexcept
        {
            auto& core = states[channel].core;

            uint16_t old_hold = core.hold;
            uint16_t new_hold = status.buttons & core::button_mask;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            core.hold    = new_hold;
            core.trigger = trigger;
            core.release = release;
        }


        void
        update_core(WPADChan channel,
                    const WPADStatus* status)
            noexcept
        {
            update_core_common(channel, *status);
            states[channel].ext = {};
        }


        void
        update_nunchuk(WPADChan channel,
                            const WPADNunchukStatus* status)
            noexcept
        {
            update_core_common(channel, status->core);

            auto& nunchuk = ensure<nunchuk_button_state>(states[channel].ext);

            uint16_t old_hold = nunchuk.hold;
            uint16_t new_hold = status->core.buttons & nunchuk::button_mask;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            nunchuk.hold    = new_hold;
            nunchuk.trigger = trigger;
            nunchuk.release = release;
        }


        void
        update_classic(WPADChan channel,
                       const WPADClassicStatus* status)
            noexcept
        {
            update_core_common(channel, status->core);

            auto& classic = ensure<classic_button_state>(states[channel].ext);

            uint16_t old_hold = classic.hold;
            uint16_t new_hold = status->ext.buttons & classic::button_mask;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            classic.hold    = new_hold;
            classic.trigger = trigger;
            classic.release = release;
        }


        void
        update_pro(WPADChan channel,
                   const WPADProStatus* status)
            noexcept
        {
            states[channel].core = {};

            auto& pro = ensure<pro_button_state>(states[channel].ext);

            uint32_t old_hold = pro.hold;
            uint32_t new_hold = status->ext.buttons & pro::button_mask;

            auto [trigger, release] = calc_trigger_release(old_hold, new_hold);

            pro.hold    = new_hold;
            pro.trigger = trigger;
            pro.release = release;
        }

    } // namespace


    bool
    update(WPADChan channel,
           const WPADStatus* status)
        noexcept
    {
        if (channel < 0 || channel >= states.size()) [[unlikely]]
            return false;
        if (!status) [[unlikely]]
            return false;

        if (status->error)
            return false;

        switch (status->extensionType) {

        case WPAD_EXT_CORE:
        case WPAD_EXT_MPLUS:
            update_core(channel, status);
            break;

        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
            update_nunchuk(channel, reinterpret_cast<const WPADNunchukStatus*>(status));
            break;

        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            update_classic(channel, reinterpret_cast<const WPADClassicStatus*>(status));
            break;

        case WPAD_EXT_PRO_CONTROLLER:
            update_pro(channel, reinterpret_cast<const WPADProStatus*>(status));
            break;

        }

        return true;
    }


    namespace {
        struct check_ext_combo_visitor {

            const button_state& state;
            const bool core_triggered;

            check_ext_combo_visitor(const button_state& st,
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
                if (holds_alternative<std::monostate>(state.ext))
                    return core_triggered;

                // there is an extension plugged in, check its buttons are all unpressed
                auto empty_hold_visitor = utils::overloaded{
                    [](std::monostate) { return true; },
                    [](const auto& st) { return st.hold == 0; }
                };
                if (visit(empty_hold_visitor, state.ext))
                    return core_triggered; // match depends on the core test

                // otherwise, at least one extension button is pressed: no match
                return false;
            }


            // when wpad combo has nunchuk buttons
            bool
            operator ()(const nunchuk::button_set& bs)
                const noexcept
            {
                auto* st = get_if<nunchuk_button_state>(&state.ext);
                if (!st)
                    return false;

                // fail early if buttons don't match
                if (st->hold != bs.buttons)
                    return false;

                // if the trigger happened on the core, there's a match
                if (core_triggered)
                    return true;

                // otherwise the trigger must happen in the extension
                return st->trigger & bs.buttons;
            }


            // when wpad combo has classic buttons
            bool
            operator ()(const classic::button_set& bs)
                const noexcept
            {
                auto* st = get_if<classic_button_state>(&state.ext);
                if (!st)
                    return false;

                // fail early if buttons don't match
                if (st->hold != bs.buttons)
                    return false;

                // if the trigger happened on the core, there's a match
                if (core_triggered)
                    return true;

                // otherwise the trigger must happen in the extension
                return st->trigger & bs.buttons;
            }


            // when wpad combo is pro buttons
            bool
            operator ()(const pro::button_set& bs)
                const noexcept
            {
                auto* st = get_if<classic_button_state>(&state.ext);
                if (!st)
                    return false;

                // fail early if buttons don't match
                if (st->hold != bs.buttons)
                    return false;

                // ignore core

                // otherwise the trigger must happen in the extension
                return st->trigger & bs.buttons;
            }

        }; // struct combo_ext_check_visitor
    } // namespace


    bool
    triggered(WPADChan channel,
              const button_combo& combo)
        noexcept
    {
        const auto* bs = get_if<button_set>(&combo);
        if (!bs)
            return false;

        if (channel < 0 || channel >= states.size()) [[unlikely]]
            return false;

        const auto& state = states[channel];

        // Test 1: core hold must be equal to core combo
        if (state.core.hold != bs->core.buttons)
            return false;

        // Test 1.1: check if a core button was the trigger
        bool core_triggered = state.core.trigger & bs->core.buttons;

        // Test 2: check if the current extension matches the combo extension
        return visit(check_ext_combo_visitor{state, core_triggered},
                     bs->ext);
    }



    const button_state&
    get_button_state(WPADChan channel)
    {
        if (channel < 0 || channel >= states.size()) [[unlikely]]
            throw std::invalid_argument{"invalid wpad channel"};
        return states[channel];
    }

} // namespace wups::utils::wpad
