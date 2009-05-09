/*
 * rememberposition-plugin.h - Adds (auto)completion support to gedit
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

#ifndef __REMEMBER_POSITION_PLUGIN_H__
#define __REMEMBER_POSITION_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <gedit/gedit-plugin.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define REMEMBER_POSITION_TYPE_PLUGIN		(remember_position_plugin_get_type ())
#define REMEMBER_POSITION_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), REMEMBER_POSITION_TYPE_PLUGIN, RememberPositionPlugin))
#define REMEMBER_POSITION_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), REMEMBER_POSITION_TYPE_PLUGIN, RememberPositionPluginClass))
#define IS_REMEMBER_POSITION_PLUGIN(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), REMEMBER_POSITION_TYPE_PLUGIN))
#define IS_REMEMBER_POSITION_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), REMEMBER_POSITION_TYPE_PLUGIN))
#define REMEMBER_POSITION_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), REMEMBER_POSITION_TYPE_PLUGIN, RememberPositionPluginClass))

/* Private structure type */
typedef struct _RememberPositionPluginPrivate	RememberPositionPluginPrivate;

/*
 * Main object structure
 */
typedef struct _RememberPositionPlugin		RememberPositionPlugin;

struct _RememberPositionPlugin
{
	GeditPlugin parent_instance;

	/*< private >*/
	RememberPositionPluginPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _RememberPositionPluginClass	RememberPositionPluginClass;

struct _RememberPositionPluginClass
{
	GeditPluginClass parent_class;
};

/*
 * Public methods
 */
GType	remember_position_plugin_get_type	(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gedit_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __REMEMBER_POSITION_PLUGIN_H__ */
