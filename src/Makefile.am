plugindir = $(libdir)/gedit-2/plugins

INCLUDES = \
	-I$(top_srcdir) 						\
	$(GEDIT_CFLAGS) 						\
	$(WARN_CFLAGS)							\
	$(DISABLE_DEPRECATED_CFLAGS)					\
	`pkg-config --cflags gtksourceview-2.0`				\
	`pkg-config --cflags gconf-2.0`					\
	-DGEDIT_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"		\
	-DICON_DIR=\""$(datadir)/gtksourceview/pixmaps"\" 

AM_CFLAGS =\
         -Wall\
         -g

plugin_LTLIBRARIES = librememberposition.la

librememberposition_la_SOURCES = \
	rememberposition-plugin.h	\
	rememberposition-plugin.c

librememberposition_la_LDFLAGS = $(PLUGIN_LIBTOOL_FLAGS) `pkg-config --libs gtksourceview-2.0` `pkg-config --libs gconf-2.0`

# Glade files (if you use glade for your plugin, list those files here)
gladedir = $(datadir)/gedit-2/glade
glade_DATA =

plugin_in_files = rememberposition.gedit-plugin.desktop.in

%.gedit-plugin: %.gedit-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gedit-plugin.desktop.in=.gedit-plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA) $(glade_DATA)

DISTCLEANFILES = $(plugin_DATA) $(glade_DATA)
