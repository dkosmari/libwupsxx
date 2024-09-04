/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_INIT_HPP
#define WUPSXX_INIT_HPP

#include <functional>


#include <wups/config_api.h>

#include "category.hpp"


namespace wups::config {

    // Throw std::runtime_error if initialization fails.
    void
    init(const char* name,
         std::function<void(category& root)> open_callback,
         std::function<void()> close_callback);

} // namespace wups::config

#endif
