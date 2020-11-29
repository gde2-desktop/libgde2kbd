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

#ifndef GDE2KBD_KEYBOARD_DRAWING_H
#define GDE2KBD_KEYBOARD_DRAWING_H 1

#include <gtk/gtk.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBgeom.h>

#ifdef __cplusplus
extern "C" {
#endif
#define GDE2KBD_KEYBOARD_DRAWING(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), gde2kbd_keyboard_drawing_get_type (), \
                               Gde2kbdKeyboardDrawing))
#define GDE2KBD_KEYBOARD_DRAWING_CLASS(clazz) (G_TYPE_CHECK_CLASS_CAST ((clazz), gde2kbd_keyboard_drawing_get_type () \
                                       Gde2kbdKeyboardDrawingClass))
#define GDE2KBD_IS_KEYBOARD_DRAWING(obj) G_TYPE_CHECK_INSTANCE_TYPE ((obj), gde2kbd_keyboard_drawing_get_type ())
typedef struct _Gde2kbdKeyboardDrawing Gde2kbdKeyboardDrawing;
typedef struct _Gde2kbdKeyboardDrawingClass Gde2kbdKeyboardDrawingClass;

typedef struct _Gde2kbdKeyboardDrawingItem Gde2kbdKeyboardDrawingItem;
typedef struct _Gde2kbdKeyboardDrawingKey Gde2kbdKeyboardDrawingKey;
typedef struct _Gde2kbdKeyboardDrawingDoodad Gde2kbdKeyboardDrawingDoodad;
typedef struct _Gde2kbdKeyboardDrawingGroupLevel
 Gde2kbdKeyboardDrawingGroupLevel;
typedef struct _Gde2kbdKeyboardDrawingRenderContext
 Gde2kbdKeyboardDrawingRenderContext;

typedef enum {
	GDE2KBD_KEYBOARD_DRAWING_ITEM_TYPE_INVALID = 0,
	GDE2KBD_KEYBOARD_DRAWING_ITEM_TYPE_KEY,
	GDE2KBD_KEYBOARD_DRAWING_ITEM_TYPE_KEY_EXTRA,
	GDE2KBD_KEYBOARD_DRAWING_ITEM_TYPE_DOODAD
} Gde2kbdKeyboardDrawingItemType;

typedef enum {
	GDE2KBD_KEYBOARD_DRAWING_POS_TOPLEFT,
	GDE2KBD_KEYBOARD_DRAWING_POS_TOPRIGHT,
	GDE2KBD_KEYBOARD_DRAWING_POS_BOTTOMLEFT,
	GDE2KBD_KEYBOARD_DRAWING_POS_BOTTOMRIGHT,
	GDE2KBD_KEYBOARD_DRAWING_POS_TOTAL,
	GDE2KBD_KEYBOARD_DRAWING_POS_FIRST =
	    GDE2KBD_KEYBOARD_DRAWING_POS_TOPLEFT,
	GDE2KBD_KEYBOARD_DRAWING_POS_LAST =
	    GDE2KBD_KEYBOARD_DRAWING_POS_BOTTOMRIGHT
} Gde2kbdKeyboardDrawingGroupLevelPosition;

/* units are in xkb form */
struct _Gde2kbdKeyboardDrawingItem {
	/*< private > */

	Gde2kbdKeyboardDrawingItemType type;
	gint origin_x;
	gint origin_y;
	gint angle;
	guint priority;
};

/* units are in xkb form */
struct _Gde2kbdKeyboardDrawingKey {
	/*< private > */

	Gde2kbdKeyboardDrawingItemType type;
	gint origin_x;
	gint origin_y;
	gint angle;
	guint priority;

	XkbKeyRec *xkbkey;
	gboolean pressed;
	guint keycode;
};

/* units are in xkb form */
struct _Gde2kbdKeyboardDrawingDoodad {
	/*< private > */

	Gde2kbdKeyboardDrawingItemType type;
	gint origin_x;
	gint origin_y;
	gint angle;
	guint priority;

	XkbDoodadRec *doodad;
	gboolean on;		/* for indicator doodads */
};

