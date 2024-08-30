/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_NUMERIC_ITEM_HPP
#define WUPSXX_NUMERIC_ITEM_HPP

#include <memory>

#include "var_item.hpp"


namespace wups::config {


    template<typename T>
    class numeric_item : public var_item<T> {

    protected:

        using var_item<T>::variable;

        T min_value;
        T max_value;
        T fast_increment;
        T slow_increment;

    public:

        numeric_item(const std::string& label,
                     T& variable, T default_value,
                     T min_value, T max_value,
                     T fast_increment = T{10},
                     T slow_increment = T{1});

        static
        std::unique_ptr<numeric_item>
        create(const std::string& label,
               T& variable, T default_value,
               T min_value, T max_value,
               T fast_increment = T{10},
               T slow_increment = T{1});


        virtual void get_display(char* buf, std::size_t size) const override;

        virtual void get_focused_display(char* buf, std::size_t size) const override;

        virtual focus_status on_input(const simple_pad_data& input) override;

    };

} // namespace wups::config

#endif
