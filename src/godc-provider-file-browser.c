/* 
 *  godc-provider-file-browser.c - Type here a short description of your plugin
 *
 *  Copyright (C) 2008 - perriman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "godc-provider-file-browser.h"
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletionitem.h>

#define GODC_PROVIDER_FILE_BROWSER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GODC_TYPE_PROVIDER_FILE_BROWSER, GodcProviderFileBrowserPrivate))

static void	 godc_provider_file_browser_iface_init	(GtkSourceCompletionProviderIface *iface);

struct _GodcProviderFileBrowserPrivate
{
	GeditWindow *window;
	GdkPixbuf *provider_icon;
	GdkPixbuf *proposal_icon;
};

G_DEFINE_TYPE_WITH_CODE (GodcProviderFileBrowser,
			 godc_provider_file_browser,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
				 		godc_provider_file_browser_iface_init))

/*TODO Share this functions with others providers*/
static GdkPixbuf *
get_icon_from_theme (const gchar *name)
{
        GtkIconTheme *theme;
        gint width;

        theme = gtk_icon_theme_get_default ();

        gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, NULL);
        return gtk_icon_theme_load_icon (theme,
                                         name,
                                         width,
                                         GTK_ICON_LOOKUP_USE_BUILTIN,
                                         NULL);
}

static const gchar * 
godc_provider_file_browser_get_name (GtkSourceCompletionProvider *self)
{
	return "File Browser files";
}

static GdkPixbuf * 
godc_provider_file_browser_get_icon (GtkSourceCompletionProvider *self)
{
	return GODC_PROVIDER_FILE_BROWSER (self)->priv->provider_icon;
}

static gboolean
godc_provider_file_browser_activate_proposal (GtkSourceCompletionProvider	*provider,
					   GtkSourceCompletionProposal	*proposal,
					   GtkTextIter			*iter)
{
	GodcProviderFileBrowser *self = GODC_PROVIDER_FILE_BROWSER (provider);
	gchar *uri = g_object_get_data (G_OBJECT (proposal), "uri");
	
	g_debug ("opening uri: %s", uri);
	gedit_commands_load_uri(self->priv->window,
                                uri,
                                NULL,
                                1);
	return TRUE;
}

static GList *
godc_provider_file_browser_get_proposals (GtkSourceCompletionProvider *base,
                                 GtkTextIter                 *iter)
{
	GodcProviderFileBrowser *self = GODC_PROVIDER_FILE_BROWSER (base);
	GList *list = NULL;
	gchar *path, *file_uri;
	const gchar *filename, *uri;
	GeditMessageBus *bus = gedit_window_get_message_bus (self->priv->window);
	GeditMessage *message;
	GValue val = {0,};
	GFile *file;
	GtkSourceCompletionProposal *item;
	
	message = gedit_message_bus_send_sync (bus,
					       "/plugins/filebrowser",
					       "get_root",
					       NULL);
	gedit_message_get_value (message,
				 "uri",
				 &val);
	uri = g_value_get_string (&val);
	if (uri == NULL)
		return NULL;
	
	g_object_unref (message);	
	
	/*TODO I think we must to use g_file_enumerate_children*/
	
	GFile *root = g_file_new_for_uri (uri);
	path = g_file_get_path (root);
	GDir *dir = g_dir_open (path, 0, NULL);
	while ((filename = g_dir_read_name (dir)) != NULL )
	{
		file = g_file_get_child (root, filename);
		if (g_file_query_file_type (file, G_FILE_QUERY_INFO_NONE, NULL) == G_FILE_TYPE_REGULAR)
		{
			file_uri = g_file_get_uri (file);

			item = GTK_SOURCE_COMPLETION_PROPOSAL (gtk_source_completion_item_new((gchar*)filename,
							       NULL,
							       self->priv->proposal_icon,
							       file_uri));
			g_object_set_data_full (G_OBJECT (item), 
						"uri",
						file_uri,
						(GDestroyNotify)g_free);
			list = g_list_append(list,item);
		}
		g_object_unref (file);
	}
	
	g_free (path);
	g_object_unref (root);
	
	return list;
}

static gboolean
godc_provider_file_browser_filter_proposal (GtkSourceCompletionProvider *provider,
                                   GtkSourceCompletionProposal *proposal,
                                   GtkTextIter                 *iter,
                                   const gchar                 *criteria)
{
	const gchar *label;
	
	label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, criteria);
}

static gboolean
godc_provider_file_browser_get_interactive (GtkSourceCompletionProvider *provider)
{
	return FALSE;
}

static gboolean
godc_provider_file_browser_get_automatic (GtkSourceCompletionProvider *provider)
{
	return FALSE;
}

static void 
godc_provider_file_browser_finalize (GObject *object)
{
	GodcProviderFileBrowser *provider = GODC_PROVIDER_FILE_BROWSER (object);
	
	if (provider->priv->provider_icon != NULL)
		g_object_unref (provider->priv->provider_icon);
	
	if (provider->priv->proposal_icon != NULL)
		g_object_unref (provider->priv->proposal_icon);

	G_OBJECT_CLASS (godc_provider_file_browser_parent_class)->finalize (object);
}

static void 
godc_provider_file_browser_class_init (GodcProviderFileBrowserClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = godc_provider_file_browser_finalize;
	
	g_type_class_add_private (object_class, sizeof(GodcProviderFileBrowserPrivate));
}

static void
godc_provider_file_browser_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = godc_provider_file_browser_get_name;
	iface->get_icon = godc_provider_file_browser_get_icon;

	iface->get_proposals = godc_provider_file_browser_get_proposals;
	iface->filter_proposal = godc_provider_file_browser_filter_proposal;
	iface->get_interactive = godc_provider_file_browser_get_interactive;
	iface->get_automatic = godc_provider_file_browser_get_automatic;
	iface->activate_proposal = godc_provider_file_browser_activate_proposal;
}

static void 
godc_provider_file_browser_init (GodcProviderFileBrowser * self)
{
	self->priv = GODC_PROVIDER_FILE_BROWSER_GET_PRIVATE (self);
	
	self->priv->proposal_icon = get_icon_from_theme (GTK_STOCK_FILE);
	self->priv->provider_icon = get_icon_from_theme (GTK_STOCK_OPEN);
}

GodcProviderFileBrowser *
godc_provider_file_browser_new (GeditWindow *window)
{
	GodcProviderFileBrowser *ret = g_object_new (GODC_TYPE_PROVIDER_FILE_BROWSER, NULL);
	ret->priv->window = window;
	
	return ret;
}
