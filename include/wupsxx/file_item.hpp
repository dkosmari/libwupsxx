/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
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

        std::size_t max_width;
        std::vector<std::string> extensions;
        std::vector<std::filesystem::directory_entry> entries;
        std::size_t current_idx;
        bool variable_is_dir;

    public:

        file_item(option<std::filesystem::path>& opt,
                  std::size_t max_width = 40,
                  const std::vector<std::string>& extensions = {});

        static
        std::unique_ptr<file_item>
        create(option<std::filesystem::path>& opt,
               std::size_t max_width = 40,
               const std::vector<std::string>& extensions = {});


        virtual void get_display(char* buf, std::size_t size) const override;

        virtual void get_focused_display(char* buf, std::size_t size) const override;

        virtual void on_focus_changed() override;

        virtual void restore_default() override;

        virtual focus_status on_input(const simple_pad_data& input) override;


    private:

        void enter_directory(std::filesystem::path dirname,
                             std::filesystem::path filename = "");

        void navigate_prev();
        void navigate_next();
        void navigate_up();
    };

} // namespace wups

#endif
