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
#include <config.h>

#include <memory.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <glib/gi18n-lib.h>

#include <gde2kbd-indicator.h>
#include <gde2kbd-indicator-marshal.h>

#include <gde2kbd-desktop-config.h>
#include <gde2kbd-indicator-config.h>

typedef struct _gki_globals {
	XklEngine *engine;
	XklConfigRegistry *registry;

	Gde2kbdDesktopConfig cfg;
	Gde2kbdIndicatorConfig ind_cfg;
	Gde2kbdKeyboardConfig kbd_cfg;

	const gchar *tooltips_format;
	gchar **full_group_names;
	gchar **short_group_names;
	GSList *widget_instances;
	GSList *images;
} gki_globals;

struct _Gde2kbdIndicatorPrivate {
	gboolean set_parent_tooltips;
	gdouble angle;
};

/* one instance for ALL widgets */
static gki_globals globals;

#define ForAllIndicators() \
	{ \
		GSList* cur; \
		for (cur = globals.widget_instances; cur != NULL; cur = cur->next) { \
			Gde2kbdIndicator * gki = (Gde2kbdIndicator*)cur->data;
#define NextIndicator() \
		} \
	}

G_DEFINE_TYPE (Gde2kbdIndicator, gde2kbd_indicator, GTK_TYPE_NOTEBOOK)

static void
gde2kbd_indicator_global_init (void);
static void
gde2kbd_indicator_global_term (void);
static GtkWidget *
gde2kbd_indicator_prepare_drawing (Gde2kbdIndicator * gki, int group);
static void
gde2kbd_indicator_set_current_page_for_group (Gde2kbdIndicator * gki, int group);
static void
gde2kbd_indicator_set_current_page (Gde2kbdIndicator * gki);
static void
gde2kbd_indicator_cleanup (Gde2kbdIndicator * gki);
static void
gde2kbd_indicator_fill (Gde2kbdIndicator * gki);
static void
gde2kbd_indicator_set_tooltips (Gde2kbdIndicator * gki, const char *str);

void
gde2kbd_indicator_load_images ()
{
	int i;
	GSList *image_filename;

	globals.images = NULL;
	gde2kbd_indicator_config_load_image_filenames (&globals.ind_cfg,
						    &globals.kbd_cfg);

	if (!globals.ind_cfg.show_flags)
		return;

	image_filename = globals.ind_cfg.image_filenames;

	for (i = xkl_engine_get_max_num_groups (globals.engine);
	     --i >= 0; image_filename = image_filename->next) {
		GdkPixbuf *image = NULL;
		char *image_file = (char *) image_filename->data;

		if (image_file != NULL) {
			GError *gerror = NULL;
			image =
			    gdk_pixbuf_new_from_file (image_file, &gerror);
			if (image == NULL) {
				GtkWidget *dialog =
				    gtk_message_dialog_new (NULL,
							    GTK_DIALOG_DESTROY_WITH_PARENT,
							    GTK_MESSAGE_ERROR,
							    GTK_BUTTONS_OK,
							    _
							    ("There was an error loading an image: %s"),
							    gerror->
							    message);
				g_signal_connect (G_OBJECT (dialog),
						  "response",
						  G_CALLBACK
						  (gtk_widget_destroy),
						  NULL);

				gtk_window_set_resizable (GTK_WINDOW
							  (dialog), FALSE);

				gtk_widget_show (dialog);
				g_error_free (gerror);
			}
			xkl_debug (150,
				   "Image %d[%s] loaded -> %p[%dx%d]\n",
				   i, image_file, image,
				   gdk_pixbuf_get_width (image),
				   gdk_pixbuf_get_height (image));
		}
		/* We append the image anyway - even if it is NULL! */
		globals.images = g_slist_append (globals.images, image);
	}
}

static void
gde2kbd_indicator_free_images ()
{
	GdkPixbuf *pi;
	GSList *img_node;

	gde2kbd_indicator_config_free_image_filenames (&globals.ind_cfg);

	while ((img_node = globals.images) != NULL) {
		pi = GDK_PIXBUF (img_node->data);
		/* It can be NULL - some images may be missing */
		if (pi != NULL) {
			g_object_unref (pi);
		}
		globals.images =
		    g_slist_remove_link (globals.images, img_node);
		g_slist_free_1 (img_node);
	}
}