struct _Gde2kbdKeyboardDrawingGroupLevel {
	gint group;
	gint level;
};

struct _Gde2kbdKeyboardDrawingRenderContext {
	cairo_t *cr;

	gint angle;		/* current angle pango is set to draw at, in tenths of a degree */
	PangoLayout *layout;
	PangoFontDescription *font_desc;

	gint scale_numerator;
	gint scale_denominator;

#if GTK_CHECK_VERSION (3, 0, 0)
	GdkRGBA dark_color;
#else
	GdkColor dark_color;
#endif
};

struct _Gde2kbdKeyboardDrawing {
	/*< private > */

	GtkDrawingArea parent;

	cairo_surface_t *surface;
	XkbDescRec *xkb;
	gboolean xkbOnDisplay;
	guint l3mod;

	Gde2kbdKeyboardDrawingRenderContext *renderContext;

	/* Indexed by keycode */
	Gde2kbdKeyboardDrawingKey *keys;

	/* list of stuff to draw in priority order */
	GList *keyboard_items;

#if GTK_CHECK_VERSION (3, 0, 0)
	GdkRGBA *colors;
#else
	GdkColor *colors;
#endif

	guint timeout;
	guint idle_redraw;

	Gde2kbdKeyboardDrawingGroupLevel **groupLevels;

	guint mods;

	Display *display;
	gint screen_num;

	gint xkb_event_type;

	Gde2kbdKeyboardDrawingDoodad **physical_indicators;
	gint physical_indicators_size;

	guint track_config:1;
	guint track_modifiers:1;
};

struct _Gde2kbdKeyboardDrawingClass {
	GtkDrawingAreaClass parent_class;

	/* we send this signal when the user presses a key that "doesn't exist"
	 * according to the keyboard geometry; it probably means their xkb
	 * configuration is incorrect */
	void (*bad_keycode) (Gde2kbdKeyboardDrawing * drawing, guint keycode);
};

GType gde2kbd_keyboard_drawing_get_type (void);
GtkWidget *gde2kbd_keyboard_drawing_new (void);

gboolean gde2kbd_keyboard_drawing_render (Gde2kbdKeyboardDrawing * kbdrawing,
				       cairo_t * cr,
				       PangoLayout * layout,
				       double x, double y,
				       double width, double height,
				       gdouble dpi_x, gdouble dpi_y);
gboolean gde2kbd_keyboard_drawing_set_keyboard (Gde2kbdKeyboardDrawing *
					     kbdrawing,
					     XkbComponentNamesRec * names);

const gchar* gde2kbd_keyboard_drawing_get_keycodes(Gde2kbdKeyboardDrawing* kbdrawing);
const gchar* gde2kbd_keyboard_drawing_get_geometry(Gde2kbdKeyboardDrawing* kbdrawing);
const gchar* gde2kbd_keyboard_drawing_get_symbols(Gde2kbdKeyboardDrawing* kbdrawing);
const gchar* gde2kbd_keyboard_drawing_get_types(Gde2kbdKeyboardDrawing* kbdrawing);
const gchar* gde2kbd_keyboard_drawing_get_compat(Gde2kbdKeyboardDrawing* kbdrawing);

void gde2kbd_keyboard_drawing_set_track_modifiers (Gde2kbdKeyboardDrawing *
						kbdrawing,
						gboolean enable);
void gde2kbd_keyboard_drawing_set_track_config (Gde2kbdKeyboardDrawing *
					     kbdrawing, gboolean enable);

void gde2kbd_keyboard_drawing_set_groups_levels (Gde2kbdKeyboardDrawing *
					      kbdrawing,
					      Gde2kbdKeyboardDrawingGroupLevel
					      * groupLevels[]);


void gde2kbd_keyboard_drawing_print (Gde2kbdKeyboardDrawing * drawing,
				  GtkWindow * parent_window,
				  const gchar * description);


GtkWidget* gde2kbd_keyboard_drawing_new_dialog (gint group, gchar* group_name);

#ifdef __cplusplus
}
#endif
#endif				/* #ifndef GDE2KBD_KEYBOARD_DRAWING_H */
