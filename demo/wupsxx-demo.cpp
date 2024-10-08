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
#include <thread>
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
#include <wupsxx/button_item.hpp>
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
using std::filesystem::path;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::minutes;
using std::chrono::hours;
using std::string;

namespace logger = wups::logger;


// This type has .r, .g, .b, .a members, and a to_string() function.
using wups::utils::color;


// Used to store button combo shortcuts.
using wups::utils::button_combo;

namespace vpad = wups::utils::vpad;
namespace wpad = wups::utils::wpad;


namespace cfg {

    namespace defaults {

        bool bool_option_1 = true;
        bool bool_option_2 = false;

        color foreground = {0xff, 0x40, 0x80};
        color background = {0xaa, 0xbb, 0xcc, 0xdd};

        milliseconds ms_value = 10ms;
        seconds s_value = 10s;
        minutes min_value = 10min;
        hours h_value = 10h;

        int int_value_1 = 5;
        int int_value_2 = 0;

        string text = "The quick brown fox jumps over the lazy dog.";

        path some_file = "fs:/vol/external01";
        path plugin_file = "fs:/vol/external01/wiiu/environments/aroma/plugins";

        button_combo shortcut1 = wpad::button_set{{WPAD_BUTTON_DOWN, WPAD_BUTTON_1},
                                                  {WPAD_NUNCHUK_BUTTON_C}};
        button_combo shortcut2 = vpad::button_set{VPAD_BUTTON_B, VPAD_BUTTON_Y};

    } // namespace cfg::defaults


    bool bool_option_1 = defaults::bool_option_1;
    bool bool_option_2 = defaults::bool_option_2;

    color foreground = defaults::foreground;
    color background = defaults::background;

    milliseconds ms_value  = defaults::ms_value;
    seconds      s_value   = defaults::s_value;
    minutes      min_value = defaults::min_value;
    hours        h_value   = defaults::h_value;

    int int_value_1 = defaults::int_value_1;
    int int_value_2 = defaults::int_value_2;

    string text = defaults::text;

    path some_file   = defaults::some_file;
    path plugin_file = defaults::plugin_file;

    button_combo shortcut1 = defaults::shortcut1;
    button_combo shortcut2 = defaults::shortcut2;


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
#define LOAD(x) wups::storage::load_or_init(#x, x, defaults::x)
        LOAD(bool_option_1);
        LOAD(bool_option_2);
        LOAD(foreground);
        LOAD(background);
        LOAD(ms_value);
        LOAD(s_value);
        LOAD(min_value);
        LOAD(h_value);
        LOAD(int_value_1);
        LOAD(int_value_2);
        LOAD(text);
        LOAD(some_file);
        LOAD(plugin_file);
        LOAD(shortcut1);
        LOAD(shortcut2);
#undef LOAD
    }

}



// Example of a button item that blocks when activated, and finishes immediately.

struct press_counter_item : wups::config::button_item {

    unsigned counter = 0;

    press_counter_item() :
        button_item{"A button counter"}
    {}


    static
    std::unique_ptr<press_counter_item>
    create()
    {
        return std::make_unique<press_counter_item>();
    }


    void
    on_started()
        override
    {
        ++counter;
        status_msg = "Pressed " + std::to_string(counter) + " times";
        // Note: we immediately set to the finished state.
        current_state = state::stopped;
    }

};


// An example of a button that does something in a background thread.

struct wait_5_seconds_item : wups::config::button_item {

    std::jthread worker_thread;


    wait_5_seconds_item() :
        button_item{"Press to wait 5 seconds"}
    {}


    static
    std::unique_ptr<wait_5_seconds_item>
    create()
    {
        return std::make_unique<wait_5_seconds_item>();
    }


    void
    on_started()
        override
    {
        status_msg = "Waiting 5 seconds...";

        // Note: we launch a thread that takes in a std::stop_token
        worker_thread = std::jthread{[this](std::stop_token token)
        {
            using clock = std::chrono::steady_clock;
            auto time_start = clock::now();

            while (clock::now() - time_start < 5s) {
                if (token.stop_requested())
                    break;

                std::this_thread::sleep_for(100ms);
            }
            // Note: Always mark the state as finished.
            // Note: button_item::current_state is atomic.
            // Note: Do this on every exit path out of the thread.
            current_state = state::stopped;
        }};
    }


