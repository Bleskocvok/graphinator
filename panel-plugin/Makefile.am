AM_CPPFLAGS =                                       \
	-DG_LOG_DOMAIN=\"xfce4-graphinator-plugin\"     \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"           \
	-I$(top_srcdir)                                 \
	$(PLATFORM_CPPFLAGS)


plugin_LTLIBRARIES =        \
	libgraphinator.la

plugindir =                 \
	$(libdir)/xfce4/panel/plugins


libgraphinator_la_SOURCES = \
	cpu_info.c              \
	data.c                  \
	mem_info.c              \
	drawing.c               \
	graphinator.c           \
	plugin.c                \
	settings.c              

libgraphinator_la_CFLAGS =  \
	-std=c11 -Wall -Wextra  \
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
