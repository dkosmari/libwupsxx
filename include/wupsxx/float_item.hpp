/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_FLOAT_ITEM_HPP
#define WUPSXX_FLOAT_ITEM_HPP

#include <memory>

#include "numeric_item.hpp"
#include "option.hpp"

namespace wups {

    using float_item = numeric_item<float>;


    std::unique_ptr<float_item>
    make_item(option<float>& opt,
              const float_item::specs& options = {});

} // namespace wups

#endif
