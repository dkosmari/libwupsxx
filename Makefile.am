EXTRA_DIST = \
	bootstrap \
	COPYING \
	README.md


AM_CPPFLAGS = \
	$(DEVKITPRO_CPPFLAGS) \
	-I$(top_srcdir)/include

AM_CXXFLAGS = \
	$(DEVKITPRO_CXXFLAGS) \
	-Wall -Wextra

AM_LDFLAGS = $(DEVKITPRO_LDFLAGS)

LIBS = $(DEVKITPRO_LIBS)


wupsxxdir = $(includedir)/wupsxx

wupsxx_HEADERS = 				\
	include/wupsxx/bool_item.hpp		\
	include/wupsxx/category.hpp		\
	include/wupsxx/color.hpp		\
	include/wupsxx/color_item.hpp		\
	include/wupsxx/config_error.hpp		\
	include/wupsxx/duration.hpp		\
	include/wupsxx/duration_items.hpp	\
	include/wupsxx/file_item.hpp		\
	include/wupsxx/int_item.hpp		\
	include/wupsxx/item.hpp			\
	include/wupsxx/numeric_item.hpp		\
	include/wupsxx/storage_error.hpp	\
	include/wupsxx/storage.hpp		\
	include/wupsxx/text_item.hpp		\
	include/wupsxx/var_item.hpp


lib_LIBRARIES = src/libwupsxx.a


src_libwupsxx_a_SOURCES = 		\
	src/bool_item.cpp		\
	src/category.cpp		\
	src/color.cpp			\
	src/color_item.cpp		\
	src/config_error.cpp		\
	src/duration.cpp		\
	src/duration_items.cpp		\
	src/file_item.cpp		\
	src/int_item.cpp		\
	src/item.cpp			\
	src/numeric_item_impl.hpp	\
	src/storage.cpp			\
	src/storage_error.cpp		\
	src/text_item.cpp


WPS_FILE = wupsxx-demo.wps

CLEANFILES = $(WPS_FILE)

noinst_PROGRAMS = wupsxx-demo.elf

wupsxx_demo_elf_SOURCES = demo/wupsxx-demo.cpp

wupsxx_demo_elf_LDADD = src/libwupsxx.a


all-local: $(WPS_FILE)


.PHONY: run

run: $(WPS_FILE)
	WIILOAD=tcp:wiiu wiiload $(WPS_FILE)


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	printf "%s" "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt


@INC_AMINCLUDE@
DISTCLEANFILES = $(AMINCLUDE)
