/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <memory>
#include <stdexcept>
#include <string>

#include <buttoncombo/api.h>

#include "wupsxx/button_combo.hpp"

#include "wupsxx/cafe_glyphs.h"
#include "wupsxx/logger.hpp"

#include "utils.hpp"


using std::string;
using std::uint32_t;


namespace wups::button_combo {


    error::error(ButtonComboModule_Error e) :
        std::runtime_error{ButtonComboModule_GetStatusStr(e)},
        code{e}
    {}


    error::error(const std::string& msg, ButtonComboModule_Error e) :
        std::runtime_error{msg + ": " + ButtonComboModule_GetStatusStr(e)},
        code{e}
    {}


    combo::combo(const std::string& arg)
    {
        auto tokens = utils::split_view(arg, "+, \t\n\r");

        for (auto token : tokens) {

            if (token == "ALL")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_ALL;

            if (token == "VPAD")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_VPAD;
            if (token == "VPAD_0")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0;
            if (token == "VPAD_1")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1;

            if (token == "WPAD")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD;
            if (token == "WPAD_0")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0;
            if (token == "WPAD_1")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1;
            if (token == "WPAD_2")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2;
            if (token == "WPAD_3")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3;
            if (token == "WPAD_4")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4;
            if (token == "WPAD_5")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5;
            if (token == "WPAD_6")
                controllers |= BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6;

            if (token == "A")
                buttons |= BCMPAD_BUTTON_A;
            if (token == "B")
                buttons |= BCMPAD_BUTTON_B;
            if (token == "X")
                buttons |= BCMPAD_BUTTON_X;
            if (token == "Y")
                buttons |= BCMPAD_BUTTON_Y;
            if (token == "LEFT")
                buttons |= BCMPAD_BUTTON_LEFT;
            if (token == "RIGHT")
                buttons |= BCMPAD_BUTTON_RIGHT;
            if (token == "UP")
                buttons |= BCMPAD_BUTTON_UP;
            if (token == "DOWN")
                buttons |= BCMPAD_BUTTON_DOWN;
            if (token == "ZL")
                buttons |= BCMPAD_BUTTON_ZL;
            if (token == "ZR")
                buttons |= BCMPAD_BUTTON_ZR;
            if (token == "L")
                buttons |= BCMPAD_BUTTON_L;
            if (token == "R")
                buttons |= BCMPAD_BUTTON_R;
            if (token == "PLUS")
                buttons |= BCMPAD_BUTTON_PLUS;
            if (token == "MINUS")
                buttons |= BCMPAD_BUTTON_MINUS;
            if (token == "STICK_R")
                buttons |= BCMPAD_BUTTON_STICK_R;
            if (token == "STICK_L")
                buttons |= BCMPAD_BUTTON_STICK_L;
            if (token == "TV")
                buttons |= BCMPAD_BUTTON_TV;
            if (token == "RESERVED")
                buttons |= BCMPAD_BUTTON_RESERVED_BIT;
            if (token == "1")
                buttons |= BCMPAD_BUTTON_1;
            if (token == "2")
                buttons |= BCMPAD_BUTTON_2;
            if (token == "C")
                buttons |= BCMPAD_BUTTON_C;
            if (token == "Z")
                buttons |= BCMPAD_BUTTON_Z;

        }
    }


