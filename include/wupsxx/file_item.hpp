/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_FILE_ITEM_HPP
#define WUPSXX_FILE_ITEM_HPP

#include <filesystem>
#include <memory>
#include <vector>

#include "var_item.hpp"


namespace wups::config {

    class file_item : public var_item<std::filesystem::path> {

        std::size_t max_width;
        std::vector<std::string> extensions;
        std::vector<std::filesystem::directory_entry> entries;
        std::size_t current_idx;
        bool variable_is_dir;

    public:

        file_item(const std::string& label,
                  std::filesystem::path& variable,
                  const std::filesystem::path& default_value,
                  std::size_t max_width = 40,
                  const std::vector<std::string>& extensions = {});

        static
        std::unique_ptr<file_item>
        create(const std::string& label,
               std::filesystem::path& variable,
               const std::filesystem::path& default_value,
               std::size_t max_width = 40,
               const std::vector<std::string>& extensions = {});


        virtual int get_display(char* buf, std::size_t size) const override;

        virtual int get_focused_display(char* buf, std::size_t size) const override;

        virtual void on_focus_changed() override;

        virtual void restore() override;

        virtual FocusChange on_input(const SimplePadData& input) override;


    private:

        void enter_directory(std::filesystem::path dirname,
                             std::filesystem::path filename = "");

        void navigate_prev();
        void navigate_next();
        void navigate_up();
    };

} // namespace wups::config

#endif
