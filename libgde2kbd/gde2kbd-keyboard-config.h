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

#ifndef __GDE2KBD_KEYBOARD_CONFIG_H__
#define __GDE2KBD_KEYBOARD_CONFIG_H__

#include <X11/Xlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <libxklavier/xklavier.h>

extern const gchar GDE2KBD_KEYBOARD_CONFIG_KEY_MODEL[];
extern const gchar GDE2KBD_KEYBOARD_CONFIG_KEY_LAYOUTS[];
extern const gchar GDE2KBD_KEYBOARD_CONFIG_KEY_OPTIONS[];

/*
 * Keyboard Configuration
 */
typedef struct _Gde2kbdKeyboardConfig Gde2kbdKeyboardConfig;
struct _Gde2kbdKeyboardConfig {
	gchar *model;
	gchar **layouts_variants;
	gchar **options;

	/* private, transient */
	GSettings *settings;
	int config_listener_id;
	XklEngine *engine;
};

/*
 * Gde2kbdKeyboardConfig functions
 */
extern void gde2kbd_keyboard_config_init (Gde2kbdKeyboardConfig * kbd_config,
				       XklEngine * engine);
extern void gde2kbd_keyboard_config_term (Gde2kbdKeyboardConfig * kbd_config);

extern void gde2kbd_keyboard_config_load_from_gsettings (Gde2kbdKeyboardConfig *
						  kbd_config,
						  Gde2kbdKeyboardConfig *
						  kbd_config_default);

extern void gde2kbd_keyboard_config_save_to_gsettings (Gde2kbdKeyboardConfig *
						kbd_config);

extern void gde2kbd_keyboard_config_load_from_x_initial (Gde2kbdKeyboardConfig *
						      kbd_config,
						      XklConfigRec * buf);

extern void gde2kbd_keyboard_config_load_from_x_current (Gde2kbdKeyboardConfig *
						      kbd_config,
						      XklConfigRec * buf);

extern void gde2kbd_keyboard_config_start_listen (Gde2kbdKeyboardConfig *
					       kbd_config,
					       GCallback func,
					       gpointer user_data);

extern void gde2kbd_keyboard_config_stop_listen (Gde2kbdKeyboardConfig *
					      kbd_config);

extern gboolean gde2kbd_keyboard_config_equals (Gde2kbdKeyboardConfig *
					     kbd_config1,
					     Gde2kbdKeyboardConfig *
					     kbd_config2);

extern gboolean gde2kbd_keyboard_config_activate (Gde2kbdKeyboardConfig *
					       kbd_config);

extern const gchar *gde2kbd_keyboard_config_merge_items (const gchar * parent,
						      const gchar * child);

extern gboolean gde2kbd_keyboard_config_split_items (const gchar * merged,
						  gchar ** parent,
						  gchar ** child);

extern gboolean gde2kbd_keyboard_config_get_descriptions (XklConfigRegistry *
						       config_registry,
						       const gchar * name,
						       gchar **
						       layout_short_descr,
						       gchar **
						       layout_descr,
						       gchar **
						       variant_short_descr,
						       gchar **
						       variant_descr);

extern const gchar *gde2kbd_keyboard_config_format_full_layout (const gchar
							     *
							     layout_descr,
							     const gchar *
							     variant_descr);

extern gchar *gde2kbd_keyboard_config_to_string (const Gde2kbdKeyboardConfig *
					      config);

extern gchar
    **gde2kbd_keyboard_config_add_default_switch_option_if_necessary (gchar **
								  layouts_list,
								  gchar **
								  options_list,
								  gboolean
								  *
								  was_appended);

#endif