    void
    on_finished()
        override
    {
        worker_thread.join();
        status_msg = "Finished";
    }


    void
    on_cancel()
        override
    {
        status_msg = "Canceling...";
        if (worker_thread.joinable())
            worker_thread.request_stop();
    }

};


void
menu_open(wups::config::category& root)
{
    using namespace wups::config;

    // A bool item, default=true, strings are true/false
    root.add(bool_item::create("Boolean option 1",
                               cfg::bool_option_1,
                               cfg::defaults::bool_option_1));

    // Another bool item, default=false, strings are ■/□
    root.add(bool_item::create("Boolean option 2",
                               cfg::bool_option_2,
                               cfg::defaults::bool_option_2,
                               "■", "□"));


    // A color item, only RGB
    root.add(color_item::create("Foreground",
                                cfg::foreground,
                                cfg::defaults::foreground));

    // Another color item, RGBA
    root.add(color_item::create("Background",
                                cfg::background,
                                cfg::defaults::background,
                                true));


    // Some time duration items
    root.add(milliseconds_item::create("Duration (ms)",
                                       cfg::ms_value,
                                       cfg::defaults::ms_value,
                                       0ms, 1000ms));

    root.add(seconds_item::create("Duration (s)",
                                  cfg::s_value,
                                  cfg::defaults::s_value,
                                  0s, 1000s));

    root.add(minutes_item::create("Duration (min)",
                                  cfg::min_value,
                                  cfg::defaults::min_value,
                                  0min, 1000min));

    root.add(hours_item::create("Duration (h)",
                                cfg::h_value,
                                cfg::defaults::h_value,
                                0h, 1000h));

    // An int item
    root.add(int_item::create("Integer option 1",
                              cfg::int_value_1,
                              cfg::defaults::int_value_1,
                              -100, 100));

    // Another int item, with custom increments
    root.add(int_item::create("Integer option 2",
                              cfg::int_value_2,
                              cfg::defaults::int_value_2,
                              -1000, 1000,
                              100, 10));


    // A text item, max width limited to 30 chars.
    root.add(text_item::create("Text",
                               cfg::text,
                               30));

    // Short text, not scrollable, not focusable.
    root.add(text_item::create("Short Text",
                               "FooBar"));


    // A file item
    root.add(file_item::create("Some file",
                               cfg::some_file,
                               cfg::defaults::some_file));

    // A file item for plugin files: only .wps extensions.
    root.add(file_item::create("Plugin file",
                               cfg::plugin_file,
                               cfg::defaults::plugin_file,
                               30,
                               {".wps"}));


    root.add(button_combo_item::create("Shortcut1",
                                       cfg::shortcut1,
                                       cfg::defaults::shortcut1));

    root.add(button_combo_item::create("Shortcut2",
                                       cfg::shortcut2,
                                       cfg::defaults::shortcut2));


    root.add(press_counter_item::create());

    root.add(wait_5_seconds_item::create());


    {
        // this tests that wups::item can be safely destroyed manually
        auto dummy = text_item::create("Dummy", "Nothing");
    }


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
        if (wups::utils::vpad::update(channel, status[idx])) {
            if (wups::utils::vpad::triggered(channel, cfg::shortcut1))
                activate_shortcut1();
            if (wups::utils::vpad::triggered(channel, cfg::shortcut2))
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
    if (wups::utils::wpad::update(channel, status)) {
        if (wups::utils::wpad::triggered(channel, cfg::shortcut1))
            activate_shortcut1();
        if (wups::utils::wpad::triggered(channel, cfg::shortcut2))
            activate_shortcut2();
    }
}

WUPS_MUST_REPLACE(WPADRead, WUPS_LOADER_LIBRARY_PADSCORE, WPADRead);
