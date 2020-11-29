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

#ifndef __GDE2KBD_INDICATOR_H__
#define __GDE2KBD_INDICATOR_H__

#include <gtk/gtk.h>

#include <libxklavier/xklavier.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _Gde2kbdIndicator Gde2kbdIndicator;
	typedef struct _Gde2kbdIndicatorPrivate Gde2kbdIndicatorPrivate;
	typedef struct _Gde2kbdIndicatorClass Gde2kbdIndicatorClass;

#define GDE2KBD_TYPE_INDICATOR             (gde2kbd_indicator_get_type ())
#define GDE2KBD_INDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDE2KBD_TYPE_INDICATOR, Gde2kbdIndicator))
#define GDE2KBD_INDICATOR_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), GDE2KBD_TYPE_INDICATOR,  Gde2kbdIndicatorClass))
#define GDE2KBD_IS_INDICATOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDE2KBD_TYPE_INDICATOR))
#define GDE2KBD_IS_INDICATOR_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GDE2KBD_TYPE_INDICATOR))
#define GDE2KBD_INDICATOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GDE2KBD_TYPE_INDICATOR, Gde2kbdIndicatorClass))

	struct _Gde2kbdIndicator {
		GtkNotebook parent;
		Gde2kbdIndicatorPrivate *priv;
	};

	struct _Gde2kbdIndicatorClass {
		GtkNotebookClass parent_class;

		void (*reinit_ui) (Gde2kbdIndicator * gki);
	};

	extern GType gde2kbd_indicator_get_type (void);

	extern GtkWidget *gde2kbd_indicator_new (void);

	extern void gde2kbd_indicator_reinit_ui (Gde2kbdIndicator * gki);

	extern void gde2kbd_indicator_set_angle (Gde2kbdIndicator * gki,
					      gdouble angle);

	extern XklEngine *gde2kbd_indicator_get_xkl_engine (void);

	extern gchar **gde2kbd_indicator_get_group_names (void);

	extern gchar *gde2kbd_indicator_get_image_filename (guint group);

	extern gdouble gde2kbd_indicator_get_max_width_height_ratio (void);

	extern void
	 gde2kbd_indicator_set_parent_tooltips (Gde2kbdIndicator *
					     gki, gboolean ifset);

	extern void
	 gde2kbd_indicator_set_tooltips_format (const gchar str[]);

#ifdef __cplusplus
}
#endif
#endif
