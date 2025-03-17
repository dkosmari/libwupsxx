/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_LOGGER_HPP
#define WUPSXX_LOGGER_HPP

#include <cstdarg>


namespace wups::logger {

    void set_prefix(const char* prefix);

    // Every call to initialize() must have a matching finalize()
    void initialize();
    void finalize();


    __attribute__(( __format__ (__printf__, 1, 2) ))
    void printf(const char* fmt, ...) noexcept;

    void vprintf(const char* fmt, std::va_list args) noexcept;


    // Keeps the logger initialized while it's constructed.
    // Use this when generating logs from initialization/finalization hooks.
    struct guard {
        guard();
        ~guard();
    };


} // namespace wups::logger

#endif
