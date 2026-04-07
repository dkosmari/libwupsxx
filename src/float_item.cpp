/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/float_item.hpp"

#include "numeric_item_impl.hpp"


namespace wups {

    // Instantiate the template for float
    template class numeric_item<float>;


    std::unique_ptr<float_item>
    make_item(option<float>& opt,
              const float_item::specs& options)
    {
        return float_item::create(opt, options);
    }

} // namespace wups
