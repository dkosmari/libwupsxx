// SPDX-License-Identifier: GPL-3.0-or-later

#include "numeric_item_impl.hpp"

namespace wups::config {

    // Here we instantiate numeric_item<> for useful types.

    template class numeric_item<std::chrono::milliseconds>;

    template class numeric_item<std::chrono::seconds>;

    template class numeric_item<std::chrono::minutes>;

    template class numeric_item<std::chrono::hours>;

    template class numeric_item<std::chrono::days>;

} // namespace wups::config
