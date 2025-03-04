/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/duration_items.hpp"

#include "numeric_item_impl.hpp"

namespace wups {

    // Here we instantiate numeric_item<> for useful types.

    template class numeric_item<std::chrono::milliseconds>;

    template class numeric_item<std::chrono::seconds>;

    template class numeric_item<std::chrono::minutes>;

    template class numeric_item<std::chrono::hours>;

} // namespace wups
