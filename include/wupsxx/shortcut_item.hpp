/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_SHORTCUT_ITEM_HPP
#define WUPSXX_SHORTCUT_ITEM_HPP

#include <memory>

#include "shortcut.hpp"

#include "var_item.hpp"


namespace wups {

    class shortcut_item : public var_item<shortcut::combo> {

        enum class state_t {
            waiting,
            reading,
            confirming
        };

        state_t state;

        shortcut::handle combo_handle{};
        ButtonComboModule_CallbackOptions old_callback{};

        std::string message;

    public:

        shortcut_item(option<shortcut::combo>& opt,
                      shortcut::handle combo_handle_);

        virtual ~shortcut_item();

        static
        std::unique_ptr<shortcut_item>
        create(option<shortcut::combo>& opt,
               shortcut::handle combo_handle);


        virtual void get_display(char* buf, std::size_t size) const override;

        virtual void get_focused_display(char* buf, std::size_t size) const override;

        virtual void on_focus_changed() override;

        virtual focus_status on_input(const simple_pad_data& input) override;
        virtual focus_status on_input(const complex_pad_data& input) override;


    };


    std::unique_ptr<shortcut_item>
    make_item(option<shortcut::combo>& opt,
              shortcut::handle combo_handle);

} // namespace wups

#endif
