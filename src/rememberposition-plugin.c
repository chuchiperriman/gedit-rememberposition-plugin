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

#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>
#include <gedit/gedit-debug.h>

#define REMEMBER_POSITION_PLUGIN_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ((object), REMEMBER_POSITION_TYPE_PLUGIN, RememberPositionPluginPrivate))

struct _RememberPositionPluginPrivate
{
	GeditWindow *gedit_window;
	GtkWidget *window;
	GList *positions;
	GList *current;
	GList *current_list;
};

struct _Position
{
	GeditTab	*tab;
	gint		 line;
	gint		 offset;
};

typedef struct _Position Position;

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
position_free (Position *pos)
{
	g_debug ("Removed position: %i-%i", pos->line, pos->offset);
	g_slice_free (Position, pos);
}

static Position*
position_get_last (RememberPositionPlugin *self)
{
	Position *pos = NULL;
	GList *last = g_list_last (self->priv->positions);
	if (last != NULL)
		pos = (Position*)last->data;
	
	return pos;
}

static void
position_free_list (GList *list)
{
	g_list_foreach (list, (GFunc)position_free, NULL);
	g_list_free (list);
}

static void
position_truncate_to_current (RememberPositionPlugin *self)
{
	if (self->priv->current != NULL)
	{
		g_debug ("truncate");
		/*TODO remove_link quita solo el link ese, necesitamos quitar todos a partir de ese*/
		
		self->priv->positions = g_list_remove_link (self->priv->positions, self->priv->current);
		position_free_list (self->priv->current);
		self->priv->current = NULL;
	}
}

static gboolean
position_store (RememberPositionPlugin *self,
		GtkTextBuffer *buffer)
{
	/*TODO We must free the mark?*/
	gint line;
	GtkTextMark *insert = gtk_text_buffer_get_insert (buffer);
	GtkTextIter iter = {0,};
	gtk_text_buffer_get_iter_at_mark (buffer,
					  &iter,
					  insert);
	line = gtk_text_iter_get_line (&iter);
	
	position_truncate_to_current (self);
	Position *last =position_get_last (self);
	
	if (last == NULL || line > (last->line + 5) || line < (last->line - 5))
	{
		Position *pos = g_slice_new (Position);
		pos->tab = gedit_window_get_active_tab(self->priv->gedit_window);
		pos->line = line;
		pos->offset = gtk_text_iter_get_line_offset (&iter);
		self->priv->positions = g_list_append (self->priv->positions, pos);
		g_debug ("Stored position: %i-%i", pos->line, pos->offset);
		return TRUE;
	}

	return FALSE;
}

static gboolean
position_navigate_previous (RememberPositionPlugin *self)
{
	GtkTextIter iter = {0,};
	Position *pos;
	GList *temp;
	
	if (self->priv->current == NULL)
	{
		pos = position_get_last (self);
		self->priv->current = g_list_last (self->priv->positions);
	}
	else
	{
		temp = g_list_previous (self->priv->current);
		if (temp == NULL)
			return FALSE;
		pos = (Position*)temp->data;
		self->priv->current = temp;
	}
	
	if (pos == NULL)
		return FALSE;
	
	GeditDocument *doc = gedit_window_get_active_document(self->priv->gedit_window);
	GeditDocument *posdoc = gedit_tab_get_document (pos->tab);
	if (doc != posdoc)
	{
		g_debug ("other document");
		gedit_window_set_active_tab (self->priv->gedit_window, pos->tab);
		doc = posdoc;
		/*Sets the document active*/
	}
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (doc);
	/*TODO We need to check if the iter is ok or the offset because
	if the offset is out of rank, gedit crash
	gtk_text_buffer_get_iter_at_line_offset (buffer, 
						 &iter,
						 pos->line,
						 pos->offset);
	*/
	gtk_text_buffer_get_iter_at_line (buffer, 
					  &iter,
					  pos->line);
	gtk_text_buffer_place_cursor (buffer,
				      &iter);
	return TRUE;
	/*TODO Set the position into the line+offset (and document in a future)*/
}

static void
insert_text_cb (GtkTextBuffer *buffer,
		GtkTextIter   *location,
		gchar         *text,
		gint           len,
		gpointer       user_data)
{
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (user_data);
	position_store (self, buffer);
}

static void
delete_range_cb (GtkTextBuffer *buffer,
		 GtkTextIter   *start,
		 GtkTextIter   *end,
		 gpointer       user_data)
{
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (user_data);
	position_store (self, buffer);
}

static void
move_cursor_cb (GtkTextView *text_view,
		GtkMovementStep step,
		gint            count,
		gboolean        extend_selection,
		gpointer        user_data)
{
	
}

static gboolean
key_release_cb (GtkWidget   *widget,
	      GdkEventKey *event,
	      gpointer     user_data)
{
	GdkModifierType mod;
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (user_data);
	
	mod = gtk_accelerator_get_default_mod_mask () & event->state;
	if (mod == GDK_MOD1_MASK && event->keyval == GDK_Left)
	{
		g_debug ("navigate");
		position_navigate_previous (self);
	}
	return FALSE;
}

static void
tab_removed_cb (GeditWindow *geditwindow,
		GeditTab    *tab,
		gpointer     user_data)
{
	GList *l;
	GList *tmp;
	Position *pos;
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (user_data);
	if (self->priv->positions != NULL)
	{
		l = self->priv->positions;
		while (l)
		{
			tmp = l;
			l = g_list_next (l);
			pos = (Position*)tmp->data;
			if (pos->tab == tab)
			{
				self->priv->positions = g_list_delete_link (self->priv->positions, tmp);
				position_free (pos);
			}
		}
	}
}

static void
tab_added_cb (GeditWindow *geditwindow,
	      GeditTab    *tab,
	      gpointer     user_data)
{
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (user_data);
	GeditView *view = gedit_tab_get_view (tab);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	
	g_signal_connect (view, "move-cursor",
			  G_CALLBACK (move_cursor_cb),
			  self);
	g_signal_connect (view, "key-release-event",
			  G_CALLBACK (key_release_cb),
			  self);
	g_signal_connect (buffer, "insert-text",
			  G_CALLBACK (insert_text_cb),
			  self);
	g_signal_connect (buffer, "delete-range",
			  G_CALLBACK (delete_range_cb),
			  self);
}

static void
impl_activate (GeditPlugin *plugin,
	       GeditWindow *window)
{
	gedit_debug (DEBUG_PLUGINS);
	
	RememberPositionPlugin *self = (RememberPositionPlugin*)plugin;
	self->priv->gedit_window = window;
	self->priv->positions = NULL;
	self->priv->current = NULL;
	self->priv->current_list = NULL;

	g_signal_connect (window, "tab-added",
			  G_CALLBACK (tab_added_cb),
			  self);
	g_signal_connect (window, "tab-removed",
			  G_CALLBACK (tab_removed_cb),
			  self);
}

static void
impl_deactivate (GeditPlugin *plugin,
		 GeditWindow *window)
{
	gedit_debug (DEBUG_PLUGINS);
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (plugin);
	if (self->priv->positions != NULL)
	{
		g_list_foreach (self->priv->positions, (GFunc)position_free, NULL);
		g_list_free (self->priv->positions);
	}
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

