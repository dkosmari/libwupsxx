/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <string>

#include "press_counter_item.hpp"


press_counter_item::press_counter_item() :
    button_item{"A button counter"}
{}


std::unique_ptr<press_counter_item>
press_counter_item::create()
{
    return std::make_unique<press_counter_item>();
}


void
press_counter_item::on_started()
{
    ++counter;
    status_msg = "Pressed " + std::to_string(counter) + " times";
    // Note: we immediately set to the finished state.
    current_state = state::stopped;
}