    combo
    combo::from_vpad(std::uint32_t buttons)
        noexcept
    {
        combo result;
        result.controllers = BUTTON_COMBO_MODULE_CONTROLLER_VPAD;

        if (buttons & VPAD_BUTTON_A)
            result.buttons |= BCMPAD_BUTTON_A;
        if (buttons & VPAD_BUTTON_B)
            result.buttons |= BCMPAD_BUTTON_B;
        if (buttons & VPAD_BUTTON_X)
            result.buttons |= BCMPAD_BUTTON_X;
        if (buttons & VPAD_BUTTON_Y)
            result.buttons |= BCMPAD_BUTTON_Y;
        if (buttons & VPAD_BUTTON_LEFT)
            result.buttons |= BCMPAD_BUTTON_LEFT;
        if (buttons & VPAD_BUTTON_RIGHT)
            result.buttons |= BCMPAD_BUTTON_RIGHT;
        if (buttons & VPAD_BUTTON_UP)
            result.buttons |= BCMPAD_BUTTON_UP;
        if (buttons & VPAD_BUTTON_DOWN)
            result.buttons |= BCMPAD_BUTTON_DOWN;
        if (buttons & VPAD_BUTTON_ZL)
            result.buttons |= BCMPAD_BUTTON_ZL;
        if (buttons & VPAD_BUTTON_ZR)
            result.buttons |= BCMPAD_BUTTON_ZR;
        if (buttons & VPAD_BUTTON_L)
            result.buttons |= BCMPAD_BUTTON_L;
        if (buttons & VPAD_BUTTON_R)
            result.buttons |= BCMPAD_BUTTON_R;
        if (buttons & VPAD_BUTTON_PLUS)
            result.buttons |= BCMPAD_BUTTON_PLUS;
        if (buttons & VPAD_BUTTON_MINUS)
            result.buttons |= BCMPAD_BUTTON_MINUS;
        if (buttons & VPAD_BUTTON_STICK_R)
            result.buttons |= BCMPAD_BUTTON_STICK_R;
        if (buttons & VPAD_BUTTON_STICK_L)
            result.buttons |= BCMPAD_BUTTON_STICK_L;
        if (buttons & VPAD_BUTTON_TV)
            result.buttons |= BCMPAD_BUTTON_TV;

        return result;
    }


    combo
    combo::from_wpad_core(std::uint32_t buttons)
        noexcept
    {
        combo result;
        result.controllers = BUTTON_COMBO_MODULE_CONTROLLER_WPAD;

        if (buttons & WPAD_BUTTON_LEFT)
            result.buttons |= BCMPAD_BUTTON_LEFT;
        if (buttons & WPAD_BUTTON_RIGHT)
            result.buttons |= BCMPAD_BUTTON_RIGHT;
        if (buttons & WPAD_BUTTON_DOWN)
            result.buttons |= BCMPAD_BUTTON_DOWN;
        if (buttons & WPAD_BUTTON_UP)
            result.buttons |= BCMPAD_BUTTON_UP;
        if (buttons & WPAD_BUTTON_PLUS)
            result.buttons |= BCMPAD_BUTTON_PLUS;
        if (buttons & WPAD_BUTTON_MINUS)
            result.buttons |= BCMPAD_BUTTON_MINUS;
        if (buttons & WPAD_BUTTON_1)
            result.buttons |= BCMPAD_BUTTON_1;
        if (buttons & WPAD_BUTTON_2)
            result.buttons |= BCMPAD_BUTTON_2;
        if (buttons & WPAD_BUTTON_A)
            result.buttons |= BCMPAD_BUTTON_A;
        if (buttons & WPAD_BUTTON_B)
            result.buttons |= BCMPAD_BUTTON_B;

        return result;
    }


    combo
    combo::from_wpad_nunchuk(std::uint32_t core, std::uint32_t ext)
        noexcept
    {
        combo result = from_wpad_core(core);

        if (ext & WPAD_BUTTON_C)
            result.buttons |= BCMPAD_BUTTON_C;
        if (ext & WPAD_BUTTON_Z)
            result.buttons |= BCMPAD_BUTTON_Z;

        return result;
    }


    combo
    combo::from_wpad_classic(std::uint32_t core, std::uint32_t ext)
        noexcept
    {
        combo result = from_wpad_core(core);

        if (ext & WPAD_CLASSIC_BUTTON_A)
            result.buttons |= BCMPAD_BUTTON_A;
        if (ext & WPAD_CLASSIC_BUTTON_B)
            result.buttons |= BCMPAD_BUTTON_B;
        if (ext & WPAD_CLASSIC_BUTTON_X)
            result.buttons |= BCMPAD_BUTTON_X;
        if (ext & WPAD_CLASSIC_BUTTON_Y)
            result.buttons |= BCMPAD_BUTTON_Y;
        if (ext & WPAD_CLASSIC_BUTTON_LEFT)
            result.buttons |= BCMPAD_BUTTON_LEFT;
        if (ext & WPAD_CLASSIC_BUTTON_RIGHT)
            result.buttons |= BCMPAD_BUTTON_RIGHT;
        if (ext & WPAD_CLASSIC_BUTTON_UP)
            result.buttons |= BCMPAD_BUTTON_UP;
        if (ext & WPAD_CLASSIC_BUTTON_DOWN)
            result.buttons |= BCMPAD_BUTTON_DOWN;
        if (ext & WPAD_CLASSIC_BUTTON_ZL)
            result.buttons |= BCMPAD_BUTTON_ZL;
        if (ext & WPAD_CLASSIC_BUTTON_ZR)
            result.buttons |= BCMPAD_BUTTON_ZR;
        if (ext & WPAD_CLASSIC_BUTTON_L)
            result.buttons |= BCMPAD_BUTTON_L;
        if (ext & WPAD_CLASSIC_BUTTON_R)
            result.buttons |= BCMPAD_BUTTON_R;
        if (ext & WPAD_CLASSIC_BUTTON_PLUS)
            result.buttons |= BCMPAD_BUTTON_PLUS;
        if (ext & WPAD_CLASSIC_BUTTON_MINUS)
            result.buttons |= BCMPAD_BUTTON_MINUS;

        return result;
    }


