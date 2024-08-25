/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_COLOR_ITEM_HPP
#define WUPSXX_COLOR_ITEM_HPP

#include <memory>

#include "color.hpp"
#include "var_item.hpp"


namespace wups::config {

    class color_item : public var_item<color> {

        bool has_alpha;
        unsigned edit_idx;

    public:

        color_item(const std::string& label,
                   color& variable,
                   color default_value,
                   bool has_alpha = false);

        static
        std::unique_ptr<color_item>
        create(const std::string& label,
               color& variable,
               color default_value,
               bool has_alpha = false);


        virtual int get_display(char* buf, std::size_t size) const override;

        virtual int get_focused_display(char* buf, std::size_t size) const override;

        virtual focus_status on_input(const simple_pad_data& input) override;

    };

} // namespace wups::config


#endif
