/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_VAR_ITEM_HPP
#define WUPSXX_VAR_ITEM_HPP

#include <optional>

#include "item.hpp"


namespace wups::config {

    // Base class for items that map to a variable.

    template<typename T>
    class var_item : public item {

    protected:

        T& variable;
        std::optional<T> previous_value;
        const T default_value;

    public:

        var_item(const std::string& label,
                 T& variable,
                 T default_value) :
            item{label},
            variable(variable),
            default_value{default_value}
        {}


        // save or restore previous value of variable
        virtual
        void
        on_focus_changed()
            override
        {
            if (has_focus()) {
                // gained focus, save the current value
                previous_value = variable;
            } else {
                // lost focus, restore previous_value if it exists
                if (previous_value) {
                    variable = *previous_value;
                    previous_value.reset();
                }
            }
        }


        virtual
        void
        restore()
            override
        {
            variable = default_value;
        }


        // Check if confirming with A, then clear previous_value
        FocusChange
        on_input(const SimplePadData& input)
            override
        {
            if (input.buttons_d & WUPS_CONFIG_BUTTON_A)
                previous_value.reset(); // we're confirming, nothing to restore

            return item::on_input(input);
        }

    };

}



#endif
