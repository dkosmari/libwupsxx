/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_NOTIFY_HPP
#define WUPSXX_NOTIFY_HPP

/*
 * This is a wrapper for libnotifications
 */

#include <chrono>
#include <cstdarg>
#include <stdexcept>

#include <notifications/notification_defines.h>

#include "color.hpp"


namespace wups::notify {


    using error_code = NotificationModuleStatus;


    struct lib_error : std::runtime_error {
        error_code code;

        lib_error(error_code code);
    };


    void
    initialize(const char* prefix = nullptr);

    void
    finalize() noexcept;


    namespace info {

        void set_text_color(const color& c);
        void set_text_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);

        void set_bg_color(const color& c);
        void set_bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);

        void set_duration(std::chrono::milliseconds t);


        __attribute__(( __format__ (__printf__, 1, 2)))
        void show(const char* fmt, ...);

        __attribute__(( __format__ (__printf__, 3, 4)))
        void show(const color& text_color,
                  const color& bg_color,
                  const char* fmt, ...);


        void vshow(const char* fmt, std::va_list args);

        void vshow(const color& text_color,
                   const color& bg_color,
                   const char* fmt, std::va_list args);

    } // namespace info


    namespace error {

        void set_text_color(const color& c);
        void set_text_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);

        void set_bg_color(const color& c);
        void set_bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);

        void set_duration(std::chrono::milliseconds t);


        __attribute__(( __format__ (__printf__, 1, 2)))
        void show(const char* fmt, ...);

        void vshow(const char* fmt, std::va_list args);

    } // namespace error


} // namespace wups::notify

#endif
