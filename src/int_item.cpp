/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/int_item.hpp"

#include "numeric_item_impl.hpp"


namespace wups {

    // Instantiate the template for `int`.
    template class numeric_item<int>;


    std::unique_ptr<int_item>
    make_item(option<int>& opt,
              const int_item::specs& options)
    {
        return int_item::create(opt, options);
    }

} // namespace wups