static void
gde2kbd_indicator_update_images (void)
{
	gde2kbd_indicator_free_images ();
	gde2kbd_indicator_load_images ();
}

void
gde2kbd_indicator_set_tooltips (Gde2kbdIndicator * gki, const char *str)
{
	g_assert (str == NULL || g_utf8_validate (str, -1, NULL));

	gtk_widget_set_tooltip_text (GTK_WIDGET (gki), str);

	if (gki->priv->set_parent_tooltips) {
		GtkWidget *parent =
		    gtk_widget_get_parent (GTK_WIDGET (gki));
		if (parent) {
			gtk_widget_set_tooltip_text (parent, str);
		}
	}
}

void
gde2kbd_indicator_cleanup (Gde2kbdIndicator * gki)
{
	int i;
	GtkNotebook *notebook = GTK_NOTEBOOK (gki);

	/* Do not remove the first page! It is the default page */
	for (i = gtk_notebook_get_n_pages (notebook); --i > 0;) {
		gtk_notebook_remove_page (notebook, i);
	}
}

void
gde2kbd_indicator_fill (Gde2kbdIndicator * gki)
{
	int grp;
	int total_groups = xkl_engine_get_num_groups (globals.engine);
	GtkNotebook *notebook = GTK_NOTEBOOK (gki);

	for (grp = 0; grp < total_groups; grp++) {
		GtkWidget *page;
		page = gde2kbd_indicator_prepare_drawing (gki, grp);

		if (page == NULL)
			page = gtk_label_new ("");

		gtk_notebook_append_page (notebook, page, NULL);
		gtk_widget_show_all (page);
	}
}

static gboolean gde2kbd_indicator_key_pressed(GtkWidget* widget, GdkEventKey* event, Gde2kbdIndicator* gki)
{
	switch (event->keyval)
	{
			case GDK_KEY_KP_Enter:
			case GDK_KEY_ISO_Enter:
			case GDK_KEY_3270_Enter:
			case GDK_KEY_Return:
			case GDK_KEY_space:
			case GDK_KEY_KP_Space:
			gde2kbd_desktop_config_lock_next_group(&globals.cfg);
			return TRUE;
		default:
			break;
	}

	return FALSE;
}

static gboolean
gde2kbd_indicator_button_pressed (GtkWidget *
			       widget,
			       GdkEventButton * event, Gde2kbdIndicator * gki)
{
	GtkWidget *img = gtk_bin_get_child (GTK_BIN (widget));
	GtkAllocation allocation;
	gtk_widget_get_allocation (img, &allocation);
	xkl_debug (150, "Flag img size %d x %d\n",
		   allocation.width, allocation.height);
	if (event->button == 1 && event->type == GDK_BUTTON_PRESS) {
		xkl_debug (150, "Mouse button pressed on applet\n");
		gde2kbd_desktop_config_lock_next_group (&globals.cfg);
		return TRUE;
	}
	return FALSE;
}

static void
flag_exposed (GtkWidget * flag, GdkEventExpose * event, GdkPixbuf * image)
{
	/* Image width and height */
	int iw = gdk_pixbuf_get_width (image);
	int ih = gdk_pixbuf_get_height (image);
	GtkAllocation allocation;
	double xwiratio, ywiratio, wiratio;
        cairo_t *cr;

	gtk_widget_get_allocation (flag, &allocation);

        cr = gdk_cairo_create (event->window);
        gdk_cairo_region (cr, event->region);
        cairo_clip (cr);

	/* widget-to-image scales, X and Y */
	xwiratio = 1.0 * allocation.width / iw;
	ywiratio = 1.0 * allocation.height / ih;
	wiratio = xwiratio < ywiratio ? xwiratio : ywiratio;

        /* transform cairo context */
        cairo_translate (cr, allocation.width / 2.0, allocation.height / 2.0);
        cairo_scale (cr, wiratio, wiratio);
        cairo_translate (cr, - iw / 2.0, - ih / 2.0);

        gdk_cairo_set_source_pixbuf (cr, image, 0, 0);
        cairo_paint (cr);

        cairo_destroy (cr);
}

