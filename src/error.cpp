/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <wups/config_api.h>

#include "wupsxx/error.hpp"


using namespace std::literals;


namespace wups {

    error::error(WUPSConfigAPIStatus status,
                 const std::string& msg) :
        std::runtime_error{msg + ": "s + WUPSConfigAPI_GetStatusStr(status)}
    {}

} // namespace wups
