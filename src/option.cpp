/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include "wupsxx/option.hpp"


namespace wups {

    option_base::option_base(const std::string& label,
                             const std::string& key) :
        label{label},
        key{key}
    {}


    option_base::~option_base() noexcept = default;

} // namespace wups
