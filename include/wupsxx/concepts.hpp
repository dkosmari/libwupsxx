/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_CONCEPTS_HPP
#define WUPSXX_CONCEPTS_HPP

#include <chrono>
#include <type_traits>


namespace wups::concepts {

    namespace detail {

        template<typename T>
        struct is_duration : std::false_type {};

        template<typename R, typename P>
        struct is_duration<std::chrono::duration<R, P>> : std::true_type {};

        template<typename T>
        constexpr inline bool is_duration_v = is_duration<T>::value;

    } // namespace detail

    template<typename T>
    concept duration = detail::is_duration_v<T>;


    namespace detail {

        template<typename T>
        struct is_numeric : std::false_type {};

        // explicitly make `bool` not numeric.
        template<>
        struct is_numeric<bool> : std::false_type {};

        template<typename T>
        requires(std::is_arithmetic_v<T>)
        struct is_numeric<T> : std::true_type {};

        template<typename T>
        requires(is_duration_v<T>)
        struct is_numeric<T> : std::true_type {};

        template<typename T>
        constexpr inline bool is_numeric_v = is_numeric<T>::value;

    } // namespace detail

    template<typename T>
    concept numeric = detail::is_numeric_v<T>;

} // namespace wups::concepts

#endif
