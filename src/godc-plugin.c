/*
 * godc-plugin.c - Adds (auto)completion support to gedit
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

#include "godc-plugin.h"

#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>
#include <gedit/gedit-debug.h>
#include "godc-provider-open-docs.h"
#include "godc-provider-recent.h"
#include "godc-provider-file-browser.h"

#define GODC_PLUGIN_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ((object), GODC_TYPE_PLUGIN, GodcPluginPrivate))

#define GODC_PROVIDERS_KEY "godc-providers"

struct _GodcPluginPrivate
{
	GeditWindow *gedit_window;
	GtkWidget *window;
};

typedef struct _ViewAndCompletion ViewAndCompletion;

GEDIT_PLUGIN_REGISTER_TYPE (GodcPlugin, godc_plugin)

static gboolean
view_key_release_cb 	(GtkSourceView 		*view,
			 GdkEventKey 		*event,
			 GodcProviderOpenDocs	*self)
{
	GtkSourceCompletion *completion;
	guint key = 0;
	GdkModifierType mod;
	guint s;
	gtk_accelerator_parse ("<Control>b", &key, &mod);
	completion = gtk_source_view_get_completion (view);
	
	s = event->state & gtk_accelerator_get_default_mod_mask();
	if (s == mod && gdk_keyval_to_lower(event->keyval) == key)
	{
		GList *providers = g_object_get_data (G_OBJECT (completion),
						      GODC_PROVIDERS_KEY);
		if (providers != NULL){
			gtk_source_completion_show (completion,
						    providers,
						    NULL,
						    NULL);
		}
	}
	
	return FALSE;
}

static void
godc_plugin_init (GodcPlugin *plugin)
{
	plugin->priv = GODC_PLUGIN_GET_PRIVATE (plugin);
	gedit_debug_message (DEBUG_PLUGINS,
			     "GodcPlugin initializing");
}

static void
godc_plugin_finalize (GObject *object)
{
	gedit_debug_message (DEBUG_PLUGINS,
			     "GodcPlugin finalizing");
	G_OBJECT_CLASS (godc_plugin_parent_class)->finalize (object);
}

static void
tab_added_cb (GeditWindow *geditwindow,
	      GeditTab    *tab,
	      gpointer     user_data)
{
	GList *providers = NULL;
	GeditView *view = gedit_tab_get_view (tab);
	GtkSourceCompletion *comp = gtk_source_view_get_completion (GTK_SOURCE_VIEW (view));
	
	GodcProviderOpenDocs *dw = godc_provider_open_docs_new (geditwindow);
	gtk_source_completion_add_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(dw), NULL);
	g_object_unref(dw);
	
	GodcProviderRecent *pr = godc_provider_recent_new (geditwindow);
	gtk_source_completion_add_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(pr), NULL);
	g_object_unref(pr);
	
	GodcProviderFileBrowser *pfb = godc_provider_file_browser_new (geditwindow);
	gtk_source_completion_add_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(pfb), NULL);
	g_object_unref(pfb);
	
	providers = g_list_append (providers, dw);
	providers = g_list_append (providers, pr);
	providers = g_list_append (providers, pfb);
	
	g_object_set_data_full (G_OBJECT (comp),
				GODC_PROVIDERS_KEY,
				providers,
				(GDestroyNotify) g_list_free);
	
	g_signal_connect(view, "key-release-event", G_CALLBACK(view_key_release_cb), user_data);
	
	g_debug ("provider registered");
}

static void
impl_activate (GeditPlugin *plugin,
	       GeditWindow *window)
{
	GodcPlugin * dw_plugin = (GodcPlugin*)plugin;
	dw_plugin->priv->gedit_window = window;
	gedit_debug (DEBUG_PLUGINS);
	
	g_signal_connect (window, "tab-added",
			  G_CALLBACK (tab_added_cb),
			  NULL);

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
godc_plugin_class_init (GodcPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GeditPluginClass *plugin_class = GEDIT_PLUGIN_CLASS (klass);

	object_class->finalize = godc_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;

	g_type_class_add_private (object_class, 
				  sizeof (GodcPluginPrivate));
}