gchar *
gde2kbd_indicator_extract_layout_name (int group, XklEngine * engine,
				    Gde2kbdKeyboardConfig * kbd_cfg,
				    gchar ** short_group_names,
				    gchar ** full_group_names)
{
	char *layout_name = NULL;
	if (group < g_strv_length (short_group_names)) {
		if (xkl_engine_get_features (engine) &
		    XKLF_MULTIPLE_LAYOUTS_SUPPORTED) {
			char *full_layout_name =
			    kbd_cfg->layouts_variants[group];
			char *variant_name;
			if (!gde2kbd_keyboard_config_split_items
			    (full_layout_name, &layout_name,
			     &variant_name))
				/* just in case */
				layout_name = full_layout_name;

			/* make it freeable */
			layout_name = g_strdup (layout_name);

			if (short_group_names != NULL) {
				char *short_group_name =
				    short_group_names[group];
				if (short_group_name != NULL
				    && *short_group_name != '\0') {
					/* drop the long name */
					g_free (layout_name);
					layout_name =
					    g_strdup (short_group_name);
				}
			}
		} else {
			layout_name = g_strdup (full_group_names[group]);
		}
	}

	if (layout_name == NULL)
		layout_name = g_strdup ("");

	return layout_name;
}

gchar *
gde2kbd_indicator_create_label_title (int group, GHashTable ** ln2cnt_map,
				   gchar * layout_name)
{
	gpointer pcounter = NULL;
	char *prev_layout_name = NULL;
	char *lbl_title = NULL;
	int counter = 0;

	if (group == 0) {
		*ln2cnt_map =
		    g_hash_table_new_full (g_str_hash, g_str_equal,
					   g_free, NULL);
	}

	/* Process layouts with repeating description */
	if (g_hash_table_lookup_extended
	    (*ln2cnt_map, layout_name, (gpointer *) & prev_layout_name,
	     &pcounter)) {
		/* "next" same description */
		gchar appendix[10] = "";
		gint utf8length;
		gunichar cidx;
		counter = GPOINTER_TO_INT (pcounter);
		/* Unicode subscript 2, 3, 4 */
		cidx = 0x2081 + counter;
		utf8length = g_unichar_to_utf8 (cidx, appendix);
		appendix[utf8length] = '\0';
		lbl_title = g_strconcat (layout_name, appendix, NULL);
	} else {
		/* "first" time this description */
		lbl_title = g_strdup (layout_name);
	}
	g_hash_table_insert (*ln2cnt_map, layout_name,
			     GINT_TO_POINTER (counter + 1));
	return lbl_title;
}

static GtkWidget *
gde2kbd_indicator_prepare_drawing (Gde2kbdIndicator * gki, int group)
{
	gpointer pimage;
	GdkPixbuf *image;
	GtkWidget *ebox;

	pimage = g_slist_nth_data (globals.images, group);
	ebox = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (ebox), FALSE);
	if (globals.ind_cfg.show_flags) {
		GtkWidget *flag;
		if (pimage == NULL)
			return NULL;
		image = GDK_PIXBUF (pimage);
		flag = gtk_drawing_area_new ();
		gtk_widget_add_events (GTK_WIDGET (flag),
				       GDK_BUTTON_PRESS_MASK);
		g_signal_connect (G_OBJECT (flag), "expose_event",
				  G_CALLBACK (flag_exposed), image);
		gtk_container_add (GTK_CONTAINER (ebox), flag);
	} else {
		char *lbl_title = NULL;
		char *layout_name = NULL;
		GtkWidget *align, *label;
		static GHashTable *ln2cnt_map = NULL;

		layout_name =
		    gde2kbd_indicator_extract_layout_name (group,
							globals.engine,
							&globals.kbd_cfg,
							globals.short_group_names,
							globals.full_group_names);

		lbl_title =
		    gde2kbd_indicator_create_label_title (group,
						       &ln2cnt_map,
						       layout_name);

		align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
		label = gtk_label_new (lbl_title);
		g_free (lbl_title);
		gtk_label_set_angle (GTK_LABEL (label), gki->priv->angle);

		if (group + 1 ==
		    xkl_engine_get_num_groups (globals.engine)) {
			g_hash_table_destroy (ln2cnt_map);
			ln2cnt_map = NULL;
		}

		gtk_container_add (GTK_CONTAINER (align), label);
		gtk_container_add (GTK_CONTAINER (ebox), align);

		gtk_container_set_border_width (GTK_CONTAINER (align), 2);
	}

	g_signal_connect (G_OBJECT (ebox),
			  "button_press_event",
			  G_CALLBACK (gde2kbd_indicator_button_pressed), gki);

	g_signal_connect (G_OBJECT (gki),
			  "key_press_event",
			  G_CALLBACK (gde2kbd_indicator_key_pressed), gki);

	/* We have everything prepared for that size */

	return ebox;
}

