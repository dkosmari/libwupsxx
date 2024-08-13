/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <chrono>
#include <filesystem>
#include <string>
#include <utility>              // move()

#include <whb/log.h>
#include <whb/log_module.h>

#include <wups.h>

#include <wupsxx/bool_item.hpp>
#include <wupsxx/category.hpp>
#include <wupsxx/color_item.hpp>
#include <wupsxx/duration_items.hpp> // note, plural
#include <wupsxx/file_item.hpp>
#include <wupsxx/int_item.hpp>
#include <wupsxx/storage.hpp>
#include <wupsxx/text_item.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define PLUGIN_NAME "Demo for libwupsxx"


WUPS_PLUGIN_NAME(PLUGIN_NAME);
WUPS_PLUGIN_VERSION(PACKAGE_VERSION);
WUPS_PLUGIN_DESCRIPTION("Show libwupsxx items.");
WUPS_PLUGIN_AUTHOR("Daniel K. O.");
WUPS_PLUGIN_LICENSE("MIT");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("wupsxx-demo"); // store config in wupsxx-demo.json


using namespace std::literals;

// This type has .r, .g, .b, .a members, and a to_string() function.
using wups::config::color;


namespace cfg {

    bool bool_option_1;
    bool bool_option_2;

    color foreground;
    color background;

    std::chrono::milliseconds ms_value;
    std::chrono::seconds s_value;
    std::chrono::minutes min_value;
    std::chrono::hours h_value;

    int int_value_1;
    int int_value_2;

    std::string text;

    std::filesystem::path some_file;

    namespace foo {

        bool enabled;

        namespace bar {

            int value;

        }

    }


#define STORE(x)  wups::storage::store(#x, x)

    void
    save()
    {
        try {
            STORE(bool_option_1);
            STORE(bool_option_2);
            STORE(foreground);
            STORE(background);
            STORE(ms_value);
            STORE(s_value);
            STORE(min_value);
            STORE(h_value);
            STORE(int_value_1);
            STORE(int_value_2);
            STORE(text);
            STORE(some_file);
            // TODO: handle nested elements

            wups::storage::save();
        }
        catch (std::exception& e) {
            WHBLogPrintf("ERROR in %s: %s\n", __PRETTY_FUNCTION__, e.what());
        }
    }

#undef STORE


#define LOAD(x, d) wups::storage::load_or_init(#x, x, d)

    void
    load()
    {
        try {
            LOAD(bool_option_1, true);
            LOAD(bool_option_2, false);
            LOAD(foreground, (color{0xff, 0x40, 0x80}));
            LOAD(background, (color{0xaa, 0xbb, 0xcc, 0xdd}));
            LOAD(ms_value, 10ms);
            LOAD(s_value, 10s);
            LOAD(min_value, 10min);
            LOAD(h_value, 10h);
            LOAD(int_value_1, 5);
            LOAD(int_value_2, 0);
            LOAD(text, "The quick brown fox jumps over the lazy dog.");
            LOAD(some_file, "fs:/vol/external01/wiiu");
        }
        catch (std::exception& e) {
            WHBLogPrintf("ERROR in %s: %s\n", __PRETTY_FUNCTION__, e.what());
        }
    }
#undef LOAD

}


WUPSConfigAPICallbackStatus
menu_open(WUPSConfigCategoryHandle root_handle)
{
    try {
        wups::config::category root{root_handle};

        // A bool item, default=true, strings are true/false
        root.add(wups::config::bool_item::create("Boolean option 1",
                                                 cfg::bool_option_1, true));

        // Another bool item, default=false, strings are ■/□
        root.add(wups::config::bool_item::create("Boolean option 2",
                                                 cfg::bool_option_2, false,
                                                 "■", "□"));


        // A color item, only RGB
        root.add(wups::config::color_item::create("Foreground (RGB)",
                                                  cfg::foreground, color{0xff, 0x40, 0x80}));

        // Another color item, RGBA
        root.add(wups::config::color_item::create("Background (RGBA)",
                                                  cfg::background, color{0xaa, 0xbb, 0xcc, 0xdd},
                                                  true));


        // Some time duration items
        root.add(wups::config::milliseconds_item::create("Duration (ms)",
                                                         cfg::ms_value, 10ms,
                                                         0ms, 1000ms));

        root.add(wups::config::seconds_item::create("Duration (s)",
                                                    cfg::s_value, 10s,
                                                    0s, 1000s));

        root.add(wups::config::minutes_item::create("Duration (min)",
                                                    cfg::min_value, 10min,
                                                    0min, 1000min));

        root.add(wups::config::hours_item::create("Duration (h)",
                                                  cfg::h_value, 10h,
                                                  0h, 1000h));

        // An int item
        root.add(wups::config::int_item::create("Integer option 1",
                                                cfg::int_value_1, 5,
                                                -100, 100));

        // Another int item, with custom increments
        root.add(wups::config::int_item::create("Integer option 2",
                                                cfg::int_value_2, 0,
                                                -1000, 1000,
                                                100, 10));


        // A text item, max width limited to 30 chars.
        root.add(wups::config::text_item::create("Text",
                                                 cfg::text,
                                                 30));


        // A file item
        root.add(wups::config::file_item::create("Some file",
                                                 cfg::some_file,
                                                 "fs:/vol/external01/wiiu"));

#if 0
        {
            // A category named "Foo"
            wups::config::category cat_foo{"Foo"};

            // Add a text item to this foo category
            cat_foo.add(wups::config::text_item::create("This is Foo"));

            // ... and a bool item
            cat_foo.add(wups::config::bool_item::create("Enabled",
                                                        cfg::foo::enabled, false,
                                                        "on", "off"));

            // Nest another category named "Bar"
            {
                wups::config::category cat_bar{"Bar"};
                cat_bar.add(wups::config::text_item::create("This is Bar, inside Foo"));
                cat_bar.add(wups::config::int_item::create("Value",
                                                           cfg::foo::bar::value, -1,
                                                           -10, 10));
                cat_foo.add(std::move(cat_bar));
            }


            // Move it to the root.
            root.add(std::move(cat_foo));
        }
#endif

        return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
    }
    catch (std::exception& e) {
        WHBLogPrintf("menu_open(): %s\n", e.what());
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
    }
}


void
menu_close()
{
    try {
        cfg::save();
    }
    catch (std::exception& e) {
        WHBLogPrintf("menu_close(): %s\n", e.what());
    }
}


INITIALIZE_PLUGIN()
{
    WHBLogModuleInit();

    // This name is shown in the config menu.
    WUPSConfigAPIOptionsV1 options{ .name = PLUGIN_NAME };
    auto status = WUPSConfigAPI_Init(options, menu_open, menu_close);
    if (status != WUPSCONFIG_API_RESULT_SUCCESS) {
        WHBLogPrintf("WUPSConfigAPI_Init() error: %s",
                     WUPSConfigAPI_GetStatusStr(status));
    } else {
        cfg::load();
    }

}


DEINITIALIZE_PLUGIN()
{
    WHBLogModuleDeinit();
}
