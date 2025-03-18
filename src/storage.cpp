/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <wups/storage.h>

#include "wupsxx/storage.hpp"


using namespace std::literals;


namespace wups {

    storage_error::storage_error(WUPSStorageError status, const std::string& msg) :
        std::runtime_error{msg + ": "s + std::string(WUPSStorageAPI::GetStatusStr(status))},
        code{status}
    {}


    template<typename T>
    bool
    load(const std::string& key, T& value)
        noexcept
    {
        auto status = WUPSStorageAPI::Get(key, value);
        if (status)
            return false;
        return true;
    }

    // Explicit instantiation for all types supported by WUPSStorageAPI::Get()
    template bool load<bool>(const std::string& key, bool& value);
    template bool load<int32_t>(const std::string& key, int32_t& value);
    template bool load<int64_t>(const std::string& key, int64_t& value);
    template bool load<uint32_t>(const std::string& key, uint32_t& value);
    template bool load<uint64_t>(const std::string& key, uint64_t& value);
    template bool load<float>(const std::string& key, float& value);
    template bool load<double>(const std::string& key, double& value);


    // std::string has special instantiation, to allow resizing
    template<>
    bool
    load<std::string>(const std::string& key,
                      std::string& value)
        noexcept
    {
        try {
            auto status = WUPSStorageAPI::Get(key, value,
                                              WUPSStorageAPI::GetOptions::RESIZE_EXISTING_BUFFER);
            if (status)
                return false;
            return true;
        }
        catch (...) {
            return false;
        }
    }


    template<>
    bool
    load<std::filesystem::path>(const std::string& key,
                                std::filesystem::path& value)
        noexcept
    try {
        std::string value_str;
        if (!load(key, value_str))
            return false;
        value = value_str;
        return true;
    }
    catch (...) {
        return false;
    }


    template<typename T>
    void
    store(const std::string& key,
          const T& value)
    {
        auto status = WUPSStorageAPI::Store(key, value);
        if (status)
            throw storage_error{status, "error storing key \"" + key + "\""};
    }


    // Explicit instantiation for all types supported by WUPSStorageAPI::Store()
    template void store<bool>(const std::string& key, const bool& value);
    template void store<int32_t>(const std::string& key, const int32_t& value);
    template void store<int64_t>(const std::string& key, const int64_t& value);
    template void store<uint32_t>(const std::string& key, const uint32_t& value);
    template void store<uint64_t>(const std::string& key, const uint64_t& value);
    template void store<float>(const std::string& key, const float& value);
    template void store<double>(const std::string& key, const double& value);
    template void store<std::string>(const std::string& key, const std::string& value);


    template<>
    void
    store<std::filesystem::path>(const std::string& key,
                                 const std::filesystem::path& p)
    {
        store(key, p.string());
    }


    void
    save()
    {
        auto status = WUPSStorageAPI::SaveStorage();
        if (status)
            throw storage_error{status, "error saving storage"};
    }


    void
    reload()
    {
        auto status = WUPSStorageAPI::ForceReloadStorage();
        if (status)
            throw storage_error{status, "error reloading storage"};
    }

} // namespace wups
