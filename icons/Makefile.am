
graphinator_plugin_24_png_SOURCES = graphinator_plugin.svg
graphinator_plugin_48_png_SOURCES = graphinator_plugin.svg
graphinator_plugin_64_png_SOURCES = graphinator_plugin.svg

BUILT_SOURCES = \
	graphinator_plugin_24.png \
	graphinator_plugin_48.png \
	graphinator_plugin_64.png

CONVERT_SETTINGS = $(CONVERT) -background none -density 1200 -resize

graphinator_plugin_24.png: graphinator_plugin.svg
	$(CONVERT_SETTINGS) 24x24 $< $@

graphinator_plugin_48.png: graphinator_plugin.svg
	$(CONVERT_SETTINGS) 48x48 $< $@

graphinator_plugin_64.png: graphinator_plugin.svg
	$(CONVERT_SETTINGS) 64x64 $< $@

EXTRA_DIST = graphinator_plugin.svg

CLEANFILES = $(BUILT_SOURCES)
