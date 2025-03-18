/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_STORAGE_HPP
#define WUPSXX_STORAGE_HPP

#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

#include <wups/storage.h>

#include "concepts.hpp"


namespace wups {


    struct storage_error : std::runtime_error {
        WUPSStorageError code;

        storage_error(WUPSStorageError status, const std::string& msg);
    };

    // Note: these are undefined templates.
    // Make sure to define it for all types.

    template<typename T>
    bool
    load(const std::string& key, T& value)
        noexcept;


    template<typename T>
    void
    store(const std::string& key,
          const T& value);


    void save();

    void reload();

} // namespace wups

#endif
