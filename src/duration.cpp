/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/duration.hpp"

#include "wupsxx/storage.hpp"


using namespace std::chrono;
using std::string;


namespace wups {

    template<>
    string
    to_string<milliseconds>(milliseconds t)
    {
        return std::to_string(t.count()) + "ms";
    }


    template<>
    string
    to_string<seconds>(seconds t)
    {
        return std::to_string(t.count()) + "s";
    }


    template<>
    string
    to_string<minutes>(minutes t)
    {
        return std::to_string(t.count()) + "min";
    }


    template<>
    string
    to_string<hours>(hours t)
    {
        return std::to_string(t.count()) + "h";
    }


    template<>
    string
    to_string<days>(days t)
    {
        return std::to_string(t.count()) + "d";
    }


    template<concepts::duration D>
    bool
    load_suffix(const std::string& key,
                D& value,
                const std::string& suffix)
        noexcept
    try {
        std::string value_str;
        if (!load(key, value_str))
            return false;
        if (value_str.ends_with(suffix))
            value_str.resize(value_str.size() - suffix.size());
        std::size_t parsed = 0;
        value = static_cast<D>(std::stoi(value_str, &parsed));
        if (parsed != value_str.size())
            return false;
        return true;
    }
    catch (...) {
        return false;
    }


    template<>
    bool
    load<milliseconds>(const std::string& key,
                       milliseconds& t)
        noexcept
    {
        return load_suffix(key, t, "ms");
    }


    template<>
    bool
    load<seconds>(const std::string& key,
                  seconds& t)
        noexcept
    {
        return load_suffix(key, t, "s");
    }


    template<>
    bool
    load<minutes>(const std::string& key,
                  minutes& t)
        noexcept
    {
        return load_suffix(key, t, "min");
    }


    template<>
    bool
    load<hours>(const std::string& key,
                hours& t)
        noexcept
    {
        return load_suffix(key, t, "h");
    }


    template<>
    bool
    load<days>(const std::string& key,
               days& t)
        noexcept
    {
        return load_suffix(key, t, "d");
    }



    template<concepts::duration D>
    void
    store(const std::string& key,
          const D& value)
    {
        wups::store(key, to_string(value));
    }


    template
    void
    store<milliseconds>(const std::string& key,
                        const milliseconds& value);

    template
    void
    store<seconds>(const std::string& key,
                   const seconds& value);

    template
    void
    store<minutes>(const std::string& key,
                   const minutes& value);

    template
    void
    store<hours>(const std::string& key,
                 const hours& value);

    template
    void
    store<days>(const std::string& key,
                const days& value);

} // namespace wups
