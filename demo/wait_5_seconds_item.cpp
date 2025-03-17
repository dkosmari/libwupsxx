/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <chrono>

#include "wait_5_seconds_item.hpp"


using namespace std::literals;


wait_5_seconds_item::wait_5_seconds_item() :
    button_item{"Press to wait 5 seconds"}
{}


std::unique_ptr<wait_5_seconds_item>
wait_5_seconds_item::create()
{
    return std::make_unique<wait_5_seconds_item>();
}


void
wait_5_seconds_item::on_started()
{
    status_msg = "Waiting 5 seconds...";

    // Note: we launch a thread that takes in a std::stop_token
    worker_thread = std::jthread{[this](std::stop_token token)
    {
        using clock = std::chrono::steady_clock;
        auto time_start = clock::now();

        while (clock::now() - time_start < 5s) {
            if (token.stop_requested())
                break;

            std::this_thread::sleep_for(100ms);
        }
        // Note: Always mark the state as finished.
        // Note: button_item::current_state is atomic.
        // Note: Do this on every exit path out of the thread.
        current_state = state::stopped;
    }};
}


void
wait_5_seconds_item::on_finished()
{
    worker_thread.join();
    status_msg = "Finished";
}


void
wait_5_seconds_item::on_cancel()
{
    status_msg = "Canceling...";
    if (worker_thread.joinable())
        worker_thread.request_stop();
}
