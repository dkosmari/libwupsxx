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
#include "option.hpp"


namespace wups {

    // Base class for items that map to a variable.

    template<typename T>
    class var_item : public item {

    protected:

        T& variable;
        std::optional<T> old_value;
        const T default_value;


        var_item(option<T>& opt) :
            item{opt.label},
            variable(opt.value),
            default_value{opt.default_value}
        {}


        virtual
        bool
        on_focus_request(bool /*new_focus*/)
            const override
        {
            return true;
        }


        virtual
        void
        on_focus_changed()
            override
        {
            if (has_focus())
                old_value = variable;
            else
                old_value.reset();
        }


        void
        confirm_change()
        {
            old_value.reset();
        }


        void
        cancel_change()
        {
            if (old_value)
                variable = *old_value;
            old_value.reset();
        }


        virtual
        void
        restore_default()
            override
        {
            variable = default_value;
            confirm_change();
        }


        // Handle confirm/cancel with A/B buttons.
        focus_status
        on_input(const simple_pad_data& input)
            override
        {
            if (input.buttons_d & WUPS_CONFIG_BUTTON_X) {
                this->restore_default();
                return focus_status::lose;
            }

            if (input.buttons_d & WUPS_CONFIG_BUTTON_B) {
                this->cancel_change();
                return focus_status::lose;
            }

            if (input.buttons_d & WUPS_CONFIG_BUTTON_A) {
                this->confirm_change();
                return focus_status::lose;
            }

            return focus_status::keep;
        }


        // don't hide item::on_input
        using item::on_input;

    };

} // namespace wups


#endif
