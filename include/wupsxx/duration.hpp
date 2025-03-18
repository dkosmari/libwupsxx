/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_DURATION_HPP
#define WUPSXX_DURATION_HPP

#include <chrono>
#include <string>

#include "concepts.hpp"


namespace wups {

    // make wups::utils::duration<> an alias to std::chrono::duration<>
    template<typename R, typename P>
    using duration = std::chrono::duration<R, P>;


    template<concepts::duration D>
    std::string
    to_string(D d);

} // namespace wups

#endif
