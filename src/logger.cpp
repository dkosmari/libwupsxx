/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstring>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>

#include <whb/log.h>
#include <whb/log_cafe.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

#include "wupsxx/logger.hpp"


using namespace std::literals;


namespace wups::logger {

    namespace {

        struct log_module {
            log_module()
            {
                if (!WHBLogModuleInit())
                    throw std::runtime_error{"WHBLogModuleInit() failed"};
            }

            ~log_module()
                noexcept
            {
                WHBLogModuleDeinit();
            }
        };

        struct log_cafe {
            log_cafe()
            {
                if (!WHBLogCafeInit())
                    throw std::runtime_error{"WHBLogCafeInit() failed"};
            }

            ~log_cafe()
            {
                WHBLogCafeDeinit();
            }
        };

        struct log_udp {
            log_udp()
            {
                if (!WHBLogUdpInit())
                    throw std::runtime_error{"WHBLogUdpInit() failed"};
            }

            ~log_udp()
            {
                WHBLogUdpDeinit();
            }
        };

        std::mutex mut;

        unsigned refs = 0;

        std::optional<log_module> log_module_guard;
        std::optional<log_cafe>   log_cafe_guard;
        std::optional<log_udp>    log_udp_guard;

        std::string prefix;

    } // namespace


    void
    set_prefix(const char* p)
    {
        std::lock_guard guard{mut};
        if (p && std::strlen(p) > 0)
            prefix = "["s + p + "] "s;
        else
            prefix.clear();
    }


    void
    initialize()
    {
        std::lock_guard guard{mut};
        if (refs == 0) {
            try {
                log_module_guard.emplace();
            }
            catch (...) {
                // only if module fails, we fall back to cafe + udp
                try {
                    log_cafe_guard.emplace();
                }
                catch (...) {
                }
                try {
                    log_udp_guard.emplace();
                }
                catch (...) {
                }
            }

            if (!log_module_guard && !log_cafe_guard && !log_udp_guard)
                return; // fail silently, can't generate logs

        }
        ++refs;
    }


    void
    finalize()
    {
        std::lock_guard guard{mut};
        if (refs == 0)
            return; // fail silently, happens when initialization fails
        if (refs == 1) {
            log_udp_guard.reset();
            log_cafe_guard.reset();
            log_module_guard.reset();
        }
        --refs;
    }


    void
    printf(const char* fmt, ...)
        noexcept
    {
        std::va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }


    void
    vprintf(const char* fmt, std::va_list args)
        noexcept
    {
        try {
            std::lock_guard guard{mut};
            if (refs == 0)
                return;

            std::va_list args_backup;
            va_copy(args_backup, args);
            int sz = std::vsnprintf(nullptr, 0, fmt, args_backup);
            va_end(args_backup);

            if (sz > 0) {
                // make buf large enough for prefix, message, and null terminator
                std::string buf(prefix.size() + sz + 1, '\0');
                buf.replace(0, prefix.size(), prefix);
                std::vsnprintf(buf.data() + prefix.size(),
                               sz + 1,
                               fmt,
                               args);
                WHBLogWrite(buf.data());
            }
        }
        catch (...) {}
    }


    guard::guard()
    {
        initialize();
    }


    guard::~guard()
    {
        finalize();
    }


} // namespace wups::logger
