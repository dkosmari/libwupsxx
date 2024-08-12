/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_VAR_ITEM_HPP
#define WUPSXX_VAR_ITEM_HPP

#include "item.hpp"


namespace wups::config {

    // Base class for items that map to a variable.

    template<typename T>
    class var_item : public item {

    protected:

        T& variable;
        const T default_value;

    public:

        var_item(const std::string& label,
                 T& variable,
                 T default_value) :
            item{label},
            variable(variable),
            default_value{default_value}
        {}


        virtual
        void
        restore()
            override
        {
            variable = default_value;
        }

    };

}



#endif
