/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>              // move()

#include <padscore/wpad.h>
#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

#include <wups.h>
#include <notifications/notifications.h>

#include <wupsxx/bool_item.hpp>
#include <wupsxx/button_combo_item.hpp>
#include <wupsxx/category.hpp>
#include <wupsxx/color_item.hpp>
#include <wupsxx/duration_items.hpp> // note, plural
#include <wupsxx/file_item.hpp>
#include <wupsxx/init.hpp>
#include <wupsxx/int_item.hpp>
#include <wupsxx/logger.hpp>
#include <wupsxx/storage.hpp>
#include <wupsxx/text_item.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define PLUGIN_NAME "Demo for libwupsxx"
#define PLUGIN_FILE_NAME PACKAGE_TARNAME "-demo"

WUPS_PLUGIN_NAME(PLUGIN_NAME);
WUPS_PLUGIN_VERSION(PACKAGE_VERSION);
WUPS_PLUGIN_DESCRIPTION("Show libwupsxx config items.");
WUPS_PLUGIN_AUTHOR("Daniel K. O.");
WUPS_PLUGIN_LICENSE("MIT");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE(PLUGIN_FILE_NAME); // store config in libwupsxx-demo.json


using namespace std::literals;

namespace logger = wups::logger;


// This type has .r, .g, .b, .a members, and a to_string() function.
using wups::config::color;


// Used to store button combo shortcuts.
using wups::config::button_combo;


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
    std::filesystem::path plugin_file;


    button_combo shortcut1;
    button_combo shortcut2;


    namespace foo {

        bool enabled;

        namespace bar {

            int value;

        }

    }



    void
    save()
    {
        try {
#define STORE(x)  wups::storage::store(#x, x)
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
            STORE(plugin_file);
            STORE(shortcut1);
            STORE(shortcut2);
            // TODO: handle nested elements
#undef STORE
            wups::storage::save();
        }
        catch (std::exception& e) {
            logger::printf("exception caught: %s\n", e.what());
        }
    }


    void
    load()
    {
#define LOAD(x, d) wups::storage::load_or_init(#x, x, d)
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
        LOAD(some_file, "fs:/vol/external01");
        LOAD(plugin_file, "fs:/vol/external01/wiiu/environments/aroma/plugins");
        LOAD(shortcut1, (wups::config::button_combo{}));
        LOAD(shortcut2, (wups::config::vpad_buttons{VPAD_BUTTON_B | VPAD_BUTTON_Y}));
#undef LOAD
    }

}


