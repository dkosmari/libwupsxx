/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_BOOL_ITEM_HPP
#define WUPSXX_BOOL_ITEM_HPP

#include <memory>
#include <string>

#include "var_item.hpp"


namespace wups {

    class bool_item : public var_item<bool> {

    public:

        struct specs {

            std::string true_label = "on";
            std::string false_label = "off";

        }; // struct specs


        bool_item(option<bool>& opt,
                  const specs& options);

        static
        std::unique_ptr<bool_item>
        create(option<bool>& opt,
               const specs& options);


        virtual
        void
        get_display(char* buf,
                    std::size_t size)
            const override;

        virtual
        void
        get_focused_display(char* buf,
                            std::size_t size)
            const override;

        virtual
        focus_status
        on_input(const simple_pad_data& input)
            override;

    private:

        const specs options;

    }; // class bool_item


    std::unique_ptr<bool_item>
    make_item(option<bool>& opt,
              const bool_item::specs& options = {});

} // namespace wups

#endif
