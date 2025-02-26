/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <wups/storage.h>

#include "wupsxx/storage.hpp"


namespace wups::storage {


    template<>
    std::expected<utils::color, storage_error>
    load<utils::color>(const std::string& key)
    {
        auto res = load<std::string>(key);
        if (!res)
            return std::unexpected{res.error()};
        return utils::color{*res};
    }


    template<>
    std::expected<std::filesystem::path, storage_error>
    load<std::filesystem::path>(const std::string& key)
    {
        auto res = load<std::string>(key);
        if (!res)
            return std::unexpected{res.error()};
        return std::filesystem::path{*res};
    }


    template<>
    std::expected<button_combo::combo, storage_error>
    load<button_combo::combo>(const std::string& key)
    {
        auto res = load<std::string>(key);
        if (!res)
            return std::unexpected{res.error()};
        return button_combo::combo{*res};
    }


    void
    store(const std::string& key, const utils::color& c)
    {
        store<std::string>(key, to_string(c));
    }


    void
    store(const std::string& key, const std::filesystem::path& p)
    {
        store<std::string>(key, p);
    }


    void
    store(const std::string& key, const button_combo::combo& c)
    {
        store<std::string>(key, to_string(c));
    }


    void
    save()
    {
        auto status = WUPSStorageAPI::SaveStorage();
        if (status != WUPS_STORAGE_ERROR_SUCCESS)
            throw storage_error{"error saving storage", status};
    }


    void
    reload()
    {
        auto status = WUPSStorageAPI::ForceReloadStorage();
        if (status != WUPS_STORAGE_ERROR_SUCCESS)
            throw storage_error{"error reloading storage", status};
    }


} // namespace wups::storage