void
menu_open(wups::config::category& root)
{
    using namespace wups::config;

    // A bool item, default=true, strings are true/false
    root.add(bool_item::create("Boolean option 1",
                               cfg::bool_option_1, true));

    // Another bool item, default=false, strings are ■/□
    root.add(bool_item::create("Boolean option 2",
                               cfg::bool_option_2, false,
                               "■", "□"));


    // A color item, only RGB
    root.add(color_item::create("Foreground",
                                cfg::foreground, color{0xff, 0x40, 0x80}));

    // Another color item, RGBA
    root.add(color_item::create("Background",
                                cfg::background, color{0xaa, 0xbb, 0xcc, 0xdd},
                                true));


    // Some time duration items
    root.add(milliseconds_item::create("Duration (ms)",
                                       cfg::ms_value, 10ms,
                                       0ms, 1000ms));

    root.add(seconds_item::create("Duration (s)",
                                  cfg::s_value, 10s,
                                  0s, 1000s));

    root.add(minutes_item::create("Duration (min)",
                                  cfg::min_value, 10min,
                                  0min, 1000min));

    root.add(hours_item::create("Duration (h)",
                                cfg::h_value, 10h,
                                0h, 1000h));

    // An int item
    root.add(int_item::create("Integer option 1",
                              cfg::int_value_1, 5,
                              -100, 100));

    // Another int item, with custom increments
    root.add(int_item::create("Integer option 2",
                              cfg::int_value_2, 0,
                              -1000, 1000,
                              100, 10));


    // A text item, max width limited to 30 chars.
    root.add(text_item::create("Text",
                               cfg::text,
                               30));


    // A file item
    root.add(file_item::create("Some file",
                               cfg::some_file,
                               "fs:/vol/external01"));

    // A file item for plugin files: only .wps extensions.
    root.add(file_item::create("Plugin file",
                               cfg::plugin_file,
                               "fs:/vol/external01/wiiu/environments/aroma/plugins",
                               30,
                               {".wps"}));


    root.add(button_combo_item::create("Shortcut1",
                                       cfg::shortcut1));

    root.add(button_combo_item::create("Shortcut2",
                                       cfg::shortcut2,
                                       vpad_buttons{VPAD_BUTTON_B | VPAD_BUTTON_Y}));


#if 0
    {
        // A category named "Foo"
        config::category cat_foo{"Foo"};

        // Add a text item to this foo category
        cat_foo.add(text_item::create("This is Foo"));

        // ... and a bool item
        cat_foo.add(bool_item::create("Enabled",
                                      cfg::foo::enabled, false,
                                      "on", "off"));

        // Nest another category named "Bar"
        {
            category cat_bar{"Bar"};
            cat_bar.add(text_item::create("This is Bar, inside Foo"));
            cat_bar.add(int_item::create("Value",
                                         cfg::foo::bar::value, -1,
                                         -10, 10));
            cat_foo.add(std::move(cat_bar));
        }


        // Move it to the root.
        root.add(std::move(cat_foo));
    }
#endif

}


void
menu_close()
{
    cfg::save();
}


INITIALIZE_PLUGIN()
{
    wups::logger::guard guard_{PLUGIN_NAME};

    NotificationModule_InitLibrary();

    try {
        wups::config::init(PLUGIN_NAME, menu_open, menu_close);
        cfg::load();
    }
    catch (std::exception& e) {
        logger::printf("Error initializing: %s\n", e.what());
    }
}


DEINITIALIZE_PLUGIN()
{
    NotificationModule_DeInitLibrary();
}


ON_APPLICATION_START()
{
    logger::initialize(PLUGIN_NAME);
}


ON_APPLICATION_ENDS()
{
    logger::finalize();
}


void
activate_shortcut1()
{
    logger::printf("activated shortcut1\n");
    NotificationModule_AddInfoNotification("activated shortcut1");
}


void
activate_shortcut2()
{
    logger::printf("activated shortcut2\n");
    NotificationModule_AddInfoNotification("activated shortcut2");
}


DECL_FUNCTION(int32_t,
              VPADRead,
              VPADChan channel,
              VPADStatus* status,
              uint32_t count,
              VPADReadError* error)
{
    auto result = real_VPADRead(channel, status, count, error);
    if (result <= 0)
        return result;

    // Note: when proc mode is loose, all button samples are identical to the most recent
    const int32_t num_samples = VPADGetButtonProcMode(channel) ? result : 1;
    for (int32_t idx = num_samples - 1; idx >= 0; --idx) {
        if (wups::config::vpad_update(channel, status[idx])) {
            if (wups::config::vpad_triggered(channel, cfg::shortcut1))
                activate_shortcut1();
            if (wups::config::vpad_triggered(channel, cfg::shortcut2))
                activate_shortcut2();
        }
    }

    return result;
}

WUPS_MUST_REPLACE(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead);


DECL_FUNCTION(void,
              WPADRead,
              WPADChan channel,
              WPADStatus* status)
{
    real_WPADRead(channel, status);
    if (wups::config::wpad_update(channel, status)) {
        if (wups::config::wpad_triggered(channel, cfg::shortcut1))
            activate_shortcut1();
        if (wups::config::wpad_triggered(channel, cfg::shortcut2))
            activate_shortcut2();
    }
}

WUPS_MUST_REPLACE(WPADRead, WUPS_LOADER_LIBRARY_PADSCORE, WPADRead);
