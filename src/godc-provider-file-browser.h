/* 
 *  godc-provider-file-browser.h - Type here a short description of your plugin
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
#ifndef __PROVIDER_FILE_BROWSER_H__
#define __PROVIDER_FILE_BROWSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourceview/gtksourcecompletionprovider.h>
#include <gedit/gedit-plugin.h>

G_BEGIN_DECLS

#define GODC_TYPE_PROVIDER_FILE_BROWSER (godc_provider_file_browser_get_type ())
#define GODC_PROVIDER_FILE_BROWSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GODC_TYPE_PROVIDER_FILE_BROWSER, GodcProviderFileBrowser))
#define GODC_PROVIDER_FILE_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GODC_TYPE_PROVIDER_FILE_BROWSER, GodcProviderFileBrowserClass))
#define GODC_IS_PROVIDER_FILE_BROWSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GODC_TYPE_PROVIDER_FILE_BROWSER))
#define GODC_IS_PROVIDER_FILE_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GODC_TYPE_PROVIDER_FILE_BROWSER))
#define GODC_PROVIDER_FILE_BROWSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GODC_TYPE_PROVIDER_FILE_BROWSER, GodcProviderFileBrowserClass))

#define GODC_PROVIDER_FILE_BROWSER_NAME "GodcProviderFileBrowser"

typedef struct _GodcProviderFileBrowser GodcProviderFileBrowser;
typedef struct _GodcProviderFileBrowserPrivate GodcProviderFileBrowserPrivate;
typedef struct _GodcProviderFileBrowserClass GodcProviderFileBrowserClass;

struct _GodcProviderFileBrowser
{
	GObject parent;
	
	GodcProviderFileBrowserPrivate *priv;
};

struct _GodcProviderFileBrowserClass
{
	GObjectClass parent;
};

GType		 godc_provider_file_browser_get_type	(void) G_GNUC_CONST;

GodcProviderFileBrowser *godc_provider_file_browser_new (GeditWindow *window);

G_END_DECLS

#endif
