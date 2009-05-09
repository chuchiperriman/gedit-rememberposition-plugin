/* 
 *  godc-provider-recent.c - Type here a short description of your plugin
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

#include "godc-provider-recent.h"
#include <gtksourceview/gtksourcecompletionitem.h>

#define GODC_PROVIDER_RECENT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GODC_TYPE_PROVIDER_RECENT, GodcProviderRecentPrivate))

static void	 godc_provider_recent_iface_init	(GtkSourceCompletionProviderIface *iface);

struct _GodcProviderRecentPrivate
{
	GeditWindow *window;
	GdkPixbuf *provider_icon;
	GdkPixbuf *proposal_icon;
};

G_DEFINE_TYPE_WITH_CODE (GodcProviderRecent,
			 godc_provider_recent,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
				 		godc_provider_recent_iface_init))

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

static gint
sort_recents_mru (GtkRecentInfo *a, GtkRecentInfo *b)
{
        return (gtk_recent_info_get_modified (b) - gtk_recent_info_get_modified (a));
}

static const gchar * 
godc_provider_recent_get_name (GtkSourceCompletionProvider *self)
{
	return "Recent documents";
}

static GdkPixbuf * 
godc_provider_recent_get_icon (GtkSourceCompletionProvider *self)
{
	return GODC_PROVIDER_RECENT (self)->priv->provider_icon;
}


static GList *
godc_provider_recent_get_proposals (GtkSourceCompletionProvider *base,
				    GtkTextIter *iter)
{
	GodcProviderRecent *self = GODC_PROVIDER_RECENT (base);
	GtkSourceCompletionProposal *item;
	GList *item_list = NULL;
	gint max_recent = 20;
	GtkRecentManager *recent_manager =  gtk_recent_manager_get_default ();
	GList *items = gtk_recent_manager_get_items (recent_manager);
	GList *filtered_items = NULL, *l;
	/* filter */
        for (l = items; l != NULL; l = l->next)
        {
                GtkRecentInfo *info = l->data;
                if (!gtk_recent_info_has_group (info, "gedit"))
                        continue;
                filtered_items = g_list_prepend (filtered_items, info);
        }

        /* sort */
        filtered_items = g_list_sort (filtered_items,
                                      (GCompareFunc) sort_recents_mru);
	gint i = 0;
        for (l = filtered_items; l != NULL; l = l->next)
        {
        	GtkRecentInfo *info = l->data;
		
		
		/* FIXME Check objects */
		const gchar *name = gtk_recent_info_get_display_name (info);
		gchar *display_info = gtk_recent_info_get_uri_display (info);
		const gchar *uri = gtk_recent_info_get_uri (info);

		item = GTK_SOURCE_COMPLETION_PROPOSAL (gtk_source_completion_item_new((gchar*)name,
						       NULL,
						       self->priv->proposal_icon,
						       (gchar*)display_info));
		g_object_set_data_full (G_OBJECT (item), 
					"uri",
					g_strdup (uri),
					(GDestroyNotify)g_free);
		g_free (display_info);		
		item_list = g_list_append(item_list,item);
        	++i;
        	if (i>=max_recent)
        		break;
        }
        g_list_free(filtered_items);
        g_list_foreach (items, (GFunc) gtk_recent_info_unref, NULL);
        g_list_free (items);
        
	return item_list;
}

static gboolean
godc_provider_recent_activate_proposal (GtkSourceCompletionProvider *provider,
				 GtkSourceCompletionProposal *proposal,
				 GtkTextIter *iter)
{
	GodcProviderRecent *self = GODC_PROVIDER_RECENT (provider);
	gchar *uri = g_object_get_data (G_OBJECT (proposal), "uri");
	
	gedit_commands_load_uri(self->priv->window,
                                uri,
                                NULL,
                                1);
	return TRUE;
}

static gboolean
godc_provider_recent_filter_proposal (GtkSourceCompletionProvider *provider,
                                   GtkSourceCompletionProposal *proposal,
                                   GtkTextIter *iter,
                                   const gchar                 *criteria)
{
	const gchar *label;
	
	label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, criteria);
}

static gboolean
godc_provider_recent_get_interactive (GtkSourceCompletionProvider *provider)
{
	return TRUE;
}

static gboolean
godc_provider_recent_get_automatic (GtkSourceCompletionProvider *provider)
{
	return FALSE;
}

static void 
godc_provider_recent_finalize (GObject *object)
{
	GodcProviderRecent *provider = GODC_PROVIDER_RECENT (object);
	
	if (provider->priv->provider_icon != NULL)
	{
		g_object_unref (provider->priv->provider_icon);
	}
	
	if (provider->priv->proposal_icon != NULL)
	{
		g_object_unref (provider->priv->proposal_icon);
	}

	G_OBJECT_CLASS (godc_provider_recent_parent_class)->finalize (object);
}


static void 
godc_provider_recent_class_init (GodcProviderRecentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = godc_provider_recent_finalize;
	
	g_type_class_add_private (object_class, sizeof(GodcProviderRecentPrivate));
}

static void
godc_provider_recent_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = godc_provider_recent_get_name;
	iface->get_icon = godc_provider_recent_get_icon;

	iface->get_proposals = godc_provider_recent_get_proposals;
	iface->filter_proposal = godc_provider_recent_filter_proposal;
	iface->get_interactive = godc_provider_recent_get_interactive;
	iface->get_automatic = godc_provider_recent_get_automatic;
	iface->activate_proposal = godc_provider_recent_activate_proposal;
}

static void 
godc_provider_recent_init (GodcProviderRecent * self)
{
	self->priv = GODC_PROVIDER_RECENT_GET_PRIVATE (self);
	
	self->priv->proposal_icon = get_icon_from_theme (GTK_STOCK_FILE);
	self->priv->provider_icon = get_icon_from_theme (GTK_STOCK_HARDDISK);
}

GodcProviderRecent *
godc_provider_recent_new (GeditWindow *window)
{
	GodcProviderRecent *ret = g_object_new (GODC_TYPE_PROVIDER_RECENT, NULL);
	ret->priv->window = window;
	return ret;
}
