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

} // namespace wups


#define WUPSXX_OPTION(label, type, name, default_value, ...)            \
    wups::option<type> name{label, #name, default_value, __VA_ARGS__}


#endif
