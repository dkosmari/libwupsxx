/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_INT_ITEM_HPP
#define WUPSXX_INT_ITEM_HPP

#include <memory>

#include "numeric_item.hpp"


namespace wups::config {

    using int_item = numeric_item<int>;

} // namespace wups::config

#endif
