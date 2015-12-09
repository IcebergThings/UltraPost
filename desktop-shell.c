/*
 * Copyright (c) 2013 Tiago Vignatti
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>

 #include <linux/input.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "Ultiway.h"
#include "desktop-shell-client-protocol.h"

extern char **environ; /* defined by libc */

gchar *filename = "/home/bobcao3/SESDE/background.JPG";

struct element {
	GtkWidget *window;
	GdkPixbuf *pixbuf;
	struct wl_surface *surface;
};

struct desktop {
	struct wl_display *display;
	struct wl_registry *registry;
	struct desktop_shell *shell;
	struct wl_output *output;

	GdkDisplay *gdk_display;

	struct element *background;
	struct element *panel;
	
	int32_t width;
	int32_t height;
	
	UltiUltiPanel *ultip;
};

static void
desktop_shell_configure(void *data,
		struct desktop_shell *desktop_shell,
		uint32_t edges,
		struct wl_surface *surface,
		int32_t width, int32_t height)
{
	struct desktop *desktop = data;

	gtk_widget_set_size_request (desktop->background->window,
				     width, height);

	gtk_widget_set_size_request (desktop->panel->window, width, 24);
	
	desktop->width = width;
	desktop->height = height;
}

static void
desktop_shell_prepare_lock_surface(void *data,
		struct desktop_shell *desktop_shell)
{
}

static void
desktop_shell_grab_cursor(void *data, struct desktop_shell *desktop_shell,
		uint32_t cursor)
{
}

static const struct desktop_shell_listener listener = {
	desktop_shell_configure,
	desktop_shell_prepare_lock_surface,
	desktop_shell_grab_cursor
};

static void
do_exposay (GtkWidget *widget, struct desktop *desktop)
{
	if (desktop->ultip->has_activated)
		desktop_shell_exposay_launch(desktop->shell);
}

static void
panel_create(struct desktop *desktop)
{
	GdkWindow *gdk_window;
	struct element *panel;
	GtkWidget *button;

	button = gtk_button_new_with_label ("❐");
	g_signal_connect (button, "clicked", G_CALLBACK (do_exposay), desktop);

	panel = malloc(sizeof *panel);
	memset(panel, 0, sizeof *panel);

	panel->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(panel->window), "Ultiway");
	gtk_window_set_decorated(GTK_WINDOW(panel->window), FALSE);
	gtk_window_resize(GTK_WINDOW(panel->window), 0,24);
	gtk_widget_realize(panel->window);

	desktop->ultip = ulti_ulti_panel_new (button);
	UltiUltiPanel *p = desktop->ultip;
	gtk_container_add (GTK_CONTAINER (panel->window), GTK_WIDGET(p));

	gdk_window = gtk_widget_get_window(panel->window);
	gdk_wayland_window_set_use_custom_surface(gdk_window);

	panel->surface = gdk_wayland_window_get_wl_surface(gdk_window);
	desktop_shell_set_user_data(desktop->shell, desktop);
	desktop_shell_set_panel(desktop->shell, desktop->output,
				panel->surface);
				
	gtk_widget_show_all(panel->window);

	desktop->panel = panel;
}

/* Expose callback for the drawing area */
static gboolean
draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	struct desktop *desktop = data;

	gdk_cairo_set_source_pixbuf (cr, desktop->background->pixbuf, 0, 0);
	cairo_paint (cr);

	return TRUE;
}

/* Destroy handler for the window */
static void
destroy_cb (GObject *object, gpointer data)
{
	gtk_main_quit ();
}

