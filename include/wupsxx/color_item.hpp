/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_COLOR_ITEM_HPP
#define WUPSXX_COLOR_ITEM_HPP

#include <memory>

#include "color.hpp"
#include "var_item.hpp"


namespace wups {

    class color_item : public var_item<color> {

        enum class mode_t {
            rgb,
            hex
        };

        bool has_alpha;
        mode_t mode;
        unsigned edit_idx;

    public:

        color_item(option<color>& opt,
                   bool has_alpha = false);

        static
        std::unique_ptr<color_item>
        create(option<color>& opt,
               bool has_alpha = false);


        virtual
        void
        get_display(char* buf, std::size_t size)
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

    };

    std::unique_ptr<color_item>
    make_item(option<color>& opt,
              bool has_alpha = false);

} // namespace wups

#endif
