EXTRA_DIST = \
	bootstrap \
	COPYING \
	README.md


AM_CPPFLAGS = \
	$(DEVKITPRO_CPPFLAGS) \
	-I$(top_srcdir)/include

AM_CXXFLAGS = \
	$(DEVKITPRO_CXXFLAGS) \
	-Wall -Wextra -Werror

AM_LDFLAGS = $(DEVKITPRO_LDFLAGS)

LIBS = $(DEVKITPRO_LIBS)


noinst_LIBRARIES = src/libwupsxx.a


src_libwupsxx_a_SOURCES = 			\
	include/wupsxx/bool_item.hpp		\
	include/wupsxx/button_combo.hpp		\
	include/wupsxx/button_combo_item.hpp	\
	include/wupsxx/button_item.hpp		\
	include/wupsxx/cafe_glyphs.h		\
	include/wupsxx/category.hpp		\
	include/wupsxx/color.hpp		\
	include/wupsxx/color_item.hpp		\
	include/wupsxx/config_error.hpp		\
	include/wupsxx/duration.hpp		\
	include/wupsxx/duration_items.hpp	\
	include/wupsxx/file_item.hpp		\
	include/wupsxx/init.hpp			\
	include/wupsxx/input.hpp		\
	include/wupsxx/int_item.hpp		\
	include/wupsxx/item.hpp			\
	include/wupsxx/logger.hpp		\
	include/wupsxx/numeric_item.hpp		\
	include/wupsxx/storage.hpp		\
	include/wupsxx/storage_error.hpp	\
	include/wupsxx/text_item.hpp		\
	include/wupsxx/var_item.hpp 		\
	src/bool_item.cpp			\
	src/button_combo.cpp			\
	src/button_combo_vpad.cpp		\
	src/button_combo_wpad.cpp		\
	src/button_combo_item.cpp		\
	src/button_item.cpp			\
	src/category.cpp			\
	src/color.cpp				\
	src/color_item.cpp			\
	src/config_error.cpp			\
	src/duration.cpp			\
	src/duration_items.cpp			\
	src/file_item.cpp			\
	src/init.cpp				\
	src/input.cpp				\
	src/int_item.cpp			\
	src/item.cpp				\
	src/logger.cpp				\
	src/numeric_item_impl.hpp		\
	src/storage.cpp				\
	src/storage_error.cpp			\
	src/text_item.cpp			\
	src/utils.cpp src/utils.hpp		\
	src/wpad_status.h


.PHONY: company run


company: compile_flags.txt

compile_flags.txt: Makefile
	printf "%s" "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
	$(CPP) -xc++ /dev/null -E -Wp,-v 2>&1 | sed -n 's,^ ,-I,p' >> $(srcdir)/compile_flags.txt


if BUILD_DEMO

WPS_FILE = demo/wupsxx-demo.wps
CLEANFILES = $(WPS_FILE)


noinst_PROGRAMS = demo/wupsxx-demo.elf

demo_wupsxx_demo_elf_SOURCES = demo/wupsxx-demo.cpp

demo_wupsxx_demo_elf_LDADD = src/libwupsxx.a


all-local: $(WPS_FILE)


run: $(WPS_FILE)
	WIILOAD=tcp:wiiu wiiload $(WPS_FILE)

endif BUILD_DEMO


@INC_AMINCLUDE@
DISTCLEANFILES = $(AMINCLUDE)