static void
gde2kbd_indicator_update_tooltips (Gde2kbdIndicator * gki)
{
	XklState *state = xkl_engine_get_current_state (globals.engine);
	gchar *buf;
	if (state == NULL || state->group < 0
	    || state->group >= g_strv_length (globals.full_group_names))
		return;

	buf = g_strdup_printf (globals.tooltips_format,
			       globals.full_group_names[state->group]);

	gde2kbd_indicator_set_tooltips (gki, buf);
	g_free (buf);
}

static void
gde2kbd_indicator_parent_set (GtkWidget * gki, GtkWidget * previous_parent)
{
	gde2kbd_indicator_update_tooltips (GDE2KBD_INDICATOR (gki));
}


void
gde2kbd_indicator_reinit_ui (Gde2kbdIndicator * gki)
{
	gde2kbd_indicator_cleanup (gki);
	gde2kbd_indicator_fill (gki);

	gde2kbd_indicator_set_current_page (gki);

	g_signal_emit_by_name (gki, "reinit-ui");
}

/* Should be called once for all widgets */
static void
gde2kbd_indicator_cfg_changed (GSettings *settings,
			       gchar     *key,
			       gpointer   user_data)
{
	xkl_debug (100,
		   "General configuration changed in GSettings - reiniting...\n");
	gde2kbd_desktop_config_load_from_gsettings (&globals.cfg);
	gde2kbd_desktop_config_activate (&globals.cfg);
	ForAllIndicators () {
		gde2kbd_indicator_reinit_ui (gki);
	} NextIndicator ();
}

/* Should be called once for all widgets */
static void
gde2kbd_indicator_ind_cfg_changed (GSettings *settings,
				   gchar     *key,
				   gpointer   user_data)
{
	xkl_debug (100,
		   "Applet configuration changed in GSettings - reiniting...\n");
	gde2kbd_indicator_config_load_from_gsettings (&globals.ind_cfg);
	gde2kbd_indicator_update_images ();
	gde2kbd_indicator_config_activate (&globals.ind_cfg);

	ForAllIndicators () {
		gde2kbd_indicator_reinit_ui (gki);
	} NextIndicator ();
}

static void
gde2kbd_indicator_load_group_names (const gchar ** layout_ids,
				 const gchar ** variant_ids)
{
	if (!gde2kbd_desktop_config_load_group_descriptions
	    (&globals.cfg, globals.registry, layout_ids, variant_ids,
	     &globals.short_group_names, &globals.full_group_names)) {
		/* We just populate no short names (remain NULL) -
		 * full names are going to be used anyway */
		gint i, total_groups =
		    xkl_engine_get_num_groups (globals.engine);
		globals.full_group_names =
		    g_new0 (gchar *, total_groups + 1);

		if (xkl_engine_get_features (globals.engine) &
		    XKLF_MULTIPLE_LAYOUTS_SUPPORTED) {
			gchar **lst = globals.kbd_cfg.layouts_variants;
			for (i = 0; *lst; lst++, i++) {
				globals.full_group_names[i] =
				    g_strdup ((char *) *lst);
			}
		} else {
			for (i = total_groups; --i >= 0;) {
				globals.full_group_names[i] =
				    g_strdup_printf ("Group %d", i);
			}
		}
	}
}

