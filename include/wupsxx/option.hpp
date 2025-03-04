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

        const std::string key;
        const std::string label;

        option_base(const std::string& key,
                    const std::string& label) :
            key{key},
            label{label}
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

        const T default_value;
        T value;

        option(const std::string& key,
               const T& default_value,
               const std::string& label) :
            option_base{key, label},
            default_value{default_value},
            value{default_value}
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


#define WUPSXX_OPTION(type, name, default_value, label)         \
    wups::option<type> name{#name, default_value, label}


#endif
