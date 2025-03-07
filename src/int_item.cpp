/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/int_item.hpp"

#include "numeric_item_impl.hpp"

namespace wups {

    // Instantiate the template for `int`.
    template class numeric_item<int>;


    std::unique_ptr<wups::int_item>
    make_item(option<int>& opt,
              int fast_increment,
              int slow_increment)
    {
        return int_item::create(opt, fast_increment, slow_increment);
    }

} // namespace wups
