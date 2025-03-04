/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/duration.hpp"

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

} // namespace wups
