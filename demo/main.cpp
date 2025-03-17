/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2025  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>              // move()
#include <vector>

#include <padscore/wpad.h>
#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

#include <wups.h>

#include <wupsxx/bool_item.hpp>
#include <wupsxx/category.hpp>
#include <wupsxx/color_item.hpp>
#include <wupsxx/duration_items.hpp> // note, plural
#include <wupsxx/file_item.hpp>
#include <wupsxx/init.hpp>
#include <wupsxx/int_item.hpp>
#include <wupsxx/logger.hpp>
#include <wupsxx/notify.hpp>
#include <wupsxx/shortcut_item.hpp>
#include <wupsxx/storage.hpp>
#include <wupsxx/text_item.hpp>


#include "press_counter_item.hpp"
#include "wait_5_seconds_item.hpp"


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
using std::filesystem::path;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::minutes;
using std::chrono::hours;
using std::string;

namespace logger = wups::logger;
namespace notify = wups::notify;
namespace shortcut = wups::shortcut;

// This type has .r, .g, .b, .a members, and a to_string() function.
using wups::color;


// Used to store button shortcut combos.
using shortcut::combo;


namespace cfg {


    WUPSXX_OPTION("Boolean option 1",
                  bool, bool_option_1, true);
    WUPSXX_OPTION("Boolean option 2",
                  bool, bool_option_2, false);

    WUPSXX_OPTION("Foreground color",
                  color, fg_color, color(0xff, 0x40, 0x80));
    WUPSXX_OPTION("Background color",
                  color, bg_color, color(0xaa, 0xbb, 0xcc));

    WUPSXX_OPTION("Duration (ms)",
                  milliseconds, ms_value, 10ms, 0ms, 1000ms);
    WUPSXX_OPTION("Duration (s)",
                  seconds, s_value, 10s, 0s, 1000s);
    WUPSXX_OPTION("Duration (min)",
                  minutes, min_value, 10min, 0min, 1000min);
    WUPSXX_OPTION("Duration (h)",
                  hours, h_value, 10h, 0h, 1000h);

    WUPSXX_OPTION("Integer option 1",
                  int, int_value_1, 5, -100, 100);
    WUPSXX_OPTION("Integer option 2",
                  int, int_value_2, 0, -1000, 1000);

    WUPSXX_OPTION("Some file",
                  path, some_file, "fs:/vol/external01");
    WUPSXX_OPTION("Plugin file",
                  path, plugin_file, "fs:/vol/external01/wiiu/environments/aroma/plugins");

    WUPSXX_OPTION("Shortcut 1",
                  combo, shortcut1,
                  combo::from_wpad_nunchuk(WPAD_BUTTON_DOWN | WPAD_BUTTON_1,
                                           WPAD_NUNCHUK_BUTTON_C));
    WUPSXX_OPTION("Shortcut 2",
                  combo, shortcut2,
                  combo::from_vpad(VPAD_BUTTON_B | VPAD_BUTTON_Y));

    string text = "The quick brown fox jumps over the lazy dog.";


    namespace foo {

        bool enabled;

        namespace bar {

            int value;

        }

    }


    // store pointers to all options in a vector for convenient load/store.
    std::vector<wups::option_base*> all_options{
        &bool_option_1,
        &bool_option_2,
        &fg_color,
        &bg_color,
        &ms_value,
        &s_value,
        &min_value,
        &h_value,
        &int_value_1,
        &int_value_2,
        &some_file,
        &plugin_file,
        &shortcut1,
        &shortcut2,
    };


    void
    load()
    {
        for (auto& opt : all_options)
            try {
                opt->load();
            }
            catch (std::exception& e) {
                logger::printf("Failed to load setting '%s': %s\n",
                               opt->key.data(),
                               e.what());
            }
        // Manually load plain variables.
        cfg::text = *wups::load<std::string>("text");
    }


