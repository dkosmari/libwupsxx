/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
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
    using seconds_item      = numeric_item<std::chrono::seconds>;
    using minutes_item      = numeric_item<std::chrono::minutes>;
    using hours_item        = numeric_item<std::chrono::hours>;


    template<concepts::duration T>
    std::unique_ptr<numeric_item<T>>
    make_item(option<T>& opt,
              const typename numeric_item<T>::specs& options = {})
    {
        return numeric_item<T>::create(opt, options);
    }

} // namespace wups

#endif