/* Should be called once for all widgets */
static void
gde2kbd_indicator_kbd_cfg_callback (Gde2kbdIndicator * gki)
{
	XklConfigRec *xklrec = xkl_config_rec_new ();
	xkl_debug (100,
		   "XKB configuration changed on X Server - reiniting...\n");

	gde2kbd_keyboard_config_load_from_x_current (&globals.kbd_cfg,
						  xklrec);
	gde2kbd_indicator_update_images ();

	g_strfreev (globals.full_group_names);
	globals.full_group_names = NULL;

	if (globals.short_group_names != NULL) {
		g_strfreev (globals.short_group_names);
		globals.short_group_names = NULL;
	}

	gde2kbd_indicator_load_group_names ((const gchar **) xklrec->layouts,
					 (const gchar **)
					 xklrec->variants);

	ForAllIndicators () {
		gde2kbd_indicator_reinit_ui (gki);
	} NextIndicator ();
	g_object_unref (G_OBJECT (xklrec));
}

/* Should be called once for all applets */
static void
gde2kbd_indicator_state_callback (XklEngine * engine,
			       XklEngineStateChange changeType,
			       gint group, gboolean restore)
{
	xkl_debug (150, "group is now %d, restore: %d\n", group, restore);

	if (changeType == GROUP_CHANGED) {
		ForAllIndicators () {
			xkl_debug (200, "do repaint\n");
			gde2kbd_indicator_set_current_page_for_group
			    (gki, group);
		}
		NextIndicator ();
	}
}


void
gde2kbd_indicator_set_current_page (Gde2kbdIndicator * gki)
{
	XklState *cur_state;
	cur_state = xkl_engine_get_current_state (globals.engine);
	if (cur_state->group >= 0)
		gde2kbd_indicator_set_current_page_for_group (gki,
							   cur_state->
							   group);
}

void
gde2kbd_indicator_set_current_page_for_group (Gde2kbdIndicator * gki, int group)
{
	xkl_debug (200, "Revalidating for group %d\n", group);

	gtk_notebook_set_current_page (GTK_NOTEBOOK (gki), group + 1);

	gde2kbd_indicator_update_tooltips (gki);
}

/* Should be called once for all widgets */
static GdkFilterReturn
gde2kbd_indicator_filter_x_evt (GdkXEvent * xev, GdkEvent * event)
{
	XEvent *xevent = (XEvent *) xev;

	xkl_engine_filter_events (globals.engine, xevent);
	switch (xevent->type) {
	case ReparentNotify:
		{
			XReparentEvent *rne = (XReparentEvent *) xev;

			ForAllIndicators () {
				GdkWindow *w =
				    gtk_widget_get_parent_window
				    (GTK_WIDGET (gki));

				/* compare the indicator's parent window with the even window */
				if (w != NULL
				    && GDK_WINDOW_XID (w) == rne->window) {
					/* if so - make it transparent... */
					xkl_engine_set_window_transparent
					    (globals.engine, rne->window,
					     TRUE);
				}
			}
			NextIndicator ()
		}
		break;
	}
	return GDK_FILTER_CONTINUE;
}


/* Should be called once for all widgets */
static void
gde2kbd_indicator_start_listen (void)
{
	gdk_window_add_filter (NULL, (GdkFilterFunc)
			       gde2kbd_indicator_filter_x_evt, NULL);
	gdk_window_add_filter (gdk_get_default_root_window (),
			       (GdkFilterFunc)
			       gde2kbd_indicator_filter_x_evt, NULL);

	xkl_engine_start_listen (globals.engine,
				 XKLL_TRACK_KEYBOARD_STATE);
}

/* Should be called once for all widgets */
static void
gde2kbd_indicator_stop_listen (void)
{
	xkl_engine_stop_listen (globals.engine, XKLL_TRACK_KEYBOARD_STATE);

	gdk_window_remove_filter (NULL, (GdkFilterFunc)
				  gde2kbd_indicator_filter_x_evt, NULL);
	gdk_window_remove_filter
	    (gdk_get_default_root_window (),
	     (GdkFilterFunc) gde2kbd_indicator_filter_x_evt, NULL);
}

static gboolean
gde2kbd_indicator_scroll (GtkWidget * gki, GdkEventScroll * event)
{
	/* mouse wheel events should be ignored, otherwise funny effects appear */
	return TRUE;
}

