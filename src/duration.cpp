// SPDX-License-Identifier: MIT

#include "wupsxx/duration.hpp"


namespace wups::config {

    template<>
    std::string
    to_string<std::chrono::milliseconds>(std::chrono::milliseconds t)
    {
        return std::to_string(t.count()) + "ms";
    }


    template<>
    std::string
    to_string<std::chrono::seconds>(std::chrono::seconds t)
    {
        return std::to_string(t.count()) + "s";
    }


    template<>
    std::string
    to_string<std::chrono::minutes>(std::chrono::minutes t)
    {
        return std::to_string(t.count()) + "min";
    }


    template<>
    std::string
    to_string<std::chrono::hours>(std::chrono::hours t)
    {
        return std::to_string(t.count()) + "h";
    }


    template<>
    std::string
    to_string<std::chrono::days>(std::chrono::days t)
    {
        return std::to_string(t.count()) + "d";
    }

}
