using Gtk;

namespace Ulti {

public class ActionMenu : Gtk.Window {
	
	private Box box = new Box (Orientation.VERTICAL, 3);
	private Button btn_shutdown = new Button ();
	private Button btn_reboot = new Button ();
	private Button btn_preferences = new Button ();
	private Label label = new Label ("IceBerg OS Action Menu");
	private GLib.SubprocessLauncher processL = new GLib.SubprocessLauncher (GLib.SubprocessFlags.NONE);
	
	public void appear () {
		this.show_all ();
	}

	public void vanish () {
		this.hide ();
	}
	
	private void shutdown () {
		try {
			processL.spawnv ({"shutdown","-P","now",null});
		} catch (Error e) {
			stderr.printf ("Failed to shutdown: %s\n", e.message);
		}
	}
	
	private void reboot () {
		try {
			processL.spawnv ({"shutdown","-r","now",null});
		} catch (Error e) {
			stderr.printf ("Failed to reboot: %s\n", e.message);
		}
	}
	
	private void preferences () {
		//UltiPanel.post.Launch.active = false;
		//SESDE.prwin.appear ();
	}
	
	public ActionMenu () {
		this.set_decorated (false);
		this.set_skip_pager_hint (true);
		this.set_skip_taskbar_hint (true);
		this.set_keep_above (true);
		this.stick ();
		
		this.set_position (Gtk.WindowPosition.CENTER_ALWAYS);
		this.maximize ();
		
		btn_shutdown.set_label ("Shutdown");
		btn_shutdown.clicked.connect (shutdown);
		btn_reboot.set_label ("Reboot");
		btn_reboot.clicked.connect (reboot);
		btn_preferences.set_label ("Preferences");
		btn_preferences.clicked.connect (preferences);
		
		box.pack_start (label, true, true, 0);
		box.pack_start (btn_shutdown, true, true, 0);
		box.pack_start (btn_reboot, true, true, 0);
		box.pack_start (btn_preferences, true, true, 0);
	
		this.add (box);
		this.border_width = 3;
	}

}

public class Ultipost : Gtk.Bin {
	
	public Gtk.ToggleButton Launch = new Gtk.ToggleButton.with_label ("Launcher");
	public Gtk.ToggleButton Action = new Gtk.ToggleButton ();
	
	private Gtk.Box box = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
	
	private Label time_dis = new Label ("");
	
	public UltiPanel p;
	public ActionMenu action = new ActionMenu ();
	
	private void timer () {
		GLib.DateTime now = new GLib.DateTime.now_local ();
		string t = now.format("%H:%M");
		//time_dis.label = t;
		Action.label = t;
	}
	
	private void launch_toggle () {
		p.has_activated = true;
		if (UltiPanel.launcher.visible) {
			UltiPanel.launcher.vanish ();
		} else {
			UltiPanel.launcher.appear ();
		}
	}
	
	private void action_toggle () {
		if (action.visible) {
			action.vanish ();
		} else {
			action.appear ();
		}
	}
	
	public Ultipost (Gtk.Widget BtnTask, UltiPanel panel) {
		p = panel;
		this.add (box);
		box.pack_start (BtnTask, false, false, 0);
		box.pack_start (Launch, false, false, 0);
		box.pack_end (Action, false, false, 0);
		Launch.toggled.connect (launch_toggle);
		Action.toggled.connect (action_toggle);
		
		box.pack_end (time_dis, false, false, 0);
		
		GLib.Timeout.add (1000, (GLib.SourceFunc) timer);
		timer ();
		
		this.show_all ();
	}
	
}

}
