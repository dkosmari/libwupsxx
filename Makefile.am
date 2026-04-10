EXTRA_DIST = \
	bootstrap \
	COPYING \
	README.md


SUBDIRS = external/WiiUPluginSystem

DIST_SUBDIRS = external


AM_CPPFLAGS = \
	$(WUPS_CPPFLAGS) \
	-I$(srcdir)/include

AM_LDFLAGS = $(WUPS_LDFLAGS)

AM_CXXFLAGS = -Wall -Wextra -Werror


LDADD = $(WUPS_LIB)


noinst_LTLIBRARIES = lib/libwupsxx.la


lib_libwupsxx_la_SOURCES = 			\
	include/wupsxx/bool_item.hpp		\
	include/wupsxx/button_item.hpp		\
	include/wupsxx/cafe_glyphs.h		\
	include/wupsxx/category.hpp		\
	include/wupsxx/color.hpp		\
	include/wupsxx/color_item.hpp		\
	include/wupsxx/concepts.hpp		\
	include/wupsxx/duration.hpp		\
	include/wupsxx/duration_items.hpp	\
	include/wupsxx/error.hpp		\
	include/wupsxx/file_item.hpp		\
	include/wupsxx/float_item.hpp		\
	include/wupsxx/init.hpp			\
	include/wupsxx/input.hpp		\
	include/wupsxx/int_item.hpp		\
	include/wupsxx/item.hpp			\
	include/wupsxx/logger.hpp		\
	include/wupsxx/numeric_item.hpp		\
	include/wupsxx/option.hpp		\
	include/wupsxx/storage.hpp		\
	include/wupsxx/text_item.hpp		\
	include/wupsxx/var_item.hpp 		\
	src/bool_item.cpp			\
	src/button_item.cpp			\
	src/category.cpp			\
	src/color.cpp				\
	src/color_item.cpp			\
	src/duration.cpp			\
	src/duration_items.cpp			\
	src/error.cpp				\
	src/file_item.cpp			\
	src/float_item.cpp			\
	src/init.cpp				\
	src/input.cpp				\
	src/int_item.cpp			\
	src/item.cpp				\
	src/logger.cpp				\
	src/numeric_item_impl.hpp		\
	src/option.cpp				\
	src/storage.cpp				\
	src/text_item.cpp			\
	src/utils.cpp src/utils.hpp


EXTRA_lib_libwupsxx_la_DEPENDENCIES = $(WUPS_LIB)


if USE_SHORTCUTS

SUBDIRS += external/libbuttoncombo

lib_libwupsxx_la_SOURCES +=			\
	include/wupsxx/shortcut.hpp		\
	include/wupsxx/shortcut_item.hpp	\
	src/shortcut.cpp			\
	src/shortcut_item.cpp

EXTRA_lib_libwupsxx_la_DEPENDENCIES += $(LIBBUTTONCOMBO_LIB)

AM_CPPFLAGS += $(LIBBUTTONCOMBO_CPPFLAGS)

LDADD += $(LIBBUTTONCOMBO_LIB)

endif USE_SHORTCUTS


if USE_NOTIFY

SUBDIRS += external/libnotifications

lib_libwupsxx_la_SOURCES +=			\
	include/wupsxx/notify.hpp		\
	src/notify.cpp

EXTRA_lib_libwupsxx_la_DEPENDENCIES += $(LIBNOTIFICATIONS_LIB)

AM_CPPFLAGS += $(LIBNOTIFICATIONS_CPPFLAGS)

LDADD += $(LIBNOTIFICATIONS_LIB)

endif USE_NOTIFY


.PHONY: company run


if BUILD_DEMO

WPS_FILE = demo/wupsxx-demo.wps
CLEANFILES = $(WPS_FILE)


noinst_PROGRAMS = demo/wupsxx-demo.elf

demo_wupsxx_demo_elf_SOURCES = 		\
	demo/press_counter_item.cpp 	\
	demo/press_counter_item.hpp 	\
	demo/wait_5_seconds_item.cpp 	\
	demo/wait_5_seconds_item.hpp 	\
	demo/main.cpp

demo_wupsxx_demo_elf_LDADD = \
	lib/libwupsxx.la \
	$(LDADD)

all-local: $(WPS_FILE)


run: $(WPS_FILE)
	WIILOAD=tcp:wiiu wiiload $(WPS_FILE)

endif BUILD_DEMO



company: compile_flags.txt

compile_flags.txt: Makefile
	printf "%s" "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS)" | xargs -n1 | sort -u > compile_flags.txt
	$(CPP) -xc++ /dev/null -E -Wp,-v 2>&1 | sed -n 's,^ ,-I,p' >> compile_flags.txt


DISTCLEANFILES = $(AMINCLUDE)

@INC_AMINCLUDE@
