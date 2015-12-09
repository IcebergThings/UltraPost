nop:
	valac Launch.vala Ultipost.vala panel.vala -C -H Ultiway.h --pkg gtk+-3.0
	gcc *.c -o shell `pkg-config --cflags --libs gtk+-wayland-3.0 wayland-client gdk-wayland-3.0`
