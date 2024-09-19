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

#include "wupsxx/button_combo.hpp"

#include "wupsxx/cafe_glyphs.h"

#include "utils.hpp"
#include "wpad_status.h"


using std::string;
using std::uint32_t;


namespace wups::utils {
    
    button_combo::button_combo(const string& str)
    {
        auto tokens = split_view(str, "+ \t\n\r");
        if (tokens.empty())
            return;

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

        if (num_vpad)
            parent::operator =(vpad::button_set{tokens});
        else
            parent::operator =(wpad::button_set{tokens});
    }


    bool
    button_combo::contains(VPADButtons btn)
        const noexcept
    {
        auto ptr = get_if<vpad::button_set>(this);
        if (!ptr)
            return false;
        return ptr->contains(btn);
    }
    

    bool
    button_combo::contains(WPADButton btn)
        const noexcept
    {
        auto ptr = get_if<wpad::button_set>(this);
        return ptr && ptr->contains(btn);
    }
    
    
    bool
    button_combo::contains(WPADNunchukButton btn)
        const noexcept
    {
        auto ptr = get_if<wpad::button_set>(this);
        return ptr && ptr->contains(btn);
    }
    

    bool
    button_combo::contains(WPADClassicButton btn)
        const noexcept
    {
        auto ptr = get_if<wpad::button_set>(this);
        return ptr && ptr->contains(btn);
    }

    
    bool
    button_combo::contains(WPADProButton btn)
        const noexcept
    {
        auto ptr = get_if<wpad::button_set>(this);
        return ptr && ptr->contains(btn);
    }
    
    
    string
    to_string(const button_combo& bc)
    {
        auto visitor = utils::overloaded{
            [](std::monostate) -> string { return {}; },
            [](const auto& bs) -> string { return to_string(bs); }
        };        
        return visit(visitor, bc);
    }


    string
    to_glyph(const button_combo& bc, bool prefix)
    {
        auto visitor = utils::overloaded{
            [](std::monostate) -> string { return {}; },
            [prefix](const auto& bs) -> string { return to_glyph(bs, prefix); }
        };
        return visit(visitor, bc);
    }
    
} // namespace wups::utils
