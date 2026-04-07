/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_INT_ITEM_HPP
#define WUPSXX_INT_ITEM_HPP

#include <memory>

#include "numeric_item.hpp"
#include "option.hpp"


namespace wups {

    using int_item = numeric_item<int>;


    std::unique_ptr<int_item>
    make_item(option<int>& opt,
              const int_item::specs& options = {});

} // namespace wups

#endif
