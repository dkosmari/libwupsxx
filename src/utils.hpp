/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <string_view>
#include <variant>
#include <vector>


namespace wups::utils {

    std::string
    concat(const std::string& a,
           const std::string& b,
           const char* sep = "+");


    std::vector<std::string_view>
    split_view(const std::string& str,
               const std::string& sep = "+ ");


    // return a reference to a variant entry, initialized if necessary
    template<typename T,
             typename... Ts>
    T&
    ensure(std::variant<Ts...>& v)
        noexcept
    {
        if (!holds_alternative<T>(v))
            v = T{};
        return get<T>(v);
    }


    template<typename... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator ()...;
    };

} // namespace wups::utils

#endif
