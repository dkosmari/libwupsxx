/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_FILE_ITEM_HPP
#define WUPSXX_FILE_ITEM_HPP

#include <filesystem>
#include <memory>
#include <vector>

#include "var_item.hpp"


namespace wups {

    class file_item : public var_item<std::filesystem::path> {

    public:

        enum class type {
            any,
            regular,
            directory,
        };

        struct specs {

            std::vector<std::string> extensions = {};
            type        valid          = type::any;
            bool        show_full_path = false;
            std::size_t max_width      = 40;

        }; // struct specs


        file_item(option<std::filesystem::path>& opt,
                  const std::filesystem::path& base_dir,
                  const specs& options);

        static
        std::unique_ptr<file_item>
        create(option<std::filesystem::path>& opt,
               const std::filesystem::path& base_dir,
               const specs& options);


        virtual
        void
        get_display(char* buf, std::size_t size)
            const override;

        virtual
        void
        get_focused_display(char* buf, std::size_t size)
            const override;

        virtual
        void
        on_focus_changed()
            override;

        virtual
        void
        restore_default()
            override;

        virtual
        focus_status
        on_input(const simple_pad_data& input)
            override;

    private:

        void
        read_directory()
            noexcept;

        void
        navigate_prev();

        void
        navigate_next();

    private:

        const std::filesystem::path base_dir;
        specs options;
        std::vector<std::filesystem::directory_entry> entries;
        std::size_t current_idx;

    }; // class file_item


    std::unique_ptr<file_item>
    make_item(option<std::filesystem::path>& opt,
              const std::filesystem::path& base_dir,
              const file_item::specs& options = {});

} // namespace wups

#endif
