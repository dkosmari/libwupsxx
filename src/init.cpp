/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <utility>
#include <stdexcept>

#include "wupsxx/init.hpp"

#include "wupsxx/logger.hpp"


namespace wups::config {

    namespace {

        std::function<void(category&)> open_callback;
        std::function<void()> close_callback;


        WUPSConfigAPICallbackStatus
        menu_open(WUPSConfigCategoryHandle root_handle)
            noexcept
        {
            try {
                if (!open_callback)
                    return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

                category root{root_handle};
                open_callback(root);

                return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
            }
            catch (std::exception& e) {
                logger::printf("Error in menu open callback: %s\n",
                               e.what());
                return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
            }
            catch (...) {
                logger::printf("Unknown error in menu open callback\n");
                return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
            }
        }


        void
        menu_close()
            noexcept
        {
            try {
                if (close_callback)
                    close_callback();
            }
            catch (std::exception& e) {
                logger::printf("Error in menu close callback: %s\n",
                               e.what());
            }
            catch (...) {
                logger::printf("Unknown error in menu close callback\n");
            }
        }

    } // namespace


    void
    init(const char* name,
         std::function<void(category& root)> open_cb,
         std::function<void()> close_cb)
    {
        open_callback = std::move(open_cb);
        close_callback = std::move(close_cb);
        WUPSConfigAPIOptionsV1 options{ .name = name };
        auto status = WUPSConfigAPI_Init(options,
                                         menu_open,
                                         menu_close);
        if (status != WUPSCONFIG_API_RESULT_SUCCESS)
            throw std::runtime_error{WUPSConfigAPI_GetStatusStr(status)};
    }


} // namespace wups
