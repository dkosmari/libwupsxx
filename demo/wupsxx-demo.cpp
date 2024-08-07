// SPDX-License-Identifier: MIT

#include <chrono>
#include <string>
#include <utility>              // move()

#include <wups.h>

#include <wupsxx/bool_item.hpp>
#include <wupsxx/category.hpp>
#include <wupsxx/color_item.hpp>
#include <wupsxx/duration_items.hpp> // note, plural
#include <wupsxx/int_item.hpp>
#include <wupsxx/storage.hpp>
#include <wupsxx/text_item.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


WUPS_PLUGIN_NAME("wupsxx demo plugin");
WUPS_PLUGIN_VERSION(PACKAGE_VERSION);
WUPS_PLUGIN_DESCRIPTION("Show wupsxx items.");
WUPS_PLUGIN_AUTHOR("Daniel K. O.");
WUPS_PLUGIN_LICENSE("MIT");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("wupsxx-demo");


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
    std::chrono::hours d_value;

    int int_value_1;
    int int_value_2;

    std::string text = "The quick brown fox jumps over the lazy dog.";


    namespace foo {

        bool enabled;

        namespace bar {

            int value;

        }

    }

}


WUPSConfigAPICallbackStatus
menu_open(WUPSConfigCategoryHandle root_handle)
{

    wups::config::category root{root_handle};

    // A bool item, default=true, strings are true/false
    root.add(wups::config::bool_item::create("bool_option_1",
                                             "Boolean option 1",
                                             cfg::bool_option_1, true));

    // Another bool item, default=false, strings are ■/□
    root.add(wups::config::bool_item::create("bool_option_2",
                                             "Boolean option 2",
                                             cfg::bool_option_2, false,
                                             "■", "□"));


    // A color item, only RGB
    root.add(wups::config::color_item::create("foreground",
                                              "Foreground (RGB)",
                                              cfg::foreground, color{0xff, 0x40, 0x80}));

    // Another color item, RGBA
    root.add(wups::config::color_item::create("background",
                                              "Background (RGBA)",
                                              cfg::background, color{0xaa, 0xbb, 0xcc, 0xdd},
                                              true));


    // Some time duration items
    root.add(wups::config::milliseconds_item::create("ms_value",
                                                     "Duration (ms)",
                                                     cfg::ms_value, 10ms,
                                                     0ms, 1000ms));

    root.add(wups::config::seconds_item::create("s_value",
                                                "Duration (s)",
                                                cfg::s_value, 10s,
                                                0s, 1000s));

    root.add(wups::config::minutes_item::create("min_value",
                                                "Duration (min)",
                                                cfg::min_value, 10min,
                                                0min, 1000min));

    root.add(wups::config::hours_item::create("h_value",
                                              "Duration (h)",
                                              cfg::h_value, 10h,
                                              0h, 1000h));

    // An int item
    root.add(wups::config::int_item::create("int_value_1",
                                            "Integer option 1",
                                            cfg::int_value_1, 5,
                                            -100, 100));

    // Another int item, with custom increments
    root.add(wups::config::int_item::create("int_value_2",
                                            "Integer option 2",
                                            cfg::int_value_2, 0,
                                            -1000, 1000,
                                            100, 10));


    // A text item, max width limited to 30 chars.
    root.add(wups::config::text_item::create("text",
                                             "Text",
                                             cfg::text,
                                             30));

    {
        // A category named "Foo"
        wups::config::category cat_foo{"Foo"};
        // Add a text item to this foo category
        cat_foo.add(wups::config::text_item::create({}, "This is Foo"));
        // ... and a bool item
        cat_foo.add(wups::config::bool_item::create({},
                                                    "Enabled",
                                                    cfg::foo::enabled, false,
                                                    "on", "off"));

        // Nest another category named "Bar"
        {
            wups::config::category cat_bar{"Bar"};
            cat_bar.add(wups::config::text_item::create({}, "This is Bar, inside Foo"));
            cat_bar.add(wups::config::int_item::create({},
                                                       "Value",
                                                       cfg::foo::bar::value, -1,
                                                       -10, 10));
            cat_foo.add(std::move(cat_bar));
        }


        // Move it to the root.
        root.add(std::move(cat_foo));
    }


    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}


void
menu_close()
{
    wups::storage::save();
}


INITIALIZE_PLUGIN()
{
    WUPSConfigAPIOptionsV1 options{ .name = "wupsxx demo plugin" };
    WUPSConfigAPI_Init(options, menu_open, menu_close);

    // TODO: load into the cfg:: variables.
}