    void
    save()
    {
        try {
            for (const auto& opt : all_options)
                opt->store();
            // Manually store plain variables.
            wups::store("text", cfg::text);
            wups::save();
        }
        catch (std::exception& e) {
            logger::printf("Failed to save settings: %s\n", e.what());
        }
    }

} // namespace cfg


shortcut::handle shortcut1_handle;
shortcut::handle shortcut2_handle;


void
activate_shortcut2(shortcut::ctr_set triggeredBy,
                   shortcut::handle handle);


void
setup_shortcuts()
{
    using wups::shortcut::create;


    auto shortcut1_callback = [](shortcut::ctr_set,
                                 shortcut::handle)
    {
        logger::printf("activated shortcut1\n");
        notify::info::show("activated shortcut1");
    };

    auto [handle1, conflict1] = create("Shortcut 1",
                                       cfg::shortcut1.value,
                                       std::move(shortcut1_callback));
    shortcut1_handle = handle1;
    if (conflict1) {
        logger::printf("Conflict in shortcut1\n");
    }


    auto [handle2, conflict2] = create("Shortcut 2",
                                       cfg::shortcut2.value,
                                       activate_shortcut2);
    shortcut2_handle = handle2;
    if (conflict2) {
        logger::printf("Conflict in shortcut2\n");
    }

}


void
clear_shortcuts()
{
    shortcut::destroy(shortcut1_handle);
    shortcut::destroy(shortcut2_handle);
}


void
menu_open(wups::category& root)
{
    using wups::make_item;

    // A bool item, strings are true/false
    root.add(make_item(cfg::bool_option_1));

    // Another bool item, strings are ■/□
    root.add(make_item(cfg::bool_option_2, "■", "□"));

    // A color item, only RGB
    root.add(make_item(cfg::fg_color));

    // Another color item, RGBA
    root.add(make_item(cfg::bg_color, true));


    // Some time duration items
    root.add(make_item(cfg::ms_value));
    root.add(make_item(cfg::s_value));
    root.add(make_item(cfg::min_value));
    root.add(make_item(cfg::h_value));


    // An int item
    root.add(make_item(cfg::int_value_1));

    // Another int item, with custom increments
    root.add(make_item(cfg::int_value_2, 100, 10));

    // A text item, max width limited to 30 chars.
    root.add(make_item("Text", cfg::text, 30));

    // Short text, not scrollable, not focusable.
    root.add(make_item("Short Text", "FooBar"));


    // A file item
    root.add(make_item(cfg::some_file));

    // A file item for plugin files: only .wps extensions.
    root.add(make_item(cfg::plugin_file, 30, {".wps"}));


    root.add(make_item(cfg::shortcut1, shortcut1_handle));

    root.add(make_item(cfg::shortcut2, shortcut2_handle));


    root.add(press_counter_item::create());

    root.add(wait_5_seconds_item::create());


    {
        // this tests that wups::item can be safely destroyed manually
        auto dummy = make_item("Dummy", "Nothing");
    }


#if 0
    // TODO: still gotta rework nested keys
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
    logger::set_prefix(PLUGIN_NAME);
    logger::guard guard_;

    notify::initialize(PLUGIN_NAME);
    shortcut::initialize(PLUGIN_NAME);

    try {
        wups::init(PLUGIN_NAME, menu_open, menu_close);
        cfg::load();
        setup_shortcuts();
    }
    catch (std::exception& e) {
        logger::printf("Error initializing: %s\n", e.what());
    }
}


DEINITIALIZE_PLUGIN()
{
    clear_shortcuts();
    notify::finalize();
    shortcut::finalize();
}


ON_APPLICATION_START()
{
    logger::initialize();
}


ON_APPLICATION_ENDS()
{
    logger::finalize();
}


void
activate_shortcut2(shortcut::ctr_set,
                   shortcut::handle)
{
    logger::printf("activated shortcut2\n");
    notify::info::show("activated shortcut2");
}