    combo
    combo::from_wpad_pro(std::uint32_t ext)
        noexcept
    {
        combo result;
        result.controllers = BUTTON_COMBO_MODULE_CONTROLLER_WPAD;

        if (ext & WPAD_PRO_BUTTON_A)
            result.buttons |= BCMPAD_BUTTON_A;
        if (ext & WPAD_PRO_BUTTON_B)
            result.buttons |= BCMPAD_BUTTON_B;
        if (ext & WPAD_PRO_BUTTON_X)
            result.buttons |= BCMPAD_BUTTON_X;
        if (ext & WPAD_PRO_BUTTON_Y)
            result.buttons |= BCMPAD_BUTTON_Y;
        if (ext & WPAD_PRO_BUTTON_LEFT)
            result.buttons |= BCMPAD_BUTTON_LEFT;
        if (ext & WPAD_PRO_BUTTON_RIGHT)
            result.buttons |= BCMPAD_BUTTON_RIGHT;
        if (ext & WPAD_PRO_BUTTON_UP)
            result.buttons |= BCMPAD_BUTTON_UP;
        if (ext & WPAD_PRO_BUTTON_DOWN)
            result.buttons |= BCMPAD_BUTTON_DOWN;
        if (ext & WPAD_PRO_BUTTON_ZL)
            result.buttons |= BCMPAD_BUTTON_ZL;
        if (ext & WPAD_PRO_BUTTON_ZR)
            result.buttons |= BCMPAD_BUTTON_ZR;
        if (ext & WPAD_PRO_BUTTON_L)
            result.buttons |= BCMPAD_BUTTON_L;
        if (ext & WPAD_PRO_BUTTON_R)
            result.buttons |= BCMPAD_BUTTON_R;
        if (ext & WPAD_PRO_BUTTON_PLUS)
            result.buttons |= BCMPAD_BUTTON_PLUS;
        if (ext & WPAD_PRO_BUTTON_MINUS)
            result.buttons |= BCMPAD_BUTTON_MINUS;
        if (ext & WPAD_PRO_BUTTON_STICK_R)
            result.buttons |= BCMPAD_BUTTON_STICK_R;
        if (ext & WPAD_PRO_BUTTON_STICK_L)
            result.buttons |= BCMPAD_BUTTON_STICK_L;

        return result;
    }


    bool
    combo::is_empty()
        const noexcept
    {
        return !controllers || !buttons;
    }


    bool
    is_available(const combo& c)
    {
        ButtonComboModule_ButtonComboOptions options {
            .controllerMask = c.controllers,
            .combo = c.buttons
        };
        ButtonComboModule_ComboStatus status{};
        auto e = ButtonComboModule_CheckComboAvailable(&options, &status);
        if (e)
            throw error{e};

        return status == BUTTON_COMBO_MODULE_COMBO_STATUS_VALID;
    }


    bool
    is_conflicted(handle h)
    {
        ButtonComboModule_ComboStatus status{};
        auto e = ButtonComboModule_GetButtonComboStatus(h, &status);
        if (e)
            throw error{e};
        return status == BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT;
    }


    namespace {

        using callback_func_t = std::function<void(ctr_set,
                                                   handle)>;

        void
        callback_wrapper(ButtonComboModule_ControllerTypes controllers,
                         ButtonComboModule_ComboHandle h,
                         void* ctx)
            noexcept
        {
            auto func = reinterpret_cast<callback_func_t*>(ctx);
            (*func)(controllers, h);
        }

    }


