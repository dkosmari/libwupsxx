LIBBUTTONCOMBO_FILES ?= \
	$(addprefix libbuttoncombo/,$(shell git -C libbuttoncombo ls-files))

LIBNOTIFICATIONS_FILES ?= \
	$(addprefix libnotifications/,$(shell git -C libnotifications ls-files))

WUPS_FILES ?= \
	$(addprefix WiiUPluginSystem/,$(shell git -C WiiUPluginSystem ls-files))

EXTRA_DIST = \
	update-submodules.sh \
	$(LIBBUTTONCOMBO_FILES) \
	$(LIBNOTIFICATIONS_FILES) \
	$(WUPS_FILES)

