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

#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>
#include <gedit/gedit-debug.h>
#include <gedit/gedit-window.h>
#include <gedit/gedit-window-activatable.h>
#include <gedit/gedit-document.h>

#define REMEMBER_POSITION_PLUGIN_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ((object), REMEMBER_POSITION_TYPE_PLUGIN, RememberPositionPluginPrivate))

#define MIN_LEN 14

struct _RememberPositionPluginPrivate
{
	GeditWindow *gedit_window;
	GtkWidget *window;
	GList *positions;
	GList *current_pos;
	GList *current_list;
};

struct _Position
{
	GeditTab	*tab;
	gint		 line;
	gint		 offset;
};

typedef struct _Position Position;

static void gedit_window_activatable_iface_init (GeditWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (RememberPositionPlugin,
				remember_position_plugin,
				PEAS_TYPE_EXTENSION_BASE,
				0,
				G_IMPLEMENT_INTERFACE_DYNAMIC (GEDIT_TYPE_WINDOW_ACTIVATABLE,
							       gedit_window_activatable_iface_init))

static void
remember_position_plugin_init (RememberPositionPlugin *plugin)
{
	plugin->priv = REMEMBER_POSITION_PLUGIN_GET_PRIVATE (plugin);
	gedit_debug_message (DEBUG_PLUGINS,
			     "RememberPositionPlugin initializing");
}

static void
remember_position_plugin_dispose (GObject *object)
{
	gedit_debug_message (DEBUG_PLUGINS,
			     "RememberPositionPlugin finalizing");
	G_OBJECT_CLASS (remember_position_plugin_parent_class)->dispose (object);
}

static void
position_free (Position *pos)
{
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
clean_current_list (RememberPositionPlugin *self)
{
	g_list_free(self->priv->current_list);
	self->priv->current_list = NULL;
	self->priv->current_pos = NULL;
}

static void
rebuild_positions (RememberPositionPlugin *self)
{
	GList *temp, *final = NULL;
	Position *pos = (Position*)self->priv->current_pos->data;
	for (temp = self->priv->current_list; temp; temp = g_list_next (temp))
	{
		if (pos == temp->data)
			break;
		final = g_list_append (final, temp->data);
	}
	
	for (temp = g_list_last (self->priv->current_list); temp; temp = g_list_previous (temp))
	{
		final = g_list_append (final, temp->data);
		if (pos == temp->data)
			break;
	}
	g_list_free (self->priv->positions);
	self->priv->positions = final;
	
	clean_current_list (self);
}

static Position*
position_new (RememberPositionPlugin *self, GtkTextIter *iter)
{
	Position *pos = g_slice_new (Position);
	pos->tab = gedit_window_get_active_tab(self->priv->gedit_window);
	pos->line = gtk_text_iter_get_line (iter);
	pos->offset = gtk_text_iter_get_line_offset (iter);
	return pos;
}

static gboolean
position_store (RememberPositionPlugin *self,
		GtkTextBuffer *buffer)
{
	/*TODO We must free the mark?*/
	gint line;
	GtkTextMark *insert = gtk_text_buffer_get_insert (buffer);
	GtkTextIter iter = {0,};
	GeditDocument *doc = gedit_window_get_active_document (self->priv->gedit_window);
	gtk_text_buffer_get_iter_at_mark (buffer,
					  &iter,
					  insert);
	line = gtk_text_iter_get_line (&iter);
	
	if (self->priv->current_list != NULL)
	{
		rebuild_positions (self);
	}
	
	Position *last = position_get_last (self);
	Position *pos = NULL;
	if (last == NULL || doc != gedit_tab_get_document(last->tab))
	{
		pos = position_new (self, &iter);
	}
	else
	{
		if (line > (last->line + MIN_LEN) || line < (last->line - MIN_LEN))
		{
			pos = position_new (self, &iter);
		}
		else
		{
			last->line = line;
			last->offset = gtk_text_iter_get_line_offset (&iter);
		}
	}
	
	if (pos != NULL)
		self->priv->positions = g_list_append (self->priv->positions, pos);

	return TRUE;
}

static void
set_current_list (RememberPositionPlugin *self)
{
	self->priv->current_list = g_list_copy (self->priv->positions);
	self->priv->current_pos = g_list_last (self->priv->current_list);
}

static gboolean
position_navigate (RememberPositionPlugin *self, Position *pos)
{
	GtkTextIter iter = {0,};

	if (pos == NULL)
		return FALSE;
	
	GeditDocument *doc = gedit_window_get_active_document(self->priv->gedit_window);
	GeditDocument *posdoc = gedit_tab_get_document (pos->tab);
	GtkTextBuffer *buffer;
	GtkTextView *view;
	if (doc != posdoc)
	{
		/*Sets the document active*/
		gedit_window_set_active_tab (self->priv->gedit_window, pos->tab);
		doc = posdoc;
	}
	
	buffer = GTK_TEXT_BUFFER (doc);
	view = GTK_TEXT_VIEW (gedit_window_get_active_view (self->priv->gedit_window));
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
	gtk_text_view_scroll_to_iter (view, &iter,0.0,FALSE,0.0,0.0);
	return TRUE;
	/*TODO Set the position into the line+offset (and document in a future)*/
}

static gboolean
position_navigate_previous (RememberPositionPlugin *self)
{
	Position *pos;
	GList *temp;
	
	if (self->priv->positions == NULL)
		return FALSE;
	
	if (self->priv->current_list == NULL)
	{
		set_current_list (self);
		pos = (Position*)self->priv->current_pos->data;
	}
	else
	{
		temp = g_list_previous (self->priv->current_pos);
		if (temp == NULL)
			return FALSE;
		pos = (Position*)temp->data;
		self->priv->current_pos = temp;
	}

	return position_navigate (self, pos);
}

static gboolean
position_navigate_next (RememberPositionPlugin *self)
{
	Position *pos;
	GList *temp;
	
	if (self->priv->positions == NULL)
		return FALSE;
	
	if (self->priv->current_list == NULL)
	{
		set_current_list (self);
		pos = (Position*)self->priv->current_pos->data;
	}
	else
	{
		temp = g_list_next (self->priv->current_pos);
		if (temp == NULL)
			return FALSE;
		pos = (Position*)temp->data;
		self->priv->current_pos = temp;
	}

	return position_navigate (self, pos);
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
	if (mod == GDK_MOD1_MASK)
	{
		if (event->keyval == GDK_KEY_Left)
			position_navigate_previous (self);
		else if (event->keyval == GDK_KEY_Right)
			position_navigate_next (self);
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
	clean_current_list (self);
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
impl_activate (GeditWindowActivatable *activatable)
{
    GeditWindow *window;
	gedit_debug (DEBUG_PLUGINS);
	
	g_object_get (activatable,
               "window", &window,
               NULL);
	
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN(activatable);
	self->priv->gedit_window = window;
	self->priv->positions = NULL;
	self->priv->current_pos = NULL;
	self->priv->current_list = NULL;

	g_signal_connect (window, "tab-added",
			  G_CALLBACK (tab_added_cb),
			  self);
	g_signal_connect (window, "tab-removed",
			  G_CALLBACK (tab_removed_cb),
			  self);
}

static void
impl_deactivate (GeditWindowActivatable *activatable)
{
	gedit_debug (DEBUG_PLUGINS);
	RememberPositionPlugin *self = REMEMBER_POSITION_PLUGIN (activatable);
	if (self->priv->positions != NULL)
	{
		g_list_foreach (self->priv->positions, (GFunc)position_free, NULL);
		g_list_free (self->priv->positions);
	}
}

static void
remember_position_plugin_class_init (RememberPositionPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = remember_position_plugin_dispose;

	g_type_class_add_private (object_class, 
				  sizeof (RememberPositionPluginPrivate));
}

static void
remember_position_plugin_class_finalize (RememberPositionPluginClass *klass)
{
}

static void
gedit_window_activatable_iface_init (GeditWindowActivatableInterface *iface)
{
	iface->activate = impl_activate;
	iface->deactivate = impl_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
	remember_position_plugin_register_type (G_TYPE_MODULE (module));

	peas_object_module_register_extension_type (module,
						    GEDIT_TYPE_WINDOW_ACTIVATABLE,
						    REMEMBER_POSITION_TYPE_PLUGIN);
}

