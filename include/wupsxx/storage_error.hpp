/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_STORAGE_ERROR_HPP
#define WUPSXX_STORAGE_ERROR_HPP

#include <stdexcept>
#include <string>

#include <wups/storage.h>


namespace wups::storage {

    struct storage_error : std::runtime_error {

        WUPSStorageError code;

        storage_error(const std::string& msg, WUPSStorageError status);

    };

} // namespace wups::storage

#endif
