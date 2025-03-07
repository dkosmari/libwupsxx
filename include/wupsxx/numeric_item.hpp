/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_NUMERIC_ITEM_HPP
#define WUPSXX_NUMERIC_ITEM_HPP

#include <memory>
#include <stdexcept>
#include <type_traits>

#include "var_item.hpp"

#include "concepts.hpp"


namespace wups {


    // partial specialization of `option<T>` for when T is numeric
    template<concepts::numeric T>
    struct option<T> : option_base {

        T value;
        const T default_value;
        const T min_value;
        const T max_value;


        option(const std::string& label,
               const std::string& key,
               T default_value,
               T min_value,
               T max_value) :
            option_base{label, key},
            value{default_value},
            default_value{default_value},
            min_value{min_value},
            max_value{max_value}
        {}


        void
        load()
            override
        {
            using std::to_string;
            using wups::to_string;
            wups::load_or_init(key, value, default_value);
            if (value < min_value || value > max_value)
                throw std::range_error{"Value ("
                                       + to_string(value)
                                       + ") is out of range ["
                                       + to_string(min_value)
                                       + ", "
                                       + to_string(max_value)
                                       + "]"};
        }


        void
        store()
            const override
        {
            wups::store(key, value);
        }

    };


    template<typename T>
    class numeric_item : public var_item<T> {

    protected:

        using var_item<T>::variable;

        T min_value;
        T max_value;
        T fast_increment;
        T slow_increment;

    public:

        numeric_item(option<T>& opt,
                     T fast_increment = T{10},
                     T slow_increment = T{1});

        static
        std::unique_ptr<numeric_item>
        create(option<T>& opt,
               T fast_increment = T{10},
               T slow_increment = T{1});


        virtual void get_display(char* buf, std::size_t size) const override;

        virtual void get_focused_display(char* buf, std::size_t size) const override;

        virtual focus_status on_input(const simple_pad_data& input) override;

    };

} // namespace wups::config

#endif
