/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_ERROR_HPP
#define WUPSXX_ERROR_HPP

#include <stdexcept>
#include <string>

#include <wups/config.h>

namespace wups {

    struct error : std::runtime_error {

        error(WUPSConfigAPIStatus status, const std::string& msg);

    };

} // namespace wups

#endif
