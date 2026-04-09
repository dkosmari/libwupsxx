/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025-2026  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <cstdio>
#include <ranges>
#include <set>
#include <stdexcept>
#include <utility>              // move()

#include "wupsxx/file_item.hpp"

#include "wupsxx/cafe_glyphs.h"

#include "utils.hpp"

#include "wupsxx/logger.hpp" // DEBUG


using namespace std::literals;


namespace wups {

    namespace {

        char
        upper_char(char c)
        {
            if (c >= 'a' && c <= 'z')
                return c - ('a' - 'A');
            return c;
        }


        std::string
        upper(const std::string& s)
        {
            std::string result = s;
            for (auto& c : result)
                c = upper_char(c);
            return result;
        }


        char32_t
        upper_uchar(char32_t c)
        {
            if (c >= U'a' && c <= U'z')
                return c - (U'a' - U'A');
            return c;
        }


        bool
        icase_compare(const std::filesystem::directory_entry& a,
                      const std::filesystem::directory_entry& b)
        {
            const auto& uni_a = a.path().u32string();
            const auto& uni_b = b.path().u32string();
            for (auto [ca, cb] : std::views::zip(uni_a, uni_b)) {
                char32_t up_ca = upper_uchar(ca);
                char32_t up_cb = upper_uchar(cb);
                if (up_ca < up_cb)
                    return true;
                if (up_ca > up_cb)
                    return false;
            }
            if (uni_a.size() < uni_b.size())
                return true;
            return false;
        }


        std::u32string
        trim_prefix(const std::u32string& str,
                    std::size_t prefix_size)
        {
            if (prefix_size >= str.size())
                return {};
            return str.substr(prefix_size);
        }


        std::u32string
        trim_suffix(const std::u32string& str,
                    std::size_t suffix_size)
        {
            if (suffix_size >= str.size())
                return {};
            return str.substr(0, str.size() - suffix_size);
        }


        std::string
        format_filename(const std::filesystem::path& input,
                        std::size_t max_width,
                        bool show_full_path,
                        bool append_slash)
        {
            if (input.empty())
                return {};

            const std::u32string ellipsis = U"…";
            const std::u32string separator = U"/";

            std::u32string filename_str = input.filename().u32string();
            if (append_slash)
                filename_str += separator;

            std::u32string parent_path_str;

            if (show_full_path) {
                parent_path_str = input.parent_path().u32string() + separator;

                // Replace "fs:/vol/external01/" by "sd:/"
                const std::u32string sd_prefix = U"fs:/vol/external01";
                if (parent_path_str.starts_with(sd_prefix))
                    parent_path_str.replace(0, sd_prefix.size(), U"sd:");

                auto total_width = parent_path_str.size() + filename_str.size();

                if (total_width > max_width) {
                    // NOTE: make room for "…"
                    auto excess_width = total_width - max_width + ellipsis.size();
                    // trim from beginning of parent_path_str
                    parent_path_str = ellipsis + trim_prefix(parent_path_str, excess_width);
                    if (parent_path_str.size() <= ellipsis.size() + separator.size())
                        parent_path_str = ellipsis + separator;
                }
            }

            auto total_width = parent_path_str.size() + filename_str.size();

            // check if we need to trim filename_str
            if (total_width > max_width) {
                // NOTE: make room for "…"
                auto excess_width = total_width - max_width + ellipsis.size();
                // trim from the end of filename_str
                auto new_filename_str = trim_suffix(filename_str, excess_width) + ellipsis;
                if (new_filename_str.size() > ellipsis.size())
                    filename_str = new_filename_str; // only if there's something other than "…"
            }

            return utils::to_utf8(parent_path_str + filename_str);
        }

    } // namespace


    file_item::file_item(option<std::filesystem::path>& opt,
                         const std::filesystem::path& base_dir,
                         const specs& options_) :
        var_item{opt},
        base_dir{base_dir},
        options{options_},
        current_idx{0},
        current_is_dir{false}
    {
        // Convert all extensions to upper-case.
        for (auto& ext : options.extensions)
            for (char& c : ext)
                c = upper_char(c);
        // and sort them.
        std::ranges::sort(options.extensions);

        if (!variable.empty() && exists(variable))
            current_is_dir = is_directory(variable);
    }


