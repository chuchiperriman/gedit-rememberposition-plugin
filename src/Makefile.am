#Remember position plugin
plugindir = $(GEDIT_PLUGINS_LIBS_DIR)
supportdir = $(GEDIT_PLUGINS_DATA_DIR)/rememberposition

INCLUDES = \
	-I$(top_srcdir) 						\
	$(GEDIT_CFLAGS) 						\
	$(WARN_CFLAGS)							\
	$(DISABLE_DEPRECATED_CFLAGS)					

plugin_LTLIBRARIES = librememberposition.la

librememberposition_la_SOURCES = \
	rememberposition-plugin.h	\
	rememberposition-plugin.c

librememberposition_la_LDFLAGS = $(PLUGIN_LIBTOOL_FLAGS)
librememberposition_la_LIBADD =	$(GEDIT_LIBS)

support_DATA =

# Plugin Info
plugin_in_files = rememberposition.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

BUILDFILES = $(plugin_in_files:.plugin.desktop.in=.plugin)

plugin_DATA = $(BUILDFILES)

EXTRA_DIST = $(plugin_in_files) $(support_DATA)

CLEANFILES = $(BUILDFILES) 
DISTCLEANFILES = $(BUILDFILES)

-include $(top_srcdir)/git.mk
