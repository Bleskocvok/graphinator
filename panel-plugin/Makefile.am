AM_CPPFLAGS =                                       \
	-DG_LOG_DOMAIN=\"xfce4-graphinator-plugin\"     \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"           \
	-I $(top_srcdir)                                \
	$(PLATFORM_CPPFLAGS)


plugin_LTLIBRARIES =        \
	libgraphinator.la

plugindir =                 \
	$(libdir)/xfce4/panel/plugins

libgraphinator_la_SOURCES = \
	mem_info.c              \
	cpu_info.c              \
	data.c                  \
	drawing.c               \
	collector.c             \
	panel_entry.c           \
	graphinator.c           \
	plugin.c                \
	settings.c              \
	defaults.c              

libgraphinator_la_CFLAGS =  \
	-std=c11 -Wall -Wextra  \
	-O2 -march=native       \
	$(LIBXFCE4UTIL_CFLAGS)  \
	$(LIBXFCE4UI_CFLAGS)    \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(PLATFORM_CFLAGS)

libgraphinator_la_LDFLAGS = \
	-avoid-version          \
	-module                 \
	-no-undefined           \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)'  \
	$(PLATFORM_LDFLAGS)

libgraphinator_la_LIBADD =  \
	$(LIBXFCE4UTIL_LIBS)    \
	$(LIBXFCE4UI_LIBS)      \
	$(LIBXFCE4PANEL_LIBS)