    std::unique_ptr<file_item>
    file_item::create(option<std::filesystem::path>& opt,
                      const std::filesystem::path& base_dir,
                      const specs& options)
    {
        return std::make_unique<file_item>(opt, base_dir, options);
    }


    void
    file_item::get_display(char* buf,
                           std::size_t size)
        const
    {
        std::string variable_str = format_filename(variable,
                                                   options.max_width,
                                                   options.show_full_path,
                                                   options.show_dir_slash && current_is_dir);
        std::snprintf(buf, size, "%s", variable_str.data());
    }


    void
    file_item::get_focused_display(char* buf,
                                   std::size_t size)
        const
    {
        std::string left, right;
        const char* const blank = CAFE_GLYPH_BTN_DPAD;

        bool has_prev = current_idx > 0;
        bool has_next = current_idx + 1 < entries.size();

        left = has_prev ? CAFE_GLYPH_BTN_LEFT : blank;
        right = has_next ? CAFE_GLYPH_BTN_RIGHT : blank;

        std::string variable_str = format_filename(variable,
                                                   options.max_width,
                                                   options.show_full_path,
                                                   options.show_dir_slash && current_is_dir);

        std::snprintf(buf, size,
                      "%s " "%s" " %s",
                      left.data(),
                      variable_str.data(),
                      right.data());
    }


    void
    file_item::on_focus_changed()
    {
        var_item::on_focus_changed();
        if (has_focus())
            read_directory();
        else
            entries.clear();
    }


    void
    file_item::restore_default()
    {
        var_item::restore_default();
    }


    focus_status
    file_item::on_input(const simple_pad_data& input)
    {
        if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_LEFT))
            navigate_prev();

        if (input.pressed_or_long_held(WUPS_CONFIG_BUTTON_RIGHT))
            navigate_next();

        return var_item::on_input(input);
    }


    void
    file_item::read_directory()
        noexcept
    try {
        current_idx = 0;
        entries.clear();

        if (!exists(base_dir) || !is_directory(base_dir))
            throw std::runtime_error{base_dir.string() + " not found or not directory"};

        for (auto& entry : std::filesystem::directory_iterator{base_dir}) {
            // filter out wrong entry types
            switch (options.valid) {
                case type::regular:
                    if (!entry.is_regular_file())
                        continue;
                    break;
                case type::directory:
                    if (!entry.is_directory())
                        continue;
                    break;
                default:
                        ;
            }

            if (!options.extensions.empty()) {
                // Apply filtering by extension.
                std::string ext = upper(entry.path().extension().string());
                if (std::ranges::binary_search(options.extensions, ext))
                    entries.push_back(entry);
            } else
                entries.push_back(entry);
        }
        // Add an empty entry.
        entries.push_back({});
        std::ranges::sort(entries, icase_compare);

        // find the entry that matches the current value, and set current_idx
        for (std::size_t i = 0; i < entries.size(); ++i)
            if (entries[i].path() == variable) {
                current_idx = i;
                break;
            }
    }
    catch (std::exception& e) {
        logger::printf("ERROR in file_item::read_directory(): %s\n", e.what());
    }


    void
    file_item::navigate_prev()
    {
        if (entries.empty())
            return;
        if (current_idx == 0)
            return;
        auto& current_entry = entries[--current_idx];
        variable = current_entry.path();
        if (!variable.empty())
            current_is_dir = current_entry.is_directory();
        else
            current_is_dir = false;
    }


    void
    file_item::navigate_next()
    {
        if (entries.empty())
            return;
        if (current_idx + 1 >= entries.size())
            return;
        auto& current_entry = entries[++current_idx];
        variable = current_entry.path();
        if (!variable.empty())
            current_is_dir = current_entry.is_directory();
        else
            current_is_dir = false;
    }


    std::unique_ptr<file_item>
    make_item(option<std::filesystem::path>& opt,
              const std::filesystem::path& base_dir,
              const file_item::specs& options)
    {
        return file_item::create(opt, base_dir, options);
    }

} // namespace wups
