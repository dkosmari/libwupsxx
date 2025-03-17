/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <cstring>
#include <string>

#include <notifications/notifications.h>

#include "wupsxx/notify.hpp"


using namespace std::literals;


namespace wups::notify {

    std::string prefix;


    namespace {

        constexpr
        NMColor
        convert(const color& c)
            noexcept
        {
            return { c.r, c.g, c.b, c.a };
        }

    }


    lib_error::lib_error(error_code code) :
        std::runtime_error{NotificationModule_GetStatusStr(code)},
        code{code}
    {}


    void
    initialize(const char* p)
    {
        if (p && std::strlen(p) > 0)
            prefix = "["s + p + "] "s;

        auto e = NotificationModule_InitLibrary();
        if (e)
            throw lib_error{e};
    }


    void
    finalize()
        noexcept
    {
        NotificationModule_DeInitLibrary();
    }


    namespace info {

        float duration = 2.0f;


        void
        set_text_color(const color& c)
        {
            auto e = NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
                                                        NOTIFICATION_MODULE_DEFAULT_OPTION_TEXT_COLOR,
                                                        convert(c));
            if (e)
                throw lib_error{e};
        }


        void
        set_text_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            set_text_color(color{r, g, b, a});
        }


        void
        set_bg_color(const color& c)
        {
            auto e = NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
                                                        NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
                                                        convert(c));
            if (e)
                throw lib_error{e};
        }


        void
        set_bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            return set_bg_color({r, g, b, a});
        }


        void
        set_duration(std::chrono::milliseconds t)
        {
            duration = t.count() / 1000.0f;
            auto e = NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
                                                        NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT,
                                                        duration);
            if (e)
                throw lib_error{e};
        }


        __attribute__(( __format__ (__printf__, 1, 2)))
        void
        show(const char* fmt, ...)
        {
            std::va_list args;
            va_start(args, fmt);
            vshow(fmt, args);
            va_end(args);
        }


        __attribute__(( __format__ (__printf__, 3, 4)))
        void
        show(const color& text_color,
             const color& bg_color,
             const char* fmt, ...)
        {
            std::va_list args;
            va_start(args, fmt);
            try {
                vshow(text_color, bg_color, fmt, args);
                va_end(args);
            }
            catch (...) {
                va_end(args);
                throw;
            }
        }


        void
        vshow(const char* fmt, std::va_list args)
        {
            std::va_list args2;
            va_copy(args2, args);
            int sz = std::vsnprintf(nullptr, 0, fmt, args2);
            va_end(args2);

            if (sz > 0) {
                // Note: std::string always one extra position to store '\0'
                std::string buf(sz, '\0');
                std::vsnprintf(buf.data(),
                               sz + 1,
                               fmt, args);
                auto e = NotificationModule_AddInfoNotification((prefix + buf).data());
                if (e)
                    throw lib_error{e};
            }
        }


        void
        vshow(const color& text_color,
              const color& bg_color,
              const char* fmt, std::va_list args)
        {
            std::va_list args2;
            va_copy(args2, args);
            int sz = std::vsnprintf(nullptr, 0, fmt, args2);
            va_end(args2);

            if (sz > 0) {
                // Note: std::string always one extra position to store '\0'
                std::string buf(sz, '\0');
                std::vsnprintf(buf.data(),
                               sz + 1,
                               fmt, args);
                auto e = NotificationModule_AddInfoNotificationEx((prefix + buf).data(),
                                                                  duration,
                                                                  convert(text_color),
                                                                  convert(bg_color),
                                                                  nullptr,
                                                                  nullptr,
                                                                  false);
                if (e)
                    throw lib_error{e};
            }
        }

    } // namespace info


    namespace error {

        float duration = 2.0f;


        void
        set_text_color(const color& c)
        {
            auto e = NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR,
                                                        NOTIFICATION_MODULE_DEFAULT_OPTION_TEXT_COLOR,
                                                        convert(c));
            if (e)
                throw lib_error{e};
        }


        void
        set_text_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            set_text_color(color{r, g, b, a});
        }


        void
        set_bg_color(const color& c)
        {
            auto e = NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR,
                                                        NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
                                                        convert(c));
            if (e)
                throw lib_error{e};
        }


        void
        set_bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            return set_bg_color({r, g, b, a});
        }


        void
        set_duration(std::chrono::milliseconds t)
        {
            duration = t.count() / 1000.0f;
            auto e = NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR,
                                                        NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT,
                                                        duration);
            if (e)
                throw lib_error{e};
        }


        __attribute__(( __format__ (__printf__, 1, 2)))
        void
        show(const char* fmt, ...)
        {
            std::va_list args;
            va_start(args, fmt);
            vshow(fmt, args);
            va_end(args);
        }


        void
        vshow(const char* fmt, std::va_list args)
        {
            std::va_list args2;
            va_copy(args2, args);
            int sz = std::vsnprintf(nullptr, 0, fmt, args2);
            va_end(args2);

            if (sz > 0) {
                // Note: std::string always one extra position to store '\0'
                std::string buf(sz, '\0');
                std::vsnprintf(buf.data(),
                               sz + 1,
                               fmt, args);
                auto e = NotificationModule_AddErrorNotification((prefix + buf).data());
                if (e)
                    throw lib_error{e};
            }
        }

    } // namespace error



} // namespace wups::notify
