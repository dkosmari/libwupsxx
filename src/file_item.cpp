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
        prefix(const std::u32string& str,
               std::size_t prefix_size)
        {
            // Note: count argument for substr() can safely exceed the string size.
            return str.substr(0, prefix_size);
        }


        std::u32string
        suffix(const std::u32string& str,
               std::size_t suffix_size)
        {
            if (suffix_size >= str.size())
                return str;
            // Note: str.substr(str.size()) is safe, returns empty string.
            return str.substr(str.size() - suffix_size, suffix_size);
        }


        std::string
        format_filename(const std::filesystem::path& p,
                        std::size_t max_width,
                        bool show_full_path)
        {
            std::u32string str = show_full_path
                                 ? p.u32string()
                                 : p.filename().u32string();

            if (show_full_path) {
                // Replace "fs:/vol/external01/" by "sd:/"
                const std::u32string pre = U"fs:/vol/external01";
                if (str.starts_with(pre))
                    str.replace(0, pre.size(), U"SD:");
            }

            if (str.size() > max_width) {
                const std::u32string ellipsis = U"…";
                const std::u32string separator = U"/";
                auto file_name = p.filename().u32string();
                auto parent_name = p.parent_path().u32string();
                if (file_name.size() + ellipsis.size() + separator.size()
                    > max_width) {
                    // Can't even show "…/file_name", so we ellipsize the file too, show
                    // "…/file_na…".
                    parent_name = ellipsis;
                    std::size_t remaining = max_width
                        - parent_name.size()
                        - separator.size()
                        - ellipsis.size();
                    file_name = prefix(file_name, remaining) + ellipsis;
                    str = parent_name + separator + file_name;
                } else {
                    // show "…end_of_path/file_name"
                    std::size_t remaining = max_width
                        - ellipsis.size()
                        - separator.size()
                        - file_name.size();
                    parent_name = U"…" + suffix(parent_name, remaining);
                    str = parent_name + separator + file_name;
                }
            }
            return utils::to_utf8(str);
        }

    } // namespace


    file_item::file_item(option<std::filesystem::path>& opt,
                         const std::filesystem::path& base_dir,
                         const specs& options_) :
        var_item{opt},
        base_dir{base_dir},
        options{options_},
        current_idx{0}
    {
        // Convert all extensions to upper-case.
        for (auto& ext : options.extensions)
            for (char& c : ext)
                c = upper_char(c);
        // and sort them.
        std::ranges::sort(options.extensions);
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
                                                   options.show_full_path);
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
                                                   options.show_full_path);

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
        variable = entries[--current_idx];
    }


    void
    file_item::navigate_next()
    {
        if (entries.empty())
            return;
        if (current_idx + 1 >= entries.size())
            return;
        variable = entries[++current_idx];
    }


    std::unique_ptr<file_item>
    make_item(option<std::filesystem::path>& opt,
              const std::filesystem::path& base_dir,
              const file_item::specs& options)
    {
        return file_item::create(opt, base_dir, options);
    }

} // namespace wups
