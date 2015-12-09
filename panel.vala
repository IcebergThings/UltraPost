using Gtk;

namespace Ulti {

public class UltiPanel : Gtk.Bin {
	
	private Gtk.Box vbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 1);
	public static Ultipost post;
	public static UltiLaunch launcher = new UltiLaunch ();
	
	public bool has_activated = false;
	
	public override bool draw(Cairo.Context cr)
    {
        var st = this.get_style_context();

        st.render_background(cr, 0, 0, get_allocated_width(), get_allocated_height());

        return base.draw(cr);
    }
	
	public UltiPanel (Gtk.Widget btn) {
		this.add (vbox);
		post = new Ultipost (btn, this);
		vbox.pack_start (post, false, true, 0);
		launcher.vanish ();
		
		this.show ();
	}

}

}
