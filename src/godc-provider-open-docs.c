/* 
 *  godc-provider-open-docs.c - Type here a short description of your plugin
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

#include <gdk/gdkkeysyms.h>
#include "godc-provider-open-docs.h"
#include <gtksourceview/gtksourcecompletionitem.h>

#define GODC_PROVIDER_OPEN_DOCS_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GODC_TYPE_PROVIDER_OPEN_DOCS, GodcProviderOpenDocsPrivate))

static void	 godc_provider_open_docs_iface_init	(GtkSourceCompletionProviderIface *iface);

struct _GodcProviderOpenDocsPrivate
{
	GeditWindow *window;
	GdkPixbuf *provider_icon;
	GdkPixbuf *proposal_icon;
};

G_DEFINE_TYPE_WITH_CODE (GodcProviderOpenDocs,
			 godc_provider_open_docs,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
				 		godc_provider_open_docs_iface_init))

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
godc_provider_open_docs_get_name (GtkSourceCompletionProvider *self)
{
	return "Open Documents";
}

static GdkPixbuf * 
godc_provider_open_docs_get_icon (GtkSourceCompletionProvider *self)
{
	return GODC_PROVIDER_OPEN_DOCS (self)->priv->provider_icon;
}

static gboolean
godc_provider_open_docs_activate_proposal (GtkSourceCompletionProvider	*provider,
					   GtkSourceCompletionProposal	*proposal,
					   GtkTextIter			*iter)
{
	GodcProviderOpenDocs *self = GODC_PROVIDER_OPEN_DOCS (provider);
	GeditDocument *doc = g_object_get_data (G_OBJECT (proposal), "document");
	GeditTab *tab = gedit_tab_get_from_document(doc);
	gedit_window_set_active_tab(self->priv->window,tab);
	return TRUE;
}

static GList *
godc_provider_open_docs_get_proposals (GtkSourceCompletionProvider	*base,
				       GtkTextIter			*iter)
{
	GList *item_list = NULL;
	GList *wins,*temp;
	GeditDocument *doc, *current_doc;
	gchar *name, *info;
	GtkSourceCompletionProposal *item;
	GodcProviderOpenDocs *self = GODC_PROVIDER_OPEN_DOCS (base);
	wins = gedit_window_get_documents(self->priv->window);
	current_doc = gedit_window_get_active_document(self->priv->window);
	temp = wins;
	while(temp!=NULL)
	{
		doc = GEDIT_DOCUMENT(temp->data);
		if (doc != current_doc)
		{
			name = gedit_document_get_short_name_for_display (doc);
			info = gedit_document_get_uri_for_display (doc);
			item = GTK_SOURCE_COMPLETION_PROPOSAL (gtk_source_completion_item_new((gchar*)name,
							       NULL,
							       self->priv->proposal_icon,
							       info));
			g_object_set_data (G_OBJECT (item), "document", doc);
			item_list = g_list_append(item_list,item);
			g_free (name);
			g_free (info);			
		}
		temp = g_list_next(temp);
	}

	g_list_free(wins);
	return item_list;
}

static gboolean
godc_provider_open_docs_filter_proposal (GtkSourceCompletionProvider	*provider,
					 GtkSourceCompletionProposal	*proposal,
					 GtkTextIter			*iter,
					 const gchar			*criteria)
{
	const gchar *label;
	
	label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, criteria);
}

static gboolean
godc_provider_open_docs_get_interactive (GtkSourceCompletionProvider *provider)
{
	return FALSE;
}

static gboolean
godc_provider_open_docs_get_automatic (GtkSourceCompletionProvider *provider)
{
	return FALSE;
}

static void 
godc_provider_open_docs_finalize (GObject *object)
{
	GodcProviderOpenDocs *provider = GODC_PROVIDER_OPEN_DOCS (object);
	
	if (provider->priv->provider_icon != NULL)
		g_object_unref (provider->priv->provider_icon);
	
	if (provider->priv->proposal_icon != NULL)
		g_object_unref (provider->priv->proposal_icon);

	G_OBJECT_CLASS (godc_provider_open_docs_parent_class)->finalize (object);
}


static void 
godc_provider_open_docs_class_init (GodcProviderOpenDocsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = godc_provider_open_docs_finalize;
	
	g_type_class_add_private (object_class, sizeof(GodcProviderOpenDocsPrivate));
}

static void
godc_provider_open_docs_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = godc_provider_open_docs_get_name;
	iface->get_icon = godc_provider_open_docs_get_icon;

	iface->get_proposals = godc_provider_open_docs_get_proposals;
	iface->filter_proposal = godc_provider_open_docs_filter_proposal;
	iface->get_interactive = godc_provider_open_docs_get_interactive;
	iface->get_automatic = godc_provider_open_docs_get_automatic;
	iface->activate_proposal = godc_provider_open_docs_activate_proposal;
}

static void 
godc_provider_open_docs_init (GodcProviderOpenDocs * self)
{
	self->priv = GODC_PROVIDER_OPEN_DOCS_GET_PRIVATE (self);
	
	self->priv->proposal_icon = get_icon_from_theme (GTK_STOCK_FILE);
	self->priv->provider_icon = get_icon_from_theme (GTK_STOCK_OPEN);
}

GodcProviderOpenDocs *
godc_provider_open_docs_new	(GeditWindow	*window)
{
	GodcProviderOpenDocs *ret = g_object_new (GODC_TYPE_PROVIDER_OPEN_DOCS, NULL);
	ret->priv->window = window;
	
	return ret;
}
