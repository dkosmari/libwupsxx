/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_NUMERIC_ITEM_HPP
#define WUPSXX_NUMERIC_ITEM_HPP

#include <memory>
#include <stdexcept>
#include <type_traits>

#include "var_item.hpp"


namespace wups {

    template<typename T>
    class numeric_item : public var_item<T> {

    public:

        struct specs {

            T fast_increment = T{10};
            T slow_increment = T{1};

        }; // struct specs


        numeric_item(option<T>& opt,
                     const specs& options);

        static
        std::unique_ptr<numeric_item>
        create(option<T>& opt,
               const specs& options);


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

    protected:

        using var_item<T>::variable;

        const T min_value;
        const T max_value;
        const specs options;

    }; // class numeric_item<T>

} // namespace wups::config

#endif
