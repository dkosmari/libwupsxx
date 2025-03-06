/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_DURATION_ITEMS_HPP
#define WUPSXX_DURATION_ITEMS_HPP

#include <chrono>

#include "numeric_item.hpp"
#include "duration.hpp"

namespace wups {

    using milliseconds_item = numeric_item<std::chrono::milliseconds>;

    using seconds_item = numeric_item<std::chrono::seconds>;

    using minutes_item = numeric_item<std::chrono::minutes>;

    using hours_item = numeric_item<std::chrono::hours>;


    template<concepts::duration T>
    std::unique_ptr<numeric_item<T>>
    make_item(option<T>& opt,
              T min_value,
              T max_value,
              T fast_increment = T{10},
              T slow_increment = T{1})
    {
        return numeric_item<T>::create(opt,
                                       min_value, max_value,
                                       fast_increment, slow_increment);
    }

} // namespace wups

#endif