static void gde2kbd_indicator_init(Gde2kbdIndicator* gki)
{
	GtkWidget *def_drawing;
	GtkNotebook *notebook;

	if (!g_slist_length(globals.widget_instances))
	{
		gde2kbd_indicator_global_init();
	}

	gki->priv = g_new0 (Gde2kbdIndicatorPrivate, 1);

	notebook = GTK_NOTEBOOK (gki);

	xkl_debug (100, "Initiating the widget startup process for %p\n", gki);

	gtk_notebook_set_show_tabs (notebook, FALSE);
	gtk_notebook_set_show_border (notebook, FALSE);

	def_drawing =
	    gtk_image_new_from_icon_name ("process-stop",
				      GTK_ICON_SIZE_BUTTON);

	gtk_notebook_append_page (notebook, def_drawing,
				  gtk_label_new (""));

	if (globals.engine == NULL) {
		gde2kbd_indicator_set_tooltips (gki,
					     _
					     ("XKB initialization error"));
		return;
	}

	gde2kbd_indicator_set_tooltips (gki, NULL);

	gde2kbd_indicator_fill (gki);
	gde2kbd_indicator_set_current_page (gki);

	gtk_widget_add_events (GTK_WIDGET (gki), GDK_BUTTON_PRESS_MASK);

	/* append AFTER all initialization work is finished */
	globals.widget_instances =
	    g_slist_append (globals.widget_instances, gki);
}

static void
gde2kbd_indicator_finalize (GObject * obj)
{
	Gde2kbdIndicator *gki = GDE2KBD_INDICATOR (obj);
	xkl_debug (100,
		   "Starting the gde2-kbd-indicator widget shutdown process for %p\n",
		   gki);

	/* remove BEFORE all termination work is finished */
	globals.widget_instances =
	    g_slist_remove (globals.widget_instances, gki);

	gde2kbd_indicator_cleanup (gki);

	xkl_debug (100,
		   "The instance of gde2-kbd-indicator successfully finalized\n");

	g_free (gki->priv);

	G_OBJECT_CLASS (gde2kbd_indicator_parent_class)->finalize (obj);

	if (!g_slist_length (globals.widget_instances))
		gde2kbd_indicator_global_term ();
}

static void
gde2kbd_indicator_global_term (void)
{
	xkl_debug (100, "*** Last  Gde2kbdIndicator instance *** \n");
	gde2kbd_indicator_stop_listen ();

	gde2kbd_desktop_config_stop_listen (&globals.cfg);
	gde2kbd_indicator_config_stop_listen (&globals.ind_cfg);

	gde2kbd_indicator_config_term (&globals.ind_cfg);
	gde2kbd_keyboard_config_term (&globals.kbd_cfg);
	gde2kbd_desktop_config_term (&globals.cfg);

	g_object_unref (G_OBJECT (globals.registry));
	globals.registry = NULL;
	g_object_unref (G_OBJECT (globals.engine));
	globals.engine = NULL;
	xkl_debug (100, "*** Terminated globals *** \n");
}

static void
gde2kbd_indicator_class_init (Gde2kbdIndicatorClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	xkl_debug (100, "*** First Gde2kbdIndicator instance *** \n");

	memset (&globals, 0, sizeof (globals));

	/* Initing some global vars */
	globals.tooltips_format = "%s";

	/* Initing vtable */
	object_class->finalize = gde2kbd_indicator_finalize;

	widget_class->scroll_event = gde2kbd_indicator_scroll;
	widget_class->parent_set = gde2kbd_indicator_parent_set;

	/* Signals */
	g_signal_new ("reinit-ui", GDE2KBD_TYPE_INDICATOR,
		      G_SIGNAL_RUN_LAST,
		      G_STRUCT_OFFSET (Gde2kbdIndicatorClass, reinit_ui),
		      NULL, NULL, gde2kbd_indicator_VOID__VOID,
		      G_TYPE_NONE, 0);
}

