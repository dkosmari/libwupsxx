/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <cstdio>
#include <locale>
#include <ranges>
#include <utility>              // move()

#include "wupsxx/file_item.hpp"

#include "nintendo_glyphs.h"


namespace wups::config {


    namespace {

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


        bool
        is_sd_root(const std::filesystem::path& p)
        {
            return p == "fs:/vol/external01";
        }


        std::string
        to_utf8(const std::u32string& s)
        {
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
            return conv.to_bytes(s);
        }


        std::u32string
        prefix(const std::u32string& str, std::size_t prefix_size)
        {
            // Note: count argument for substr() can safely exceed the string size.
            return str.substr(0, prefix_size);
        }


        std::u32string
        suffix(const std::u32string& str, std::size_t suffix_size)
        {
            if (suffix_size >= str.size())
                return str;
            // Note: str.substr(str.size()) is safe, returns empty string.
            return str.substr(str.size() - suffix_size, suffix_size);
        }


        std::string
        ellipsize_path(const std::filesystem::path& p,
                       std::size_t max_width)
        {
            std::u32string str = p.u32string();
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
            return to_utf8(str);
        }


    } // namespace


    file_item::file_item(const std::string& label,
                         std::filesystem::path& variable,
                         const std::filesystem::path& default_value,
                         std::size_t max_width) :
        var_item{label, variable, default_value},
        max_width{max_width},
        current_idx{0},
        variable_is_dir{is_directory(variable)}
    {}


    std::unique_ptr<file_item>
    file_item::create(const std::string& label,
                      std::filesystem::path& variable,
                      const std::filesystem::path& default_value,
                      std::size_t max_width)
    {
        return std::make_unique<file_item>(label, variable, default_value, max_width);
    }


    int
    file_item::get_display(char* buf, std::size_t size)
        const
    {
        const char* dir_indicator = "";
        if (variable_is_dir)
            dir_indicator = "/";

        std::string variable_str = ellipsize_path(variable, max_width);

        std::snprintf(buf, size,
                      "%s%s",
                      variable_str.c_str(),
                      dir_indicator);
        return 0;
    }


    int
    file_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        const char* dir_indicator = "";
        if (variable_is_dir)
            dir_indicator = "/";

        const char* up_symbol = "";
        const char* prev_symbol = "";
        const char* next_symbol = "";
        if (!is_sd_root(variable))
            up_symbol = ", " NIN_GLYPH_BTN_L "=" NIN_GLYPH_BACK;
        if (current_idx > 0)
            prev_symbol = NIN_GLYPH_BTN_DPAD_LEFT " ";
        if (current_idx + 1 < entries.size())
            next_symbol = " " NIN_GLYPH_BTN_DPAD_RIGHT;

        std::string variable_str = ellipsize_path(variable, max_width);

        std::snprintf(buf, size,
                      "(" NIN_GLYPH_BTN_A "=enter%s) "
                      "%s" "%s%s" "%s",
                      up_symbol,
                      prev_symbol,
                      variable_str.c_str(),
                      dir_indicator,
                      next_symbol);
        return 0;
    }


    void
    file_item::on_focus_changed()
    {
        if (has_focus())
            enter_directory(variable.parent_path(), variable);
    }


    // Note: when restoring to default value, we must update variable_is_dir.
    void
    file_item::restore()
    {
        var_item::restore();
        variable_is_dir = is_directory(variable);
    }


    FocusChange
    file_item::on_input(const SimplePadData& input)
    {
        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_LEFT))
            navigate_prev();

        if (input.pressed_or_repeated(WUPS_CONFIG_BUTTON_RIGHT))
            navigate_next();

        if (input.buttons_d & WUPS_CONFIG_BUTTON_A)
            enter_directory(variable);

        if (input.buttons_d & WUPS_CONFIG_BUTTON_L)
            navigate_up();

        return var_item::on_input(input);
    }


    void
    file_item::enter_directory(std::filesystem::path dirname,
                               std::filesystem::path filename)
    {
        try {
            // Special handling for "fs:/vol", because it's not counted as a directory.
            if (dirname == "fs:/vol" && is_sd_root(filename)) {
                entries.clear();
                entries.emplace_back(filename);
                current_idx = 0;
                variable = filename;
                variable_is_dir = is_directory(variable);
                return;
            }

            if (!is_directory(dirname))
                return;

            std::vector<std::filesystem::directory_entry> new_entries;
            for (auto& entry : std::filesystem::directory_iterator{dirname})
                new_entries.push_back(entry);

            // Don't enter empty directories, there's nothing to select.
            if (new_entries.empty())
                return;

            current_idx = 0;
            entries = std::move(new_entries);
            std::ranges::sort(entries, icase_compare);

            // find the entry that matches filename
            if (!filename.empty()) {
                for (std::size_t i = 0; i < entries.size(); ++i)
                    if (entries[i].path() == filename) {
                        current_idx = i;
                        break;
                    }
            }

            variable = entries[current_idx];
            variable_is_dir = entries[current_idx].is_directory();

        }
        catch (std::exception& e) {
        }
    }


    void
    file_item::navigate_prev()
    {
        if (current_idx == 0)
            return;
        --current_idx;
        if (!entries.empty()) {
            variable = entries[current_idx];
            variable_is_dir = entries[current_idx].is_directory();
        }
    }


    void
    file_item::navigate_next()
    {
        if (current_idx + 1 >= entries.size())
            return;
        ++current_idx;
        variable = entries[current_idx];
        variable_is_dir = entries[current_idx].is_directory();
    }


    void
    file_item::navigate_up()
    {
        if (!is_sd_root(variable))
            enter_directory(variable.parent_path().parent_path(),
                            variable.parent_path());
    }

} // namespace wups::config
