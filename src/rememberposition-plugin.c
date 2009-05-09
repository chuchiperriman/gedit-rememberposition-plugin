/*
 * rememberposition-plugin.c - Adds (auto)completion support to gedit
 *
 * Copyright (C) 2007 - chuchiperriman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rememberposition-plugin.h"

#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>
#include <gedit/gedit-debug.h>

#define REMEMBER_POSITION_PLUGIN_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ((object), REMEMBER_POSITION_TYPE_PLUGIN, RememberPositionPluginPrivate))

struct _RememberPositionPluginPrivate
{
	GeditWindow *gedit_window;
	GtkWidget *window;
};

GEDIT_PLUGIN_REGISTER_TYPE (RememberPositionPlugin, remember_position_plugin)

static void
remember_position_plugin_init (RememberPositionPlugin *plugin)
{
	plugin->priv = REMEMBER_POSITION_PLUGIN_GET_PRIVATE (plugin);
	gedit_debug_message (DEBUG_PLUGINS,
			     "RememberPositionPlugin initializing");
}

static void
remember_position_plugin_finalize (GObject *object)
{
	gedit_debug_message (DEBUG_PLUGINS,
			     "RememberPositionPlugin finalizing");
	G_OBJECT_CLASS (remember_position_plugin_parent_class)->finalize (object);
}

static void
tab_added_cb (GeditWindow *geditwindow,
	      GeditTab    *tab,
	      gpointer     user_data)
{
	GeditView *view = gedit_tab_get_view (tab);
	
	g_debug ("provider registered");
}

static void
impl_activate (GeditPlugin *plugin,
	       GeditWindow *window)
{
	RememberPositionPlugin *self = (RememberPositionPlugin*)plugin;
	self->priv->gedit_window = window;
	gedit_debug (DEBUG_PLUGINS);
	
	g_signal_connect (window, "tab-added",
			  G_CALLBACK (tab_added_cb),
			  self);
}

static void
impl_deactivate (GeditPlugin *plugin,
		 GeditWindow *window)
{
	gedit_debug (DEBUG_PLUGINS);
}

static void
impl_update_ui (GeditPlugin *plugin,
		GeditWindow *window)
{
}

static void
remember_position_plugin_class_init (RememberPositionPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GeditPluginClass *plugin_class = GEDIT_PLUGIN_CLASS (klass);

	object_class->finalize = remember_position_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;

	g_type_class_add_private (object_class, 
				  sizeof (RememberPositionPluginPrivate));
}