static void
gde2kbd_indicator_global_init (void)
{
	XklConfigRec *xklrec = xkl_config_rec_new ();

	globals.engine = xkl_engine_get_instance(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()));

	if (globals.engine == NULL)
	{
		xkl_debug (0, "Libxklavier initialization error");
		return;
	}

	g_signal_connect (globals.engine, "X-state-changed",
			  G_CALLBACK (gde2kbd_indicator_state_callback),
			  NULL);
	g_signal_connect (globals.engine, "X-config-changed",
			  G_CALLBACK (gde2kbd_indicator_kbd_cfg_callback),
			  NULL);

	gde2kbd_desktop_config_init (&globals.cfg, globals.engine);
	gde2kbd_keyboard_config_init (&globals.kbd_cfg, globals.engine);
	gde2kbd_indicator_config_init (&globals.ind_cfg, globals.engine);

	gde2kbd_desktop_config_start_listen (&globals.cfg,
					  (GCallback)
					  gde2kbd_indicator_cfg_changed,
					  NULL);
	gde2kbd_indicator_config_start_listen (&globals.ind_cfg,
					    (GCallback)
					    gde2kbd_indicator_ind_cfg_changed,
					    NULL);

	gde2kbd_desktop_config_load_from_gsettings (&globals.cfg);
	gde2kbd_desktop_config_activate (&globals.cfg);

	globals.registry =
	    xkl_config_registry_get_instance (globals.engine);
	xkl_config_registry_load (globals.registry,
				  globals.cfg.load_extra_items);

	gde2kbd_keyboard_config_load_from_x_current (&globals.kbd_cfg,
						  xklrec);

	gde2kbd_indicator_config_load_from_gsettings (&globals.ind_cfg);
	gde2kbd_indicator_update_images ();
	gde2kbd_indicator_config_activate (&globals.ind_cfg);

	gde2kbd_indicator_load_group_names ((const gchar **) xklrec->layouts,
					 (const gchar **)
					 xklrec->variants);
	g_object_unref (G_OBJECT (xklrec));

	gde2kbd_indicator_start_listen ();

	xkl_debug (100, "*** Inited globals *** \n");
}

GtkWidget *
gde2kbd_indicator_new (void)
{
	return
	    GTK_WIDGET (g_object_new (gde2kbd_indicator_get_type (), NULL));
}

void
gde2kbd_indicator_set_parent_tooltips (Gde2kbdIndicator * gki, gboolean spt)
{
	gki->priv->set_parent_tooltips = spt;
	gde2kbd_indicator_update_tooltips (gki);
}

void
gde2kbd_indicator_set_tooltips_format (const gchar format[])
{
	globals.tooltips_format = format;
	ForAllIndicators ()
	    gde2kbd_indicator_update_tooltips (gki);
	NextIndicator ()
}

/**
 * gde2kbd_indicator_get_xkl_engine:
 *
 * Returns: (transfer none): The engine shared by all Gde2kbdIndicator objects
 */
XklEngine *
gde2kbd_indicator_get_xkl_engine ()
{
	return globals.engine;
}

/**
 * gde2kbd_indicator_get_group_names:
 *
 * Returns: (transfer none) (array zero-terminated=1): List of group names
 */
gchar **
gde2kbd_indicator_get_group_names ()
{
	return globals.full_group_names;
}

gchar *
gde2kbd_indicator_get_image_filename (guint group)
{
	if (!globals.ind_cfg.show_flags)
		return NULL;
	return gde2kbd_indicator_config_get_images_file (&globals.ind_cfg,
						      &globals.kbd_cfg,
						      group);
}

gdouble
gde2kbd_indicator_get_max_width_height_ratio (void)
{
	gdouble rv = 0.0;
	GSList *ip = globals.images;
	if (!globals.ind_cfg.show_flags)
		return 0;
	while (ip != NULL) {
		GdkPixbuf *img = GDK_PIXBUF (ip->data);
		gdouble r =
		    1.0 * gdk_pixbuf_get_width (img) /
		    gdk_pixbuf_get_height (img);
		if (r > rv)
			rv = r;
		ip = ip->next;
	}
	return rv;
}

void
gde2kbd_indicator_set_angle (Gde2kbdIndicator * gki, gdouble angle)
{
	gki->priv->angle = angle;
}
