/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

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

} // namespace wups::utils
