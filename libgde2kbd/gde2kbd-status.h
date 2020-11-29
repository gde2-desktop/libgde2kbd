/*
 * Copyright (C) 2009 Sergey V. Udaltsov <svu@gnome.org>
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

#ifndef __GDE2KBD_STATUS_H__
#define __GDE2KBD_STATUS_H__

#include <gtk/gtk.h>

#include <libxklavier/xklavier.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _Gde2kbdStatus Gde2kbdStatus;
	typedef struct _Gde2kbdStatusPrivate Gde2kbdStatusPrivate;
	typedef struct _Gde2kbdStatusClass Gde2kbdStatusClass;

	#define GDE2KBD_TYPE_STATUS             (gde2kbd_status_get_type ())
	#define GDE2KBD_STATUS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDE2KBD_TYPE_STATUS, Gde2kbdStatus))
	#define GDE2KBD_STATUS_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), GDE2KBD_TYPE_STATUS,  Gde2kbdStatusClass))
	#define GDE2KBD_IS_STATUS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDE2KBD_TYPE_STATUS))
	#define GDE2KBD_IS_STATUS_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GDE2KBD_TYPE_STATUS))
	#define GDE2KBD_STATUS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GDE2KBD_TYPE_STATUS, Gde2kbdStatusClass))

	struct _Gde2kbdStatus {
		GtkStatusIcon parent;
		Gde2kbdStatusPrivate *priv;
	};

	struct _Gde2kbdStatusClass {
		GtkNotebookClass parent_class;
	};

	extern GType gde2kbd_status_get_type (void);

	extern GtkStatusIcon *gde2kbd_status_new (void);

	extern void gde2kbd_status_reinit_ui (Gde2kbdStatus * gki);

	extern void gde2kbd_status_set_angle (Gde2kbdStatus * gki,
					   gdouble angle);

	extern XklEngine *gde2kbd_status_get_xkl_engine (void);

	extern gchar **gde2kbd_status_get_group_names (void);

	extern gchar *gde2kbd_status_get_image_filename (guint group);

	extern void
	 gde2kbd_status_set_tooltips_format (const gchar str[]);

#ifdef __cplusplus
}
#endif
#endif
