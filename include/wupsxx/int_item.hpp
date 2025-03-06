/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_INT_ITEM_HPP
#define WUPSXX_INT_ITEM_HPP

#include <concepts>
#include <memory>

#include "numeric_item.hpp"


namespace wups {

    using int_item = numeric_item<int>;


    std::unique_ptr<int_item>
    make_item(option<int>& opt,
              int min_value,
              int max_value,
              int fast_increment = 10,
              int slow_increment = 1);

} // namespace wups

#endif
