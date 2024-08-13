/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <cstdio>
#include <ranges>
#include <utility>              // move()

#include <whb/log.h> // DEBUG

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
        wut_is_root(const std::filesystem::path& p)
        {
            // TODO: it seems fs:/vol is not a valid directory, what's up with that?
            return p == "fs:/vol/external01";
        }

    } // namespace


    file_item::file_item(const std::string& label,
                         std::filesystem::path& variable,
                         const std::filesystem::path& default_value) :
        var_item{label, variable, default_value},
        current_idx{0}
    {}


    std::unique_ptr<file_item>
    file_item::create(const std::string& label,
                      std::filesystem::path& variable,
                      const std::filesystem::path& default_value)
    {
        return std::make_unique<file_item>(label, variable, default_value);
    }


    int
    file_item::get_display(char* buf, std::size_t size)
        const
    {
        const char* dir_indicator = "";
        if (is_directory(variable))
            dir_indicator = "/";

        std::snprintf(buf, size,
                      "%s%s",
                      variable.c_str(),
                      dir_indicator);
        return 0;
    }


    int
    file_item::get_focused_display(char* buf, std::size_t size)
        const
    {
        const char* dir_indicator = "";
        if (is_directory(variable))
            dir_indicator = "/";

        const char* up_symbol = "";
        const char* prev_symbol = "";
        const char* next_symbol = "";
        if (!wut_is_root(variable.parent_path()))
            up_symbol = "(" NIN_GLYPH_BTN_L " = " NIN_GLYPH_BACK ") " ;
        if (current_idx > 0)
            prev_symbol = NIN_GLYPH_BTN_DPAD_LEFT " ";
        if (current_idx + 1 < entries.size())
            next_symbol = " " NIN_GLYPH_BTN_DPAD_RIGHT;

        std::snprintf(buf, size,
                      "%s%s" "%s%s" "%s",
                      up_symbol,
                      prev_symbol,
                      variable.c_str(),
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
            // WHBLogPrintf("%s: dirname = \"%s\", filename = \"%s\"\n",
            //              __PRETTY_FUNCTION__,
            //              dirname.c_str(), filename.c_str());
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
        if (!entries.empty())
            variable = entries[current_idx];
    }


    void
    file_item::navigate_next()
    {
        if (current_idx + 1 >= entries.size())
            return;
        ++current_idx;
        variable = entries[current_idx];
    }


    void
    file_item::navigate_up()
    {
        enter_directory(variable.parent_path().parent_path(),
                        variable.parent_path());
    }

} // namespace wups::config
