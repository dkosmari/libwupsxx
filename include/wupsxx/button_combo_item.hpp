/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_BUTTON_COMBO_ITEM_HPP
#define WUPSXX_BUTTON_COMBO_ITEM_HPP

#include <memory>

#include "button_combo.hpp"

#include "var_item.hpp"


namespace wups::config {


    class button_combo_item : public var_item<button_combo::combo> {

        enum class state_t {
            waiting,
            reading,
            confirming
        };

        state_t state;

        button_combo::handle combo_handle{};
        ButtonComboModule_CallbackOptions old_callback{};

        std::string message;

    public:

        button_combo_item(const std::string& label,
                          button_combo::handle combo_handle,
                          button_combo::combo& variable,
                          const button_combo::combo& default_value = {});

        virtual ~button_combo_item();

        static
        std::unique_ptr<button_combo_item>
        create(const std::string& label,
               button_combo::handle combo_handle,
               button_combo::combo& variable,
               const button_combo::combo& default_value = {});


        virtual void get_display(char* buf, std::size_t size) const override;

        virtual void get_focused_display(char* buf, std::size_t size) const override;

        virtual void on_focus_changed() override;

        virtual focus_status on_input(const simple_pad_data& input) override;
        virtual focus_status on_input(const complex_pad_data& input) override;


    };


} // namespace wups::config


#endif