    // Return handle and conflict status.
    std::pair<handle, bool>
    create(const std::string& label,
           const combo& c,
           callback_func_t callback)
    {
        handle h{};
        ButtonComboModule_ComboStatus status{};

        auto f = std::make_unique<callback_func_t>(std::move(callback));

        // Workaround until ButtonComboModule accepts empty combos
        combo real_c = c;
        if (c.is_empty()) {
            real_c.controllers = BUTTON_COMBO_MODULE_CONTROLLER_ALL;
            real_c.buttons = static_cast<ButtonComboModule_Buttons>(~0);
        }

        auto e = ButtonComboModule_AddButtonComboPressDownEx(label.data(),
                                                             real_c.controllers,
                                                             real_c.buttons,
                                                             callback_wrapper,
                                                             f.get(),
                                                             false,
                                                             &h,
                                                             &status);
        if (e)
            throw error{e};

        // Workaround until ButtonComboModule accepts empty combos
        if (c.is_empty()) {
            try {
                bool conflict = update(h, c);
                if (conflict)
                    status = BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT;
            }
            catch (std::exception& e) {
                logger::printf("button_combo workaround failed: %s\n", e.what());
                destroy(h);
                throw;
            }
        }

        f.release();
        return { h, status == BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT };
    }


    void
    destroy(handle& h)
    {
        if (h != nullptr) {
            // First, delete the context.
            ButtonComboModule_CallbackOptions cb_opt;
            auto e = ButtonComboModule_GetButtonComboCallback(h, &cb_opt);
            if (e)
                throw error{e};
            delete reinterpret_cast<callback_func_t*>(cb_opt.context);

            ButtonComboModule_RemoveButtonCombo(h);
            h = {};
        }
    }


    bool
    update(handle& h, const combo& c)
    {
        ButtonComboModule_ComboStatus status{};

        auto e = ButtonComboModule_UpdateControllerMask(h, c.controllers, nullptr);
        if (e)
            throw error{e};

        e = ButtonComboModule_UpdateButtonCombo(h, c.buttons, &status);
        if  (e)
            throw error{e};

        return status == BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT;
    }


    namespace {

        std::string
        to_string(ctr_set arg)
        {
            if (arg == BUTTON_COMBO_MODULE_CONTROLLER_ALL)
                return "ALL";

            using utils::concat;
            std::string result;

            if ((arg & BUTTON_COMBO_MODULE_CONTROLLER_VPAD)
                == BUTTON_COMBO_MODULE_CONTROLLER_VPAD)
                result = concat(result, "VPAD");
            else {
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0)
                    result = concat(result, "VPAD_0");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1)
                    result = concat(result, "VPAD_1");
            }

