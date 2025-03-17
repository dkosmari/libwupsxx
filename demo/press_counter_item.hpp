/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <wupsxx/button_item.hpp>

// Example of a button item that blocks when activated, and finishes immediately.

struct press_counter_item : wups::button_item {

    unsigned counter = 0;

    press_counter_item();

    static
    std::unique_ptr<press_counter_item>
    create();

    void
    on_started()
        override;

};
