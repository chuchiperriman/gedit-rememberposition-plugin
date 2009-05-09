/* 
 *  godc-provider-open-docs.h - Type here a short description of your plugin
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
#ifndef __PROVIDER_OPEN_DOCS_H__
#define __PROVIDER_OPEN_DOCS_H__

#include <glib.h>
#include <glib-object.h>
#include <gedit/gedit-plugin.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

#define GODC_TYPE_PROVIDER_OPEN_DOCS (godc_provider_open_docs_get_type ())
#define GODC_PROVIDER_OPEN_DOCS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GODC_TYPE_PROVIDER_OPEN_DOCS, GodcProviderOpenDocs))
#define GODC_PROVIDER_OPEN_DOCS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GODC_TYPE_PROVIDER_OPEN_DOCS, GodcProviderOpenDocsClass))
#define GODC_IS_PROVIDER_OPEN_DOCS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GODC_TYPE_PROVIDER_OPEN_DOCS))
#define GODC_IS_PROVIDER_OPEN_DOCS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GODC_TYPE_PROVIDER_OPEN_DOCS))
#define GODC_PROVIDER_OPEN_DOCS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GODC_TYPE_PROVIDER_OPEN_DOCS, GodcProviderOpenDocsClass))

#define GODC_PROVIDER_OPEN_DOCS_NAME "GodcProviderOpenDocs"

typedef struct _GodcProviderOpenDocs GodcProviderOpenDocs;
typedef struct _GodcProviderOpenDocsPrivate GodcProviderOpenDocsPrivate;
typedef struct _GodcProviderOpenDocsClass GodcProviderOpenDocsClass;

struct _GodcProviderOpenDocs
{
	GObject parent;
	
	GodcProviderOpenDocsPrivate *priv;
};

struct _GodcProviderOpenDocsClass
{
	GObjectClass parent;
};

GType		 godc_provider_open_docs_get_type	(void) G_GNUC_CONST;

GodcProviderOpenDocs *godc_provider_open_docs_new	(GeditWindow	*window);

G_END_DECLS

#endif
