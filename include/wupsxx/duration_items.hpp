/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_DURATION_ITEMS_HPP
#define WUPSXX_DURATION_ITEMS_HPP

#include <chrono>

#include "numeric_item.hpp"


namespace wups::config {

    using milliseconds_item = numeric_item<std::chrono::milliseconds>;

    using seconds_item = numeric_item<std::chrono::seconds>;

    using minutes_item = numeric_item<std::chrono::minutes>;

    using hours_item = numeric_item<std::chrono::hours>;

} // namespace wups::config

#endif
