#ifndef DESKTOP_CLIENT_PROTOCOL_H
#define DESKTOP_CLIENT_PROTOCOL_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "wayland-client.h"

struct wl_client;
struct wl_resource;

struct desktop_shell;
struct screensaver;
struct wl_output;
struct wl_surface;

extern const struct wl_interface desktop_shell_interface;
extern const struct wl_interface screensaver_interface;

#ifndef DESKTOP_SHELL_CURSOR_ENUM
#define DESKTOP_SHELL_CURSOR_ENUM
enum desktop_shell_cursor {
	DESKTOP_SHELL_CURSOR_NONE = 0,
	DESKTOP_SHELL_CURSOR_RESIZE_TOP = 1,
	DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM = 2,
	DESKTOP_SHELL_CURSOR_ARROW = 3,
	DESKTOP_SHELL_CURSOR_RESIZE_LEFT = 4,
	DESKTOP_SHELL_CURSOR_RESIZE_TOP_LEFT = 5,
	DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM_LEFT = 6,
	DESKTOP_SHELL_CURSOR_MOVE = 7,
	DESKTOP_SHELL_CURSOR_RESIZE_RIGHT = 8,
	DESKTOP_SHELL_CURSOR_RESIZE_TOP_RIGHT = 9,
	DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM_RIGHT = 10,
	DESKTOP_SHELL_CURSOR_BUSY = 11,
};
#endif /* DESKTOP_SHELL_CURSOR_ENUM */

#ifndef DESKTOP_SHELL_PANEL_POSITION_ENUM
#define DESKTOP_SHELL_PANEL_POSITION_ENUM
enum desktop_shell_panel_position {
	DESKTOP_SHELL_PANEL_POSITION_TOP = 0,
	DESKTOP_SHELL_PANEL_POSITION_BOTTOM = 1,
	DESKTOP_SHELL_PANEL_POSITION_LEFT = 2,
	DESKTOP_SHELL_PANEL_POSITION_RIGHT = 3,
};
#endif /* DESKTOP_SHELL_PANEL_POSITION_ENUM */

#ifndef DESKTOP_SHELL_ERROR_ENUM
#define DESKTOP_SHELL_ERROR_ENUM
enum desktop_shell_error {
	DESKTOP_SHELL_ERROR_INVALID_ARGUMENT = 0,
};
#endif /* DESKTOP_SHELL_ERROR_ENUM */

/**
 * desktop_shell - create desktop widgets and helpers
 * @configure: (none)
 * @prepare_lock_surface: tell the client to create, set the lock surface
 * @grab_cursor: tell client what cursor to show during a grab
 *
 * Traditional user interfaces can rely on this interface to define the
 * foundations of typical desktops. Currently it's possible to set up
 * background, panels and locking surfaces.
 */
struct desktop_shell_listener {
	/**
	 * configure - (none)
	 * @edges: (none)
	 * @surface: (none)
	 * @width: (none)
	 * @height: (none)
	 */
	void (*configure)(void *data,
			  struct desktop_shell *desktop_shell,
			  uint32_t edges,
			  struct wl_surface *surface,
			  int32_t width,
			  int32_t height);
	/**
	 * prepare_lock_surface - tell the client to create, set the lock
	 *	surface
	 *
	 * Tell the client we want it to create and set the lock surface,
	 * which is a GUI asking the user to unlock the screen. The lock
	 * surface is announced with 'set_lock_surface'. Whether or not the
	 * client actually implements locking, it MUST send 'unlock'
	 * request to let the normal desktop resume.
	 */
	void (*prepare_lock_surface)(void *data,
				     struct desktop_shell *desktop_shell);
	/**
	 * grab_cursor - tell client what cursor to show during a grab
	 * @cursor: (none)
	 *
	 * This event will be sent immediately before a fake enter event
	 * on the grab surface.
	 */
	void (*grab_cursor)(void *data,
			    struct desktop_shell *desktop_shell,
			    uint32_t cursor);
};

static inline int
desktop_shell_add_listener(struct desktop_shell *desktop_shell,
			   const struct desktop_shell_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) desktop_shell,
				     (void (**)(void)) listener, data);
}

#define DESKTOP_SHELL_SET_BACKGROUND	0
#define DESKTOP_SHELL_SET_PANEL	1
#define DESKTOP_SHELL_SET_LOCK_SURFACE	2
#define DESKTOP_SHELL_UNLOCK	3
#define DESKTOP_SHELL_SET_GRAB_SURFACE	4
#define DESKTOP_SHELL_DESKTOP_READY	5
#define DESKTOP_SHELL_SET_PANEL_POSITION	6
#define DESKTOP_SHELL_EXPOSAY_LAUNCH	7

static inline void
desktop_shell_set_user_data(struct desktop_shell *desktop_shell, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) desktop_shell, user_data);
}

static inline void *
desktop_shell_get_user_data(struct desktop_shell *desktop_shell)
{
	return wl_proxy_get_user_data((struct wl_proxy *) desktop_shell);
}

static inline void
desktop_shell_destroy(struct desktop_shell *desktop_shell)
{
	wl_proxy_destroy((struct wl_proxy *) desktop_shell);
}

static inline void
desktop_shell_set_background(struct desktop_shell *desktop_shell, struct wl_output *output, struct wl_surface *surface)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_SET_BACKGROUND, output, surface);
}

static inline void
desktop_shell_set_panel(struct desktop_shell *desktop_shell, struct wl_output *output, struct wl_surface *surface)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_SET_PANEL, output, surface);
}

static inline void
desktop_shell_set_lock_surface(struct desktop_shell *desktop_shell, struct wl_surface *surface)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_SET_LOCK_SURFACE, surface);
}

static inline void
desktop_shell_unlock(struct desktop_shell *desktop_shell)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_UNLOCK);
}

static inline void
desktop_shell_set_grab_surface(struct desktop_shell *desktop_shell, struct wl_surface *surface)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_SET_GRAB_SURFACE, surface);
}

static inline void
desktop_shell_desktop_ready(struct desktop_shell *desktop_shell)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_DESKTOP_READY);
}

static inline void
desktop_shell_set_panel_position(struct desktop_shell *desktop_shell, uint32_t position)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_SET_PANEL_POSITION, position);
}

static inline void
desktop_shell_exposay_launch(struct desktop_shell *desktop_shell)
{
	wl_proxy_marshal((struct wl_proxy *) desktop_shell,
			 DESKTOP_SHELL_EXPOSAY_LAUNCH);
}

#define SCREENSAVER_SET_SURFACE	0

static inline void
screensaver_set_user_data(struct screensaver *screensaver, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) screensaver, user_data);
}

static inline void *
screensaver_get_user_data(struct screensaver *screensaver)
{
	return wl_proxy_get_user_data((struct wl_proxy *) screensaver);
}

static inline void
screensaver_destroy(struct screensaver *screensaver)
{
	wl_proxy_destroy((struct wl_proxy *) screensaver);
}

static inline void
screensaver_set_surface(struct screensaver *screensaver, struct wl_output *output, struct wl_surface *surface)
{
	wl_proxy_marshal((struct wl_proxy *) screensaver,
			 SCREENSAVER_SET_SURFACE, output, surface);
}

#ifdef  __cplusplus
}
#endif

#endif
