/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>

#include <wupsxx/button_item.hpp>


// An example of a button that does something in a background thread.

struct wait_5_seconds_item : wups::button_item {

    std::jthread worker_thread;

    wait_5_seconds_item();

    static
    std::unique_ptr<wait_5_seconds_item>
    create();

    void
    on_started()
        override;

    void
    on_finished()
        override;

    void
    on_cancel()
        override;

};
