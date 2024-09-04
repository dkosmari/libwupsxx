/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstring>
#include <mutex>
#include <string>

#include <whb/log.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

#include "wupsxx/logger.hpp"


using namespace std::literals;


namespace wups::logger {

    std::mutex mut;

    unsigned refs = 0;
    bool initialized_module = false;
    bool initialized_udp = false;

    std::string prefix;


    void
    initialize(const char* p)
    {
        std::lock_guard guard{mut};
        if (refs == 0) {
            initialized_module = WHBLogModuleInit();
            if (!initialized_module)
                initialized_udp = WHBLogUdpInit();

            if (!initialized_module && !initialized_udp)
                return; // fail silently, can't generate logs

            if (p && std::strlen(p) > 0)
                prefix = "["s + p + "] "s;
            else
                prefix.clear();
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
            if (initialized_module) {
                WHBLogModuleDeinit();
                initialized_module = false;
            }
            if (initialized_udp) {
                WHBLogUdpDeinit();
                initialized_udp = false;
            }
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
                WHBLogWrite(buf.c_str());
            }
        }
        catch (...) {}
    }


    guard::guard(const char* p)
    {
        initialize(p);
    }


    guard::~guard()
    {
        finalize();
    }


} // namespace wups::logger
