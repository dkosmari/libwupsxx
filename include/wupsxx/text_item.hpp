/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_TEXT_ITEM_HPP
#define WUPSXX_TEXT_ITEM_HPP

#include <memory>

#include "item.hpp"


namespace wups::config {

    // Note: this class doesn't do much on its own, so it's all public.

    struct text_item : item {

        std::string text;
        std::size_t max_width;
        std::size_t first = 0; // first visible character

        text_item(const std::string& label,
                  const std::string& text = "",
                  std::size_t max_width = 50);

        static
        std::unique_ptr<text_item>
        create(const std::string& label,
               const std::string& text = "",
               std::size_t max_width = 50);


        virtual int get_display(char* buf, std::size_t size) const override;

        virtual int get_focused_display(char* buf, std::size_t size) const override;

        virtual bool on_focus_request(bool new_focus) const override;

        virtual focus_status on_input(const simple_pad_data& input) override;

    };

} // namespace wups::config

#endif
