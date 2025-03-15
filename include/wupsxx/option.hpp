/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_OPTION_HPP
#define WUPSXX_OPTION_HPP

#include <string>
#include <stdexcept>

#include "concepts.hpp"
#include "storage.hpp"


namespace wups {

    struct option_base {

        const std::string label;
        const std::string key;

        option_base(const std::string& label,
                    const std::string& key) :
            label{label},
            key{key}
        {}


        // disallow moving
        option_base(option_base&&) = delete;


        virtual void load() = 0;
        virtual void store() const = 0;


    protected:

        // Disallow destroying from base pointer.
        ~option_base() noexcept = default;

    };


    template<typename T>
    struct option : option_base {

        T value;
        const T default_value;

        option(const std::string& label,
               const std::string& key,
               const T& default_value) :
            option_base{label, key},
            value{default_value},
            default_value{default_value}
        {}


        void
        load()
            override
        {
            wups::load_or_init(key, value, default_value);
        }


        void
        store()
            const override
        {
            wups::store(key, value);
        }

    };


    // Partial specialization of `option<T>` for when T is numeric.
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

    }; // option<T> specialization for numeric T

} // namespace wups


#define WUPSXX_OPTION(label, type, name, default_value, ...)            \
    wups::option<type> name{label, #name, default_value, __VA_ARGS__}


#endif
