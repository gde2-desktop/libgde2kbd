/*
 * Copyright (C) 2006 Sergey V. Udaltsov <svu@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GDE2KBD_INDICATOR_CONFIG_H__
#define __GDE2KBD_INDICATOR_CONFIG_H__

#include <gtk/gtk.h>

#include "libgde2kbd/gde2kbd-keyboard-config.h"

/*
 * Indicator configuration
 */
typedef struct _Gde2kbdIndicatorConfig Gde2kbdIndicatorConfig;
struct _Gde2kbdIndicatorConfig {
	int secondary_groups_mask;
	gboolean show_flags;

	gchar *font_family;
	gchar *foreground_color;
	gchar *background_color;

	/* private, transient */
	GSettings *settings;
	GSList *image_filenames;
	GtkIconTheme *icon_theme;
	int config_listener_id;
	XklEngine *engine;
};

/*
 * Gde2kbdIndicatorConfig functions -
 * some of them require Gde2kbdKeyboardConfig as well -
 * for loading approptiate images
 */
extern void gde2kbd_indicator_config_init (Gde2kbdIndicatorConfig *
					applet_config,
					XklEngine * engine);
extern void gde2kbd_indicator_config_term (Gde2kbdIndicatorConfig *
					applet_config);

extern void gde2kbd_indicator_config_load_from_gsettings (Gde2kbdIndicatorConfig
						   * applet_config);
extern void gde2kbd_indicator_config_save_to_gsettings (Gde2kbdIndicatorConfig *
						 applet_config);

extern void gde2kbd_indicator_config_refresh_style (Gde2kbdIndicatorConfig *
						 applet_config);

extern gchar
    * gde2kbd_indicator_config_get_images_file (Gde2kbdIndicatorConfig *
					     applet_config,
					     Gde2kbdKeyboardConfig *
					     kbd_config, int group);

extern void gde2kbd_indicator_config_load_image_filenames (Gde2kbdIndicatorConfig
							* applet_config,
							Gde2kbdKeyboardConfig
							* kbd_config);
extern void gde2kbd_indicator_config_free_image_filenames (Gde2kbdIndicatorConfig
							* applet_config);

/* Should be updated on Indicator/GSettings configuration change */
extern void gde2kbd_indicator_config_activate (Gde2kbdIndicatorConfig *
					    applet_config);

extern void gde2kbd_indicator_config_start_listen (Gde2kbdIndicatorConfig *
						applet_config,
						GCallback
						func, gpointer user_data);

extern void gde2kbd_indicator_config_stop_listen (Gde2kbdIndicatorConfig *
					       applet_config);

#endif