            if ((arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD)
                == BUTTON_COMBO_MODULE_CONTROLLER_WPAD)
                result = concat(result, "WPAD");
            else {
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0)
                    result = concat(result, "WPAD_0");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1)
                    result = concat(result, "WPAD_1");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2)
                    result = concat(result, "WPAD_2");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3)
                    result = concat(result, "WPAD_3");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4)
                    result = concat(result, "WPAD_4");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5)
                    result = concat(result, "WPAD_5");
                if (arg & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6)
                    result = concat(result, "WPAD_6");
            }

            return result;
        }


        std::string
        to_string(btn_set arg)
        {
            using utils::concat;
            std::string result;

            if (arg & BCMPAD_BUTTON_A)
                result = concat(result, "A");
            if (arg & BCMPAD_BUTTON_B)
                result = concat(result, "B");
            if (arg & BCMPAD_BUTTON_X)
                result = concat(result, "X");
            if (arg & BCMPAD_BUTTON_Y)
                result = concat(result, "Y");
            if (arg & BCMPAD_BUTTON_LEFT)
                result = concat(result, "LEFT");
            if (arg & BCMPAD_BUTTON_RIGHT)
                result = concat(result, "RIGHT");
            if (arg & BCMPAD_BUTTON_UP)
                result = concat(result, "UP");
            if (arg & BCMPAD_BUTTON_DOWN)
                result = concat(result, "DOWN");
            if (arg & BCMPAD_BUTTON_ZL)
                result = concat(result, "ZL");
            if (arg & BCMPAD_BUTTON_ZR)
                result = concat(result, "ZR");
            if (arg & BCMPAD_BUTTON_L)
                result = concat(result, "L");
            if (arg & BCMPAD_BUTTON_R)
                result = concat(result, "R");
            if (arg & BCMPAD_BUTTON_PLUS)
                result = concat(result, "PLUS");
            if (arg & BCMPAD_BUTTON_MINUS)
                result = concat(result, "MINUS");
            if (arg & BCMPAD_BUTTON_STICK_R)
                result = concat(result, "STICK_R");
            if (arg & BCMPAD_BUTTON_STICK_L)
                result = concat(result, "STICK_L");
            if (arg & BCMPAD_BUTTON_TV)
                result = concat(result, "TV");
            if (arg & BCMPAD_BUTTON_RESERVED_BIT)
                result = concat(result, "RESERVED");
            if (arg & BCMPAD_BUTTON_1)
                result = concat(result, "1");
            if (arg & BCMPAD_BUTTON_2)
                result = concat(result, "2");
            if (arg & BCMPAD_BUTTON_C)
                result = concat(result, "C");
            if (arg & BCMPAD_BUTTON_Z)
                result = concat(result, "Z");

            return result;
        }

    }


    std::string
    to_string(const combo& c)
    {
        return utils::concat(to_string(c.controllers), to_string(c.buttons));
    }


    namespace {

        std::string
        to_glyph(btn_set arg)
        {
            auto concat = [](const std::string& a, const std::string& b)
            {
                return utils::concat(a, b, " ");
            };

            std::string result;

            if (arg & BCMPAD_BUTTON_A)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_A);
            if (arg & BCMPAD_BUTTON_B)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_B);
            if (arg & BCMPAD_BUTTON_X)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_X);
            if (arg & BCMPAD_BUTTON_Y)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_Y);
            if (arg & BCMPAD_BUTTON_LEFT)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_LEFT);
            if (arg & BCMPAD_BUTTON_RIGHT)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_RIGHT);
            if (arg & BCMPAD_BUTTON_UP)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_UP);
            if (arg & BCMPAD_BUTTON_DOWN)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_DOWN);
            if (arg & BCMPAD_BUTTON_ZL)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_ZL);
            if (arg & BCMPAD_BUTTON_ZR)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_ZR);
            if (arg & BCMPAD_BUTTON_L)
                result = concat(result, CAFE_GLYPH_BTN_L);
            if (arg & BCMPAD_BUTTON_R)
                result = concat(result, CAFE_GLYPH_BTN_R);
            if (arg & BCMPAD_BUTTON_PLUS)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_PLUS);
            if (arg & BCMPAD_BUTTON_MINUS)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_MINUS);
            if (arg & BCMPAD_BUTTON_STICK_R)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_STICK_R);
            if (arg & BCMPAD_BUTTON_STICK_L)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_STICK_L);
            if (arg & BCMPAD_BUTTON_TV)
                result = concat(result, CAFE_GLYPH_GAMEPAD_BTN_TV);
            if (arg & BCMPAD_BUTTON_RESERVED_BIT)
                result = concat(result, "?");

            if (arg & BCMPAD_BUTTON_1)
                result = concat(result, CAFE_GLYPH_WIIMOTE_BTN_1);
            if (arg & BCMPAD_BUTTON_2)
                result = concat(result, CAFE_GLYPH_WIIMOTE_BTN_2);
            if (arg & BCMPAD_BUTTON_C)
                result = concat(result, CAFE_GLYPH_NUNCHUK_BTN_C);
            if (arg & BCMPAD_BUTTON_Z)
                result = concat(result, CAFE_GLYPH_NUNCHUK_BTN_Z);

            return result;
        }

    }


    std::string
    to_glyph(const combo& c)
    {
        using utils::concat;
        std::string prefix;

        // only show prefix when it's not ALL
        if (c.controllers != BUTTON_COMBO_MODULE_CONTROLLER_ALL) {

            if (c.controllers & BUTTON_COMBO_MODULE_CONTROLLER_VPAD)
                prefix = "(" CAFE_GLYPH_GAMEPAD ") ";
            else if (c.controllers & BUTTON_COMBO_MODULE_CONTROLLER_WPAD)
                prefix = "(" CAFE_GLYPH_WIIMOTE ") ";

        }

        return prefix + to_glyph(c.buttons);
    }

} // namespace wups::button_combo