static void
background_create(struct desktop *desktop)
{
	GdkWindow *gdk_window;
	struct element *background;

	background = malloc(sizeof *background);
	memset(background, 0, sizeof *background);

	/* TODO: get the "right" directory */
	background->pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
	if (!background->pixbuf) {
		g_message ("Could not load background.");
		exit (EXIT_FAILURE);
	}
	background->pixbuf = gdk_pixbuf_scale_simple (background->pixbuf, desktop->width, desktop->height, GDK_INTERP_BILINEAR);

	background->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect (background->window, "destroy",
			  G_CALLBACK (destroy_cb), NULL);

	g_signal_connect (background->window, "draw",
			  G_CALLBACK (draw_cb), desktop);

	gtk_window_set_title(GTK_WINDOW(background->window), "Ultiway");
	gtk_window_set_decorated(GTK_WINDOW(background->window), FALSE);
	gtk_widget_realize(background->window);

	gdk_window = gtk_widget_get_window(background->window);
	gdk_wayland_window_set_use_custom_surface(gdk_window);

	background->surface = gdk_wayland_window_get_wl_surface(gdk_window);
	desktop_shell_set_user_data(desktop->shell, desktop);
	desktop_shell_set_background(desktop->shell, desktop->output,
		background->surface);

	desktop->background = background;

	gtk_widget_show_all(background->window);
}

static void
registry_handle_global(void *data, struct wl_registry *registry,
		uint32_t name, const char *interface, uint32_t version)
{
	struct desktop *d = data;

	if (!strcmp(interface, "desktop_shell")) {
		d->shell = wl_registry_bind(registry, name,
				&desktop_shell_interface, 1);
		desktop_shell_add_listener(d->shell, &listener, d);
	} else if (!strcmp(interface, "wl_output")) {

		/* TODO: create multiple outputs */
		d->output = wl_registry_bind(registry, name,
					     &wl_output_interface, 1);
	}
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
		uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global,
	registry_handle_global_remove
};

static void
sigchild_handler(int s)
{
	int status;
	pid_t pid;

	while (pid = waitpid(-1, &status, WNOHANG), pid > 0)
		fprintf(stderr, "child %d exited\n", pid);
}

static void
grab_surface_create(struct desktop *desktop)
{
	struct wl_surface *s;

	GtkWidget *a = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_realize (a);
	gtk_widget_hide (a);
	GdkWindow *gdk_window = gtk_widget_get_window(a);
	gdk_wayland_window_set_use_custom_surface(gdk_window);
	s = gdk_wayland_window_get_wl_surface(gdk_window);

	desktop_shell_set_grab_surface(desktop->shell, s);
}

static void
css_setup (struct desktop *desktop)
{
  GtkCssProvider *provider;
  GError *error = NULL;

  provider = gtk_css_provider_new ();

  if (!gtk_css_provider_load_from_path (provider, "/home/bobcao3/SESDE/Wayland/style.css", &error))
    {
      g_warning ("Failed to load CSS file: %s", error->message);
      g_clear_error (&error);
      return;
    }

  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
      GTK_STYLE_PROVIDER (provider), 600);
}

int
main(int argc, char *argv[])
{
	struct desktop *desktop;
	desktop = malloc(sizeof *desktop);

	desktop->output = NULL;
	desktop->shell = NULL;

	gtk_init(&argc, &argv);

	desktop->gdk_display = gdk_display_get_default();
	desktop->display =
		gdk_wayland_display_get_wl_display(desktop->gdk_display);
	if (desktop->display == NULL) {
		fprintf(stderr, "failed to get display: %m\n");
		return -1;
	}

	desktop->registry = wl_display_get_registry(desktop->display);
	wl_registry_add_listener(desktop->registry,
			&registry_listener, desktop);

	signal(SIGCHLD, sigchild_handler);

	/* Wait until we have been notified about the compositor and shell
	 * objects */
	while (!desktop->output || !desktop->shell)
		wl_display_roundtrip (desktop->display);

	grab_surface_create(desktop);

	GdkScreen *screen = gdk_display_get_default_screen (desktop->gdk_display);
	desktop->width = gdk_screen_get_width (screen);
	desktop->height = gdk_screen_get_height (screen);
	
	css_setup (desktop);

	panel_create(desktop);
	background_create(desktop);
	
	gtk_main();

	/* TODO cleanup */
	return EXIT_SUCCESS;
}
	
