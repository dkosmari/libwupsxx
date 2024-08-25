/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_BOOL_ITEM_HPP
#define WUPSXX_BOOL_ITEM_HPP

#include <memory>

#include "var_item.hpp"


namespace wups::config {

    class bool_item : public var_item<bool> {

        std::string true_str;
        std::string false_str;

    public:

        bool_item(const std::string& label,
                  bool& variable,
                  bool default_value,
                  const std::string& true_str = "true",
                  const std::string& false_str = "false");

        static
        std::unique_ptr<bool_item>
        create(const std::string& label,
               bool& variable,
               bool default_value,
               const std::string& true_str = "true",
               const std::string& false_str = "false");


        virtual int get_display(char* buf, std::size_t size) const override;

        virtual int get_focused_display(char* buf, std::size_t size) const override;

        virtual focus_status on_input(const simple_pad_data& input) override;

    };

} // namespace wups::config

#endif
