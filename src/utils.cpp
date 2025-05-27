/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdexcept>

#include "utils.hpp"


using std::string;


namespace wups::utils {

    string
    concat(const string& a,
           const string& b,
           const char* sep)
    {
        if (a.empty())
            return b;
        if (b.empty())
            return a;
        return a + sep + b;
    }


    std::vector<std::string_view>
    split_view(const string& str,
               const string& sep)
    {
        std::vector<std::string_view> result;
        using size_type = string::size_type;

        size_type begin_pos = 0;
        size_type end_pos = 0;

        for (;;) {

            begin_pos = str.find_first_not_of(sep, end_pos);

            // if string ends in separators, stop
            if (begin_pos == string::npos)
                break;

            end_pos = str.find_first_of(sep, begin_pos);

            // if no more separators, append remaining of str and stop
            if (end_pos == string::npos) {
                result.emplace_back(str.data() + begin_pos,
                                    str.data() + str.size());
                break;
            }

            result.emplace_back(str.data() + begin_pos,
                                str.data() + end_pos);

        }

        return result;
    }


    std::string
    to_utf8(const std::u32string& input)
    {
        std::string output;

        constexpr char8_t cont_mask   = 0b0011'1111;
        constexpr char8_t cont_prefix = 0b1000'0000;

        for (char32_t c : input) {
            if (!c) // null terminator, stop early
                break;
            if (c < 0x0080) {
                output.push_back(c);
            } else if (c < 0x0800) {
                output.push_back(0b1100'0000 | (c >> 6));
                output.push_back(cont_prefix | (c & cont_mask));
            } else if (c < 0x010000) {
                output.push_back(0b1110'0000 |  (c >> 12));
                output.push_back(cont_prefix | ((c >>  6) & cont_mask));
                output.push_back(cont_prefix | ((c >>  0) & cont_mask));
            } else if (c < 0x110000) {
                output.push_back(0b1111'0000 |  (c >> 18));
                output.push_back(cont_prefix | ((c >> 12) & cont_mask));
                output.push_back(cont_prefix | ((c >>  6) & cont_mask));
                output.push_back(cont_prefix | ((c >>  0) & cont_mask));
            } else {
                throw std::runtime_error{"invalid UTF-32 input"};
            }
        }

        return output;
    }

} // namespace wups::utils
