/* 
 *  godc-provider-recent.h - Type here a short description of your plugin
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
#ifndef __PROVIDER_RECENT_H__
#define __PROVIDER_RECENT_H__

#include <glib.h>
#include <glib-object.h>
#include <gedit/gedit-plugin.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

#define GODC_TYPE_PROVIDER_RECENT (godc_provider_recent_get_type ())
#define GODC_PROVIDER_RECENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GODC_TYPE_PROVIDER_RECENT, GodcProviderRecent))
#define GODC_PROVIDER_RECENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GODC_TYPE_PROVIDER_RECENT, GodcProviderRecentClass))
#define GODC_IS_PROVIDER_RECENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GODC_TYPE_PROVIDER_RECENT))
#define GODC_IS_PROVIDER_RECENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GODC_TYPE_PROVIDER_RECENT))
#define GODC_PROVIDER_RECENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GODC_TYPE_PROVIDER_RECENT, GodcProviderRecentClass))

#define GODC_PROVIDER_RECENT_NAME "GodcProviderRecent"

typedef struct _GodcProviderRecent GodcProviderRecent;
typedef struct _GodcProviderRecentPrivate GodcProviderRecentPrivate;
typedef struct _GodcProviderRecentClass GodcProviderRecentClass;

struct _GodcProviderRecent
{
	GObject parent;
	
	GodcProviderRecentPrivate *priv;
};

struct _GodcProviderRecentClass
{
	GObjectClass parent;
};

GType		 godc_provider_recent_get_type	(void) G_GNUC_CONST;

GodcProviderRecent *godc_provider_recent_new (GeditWindow *window);

G_END_DECLS

#endif
