#ifndef GATEICON__INCLUDED
#define GATEICON__INCLUDED
#include <gtkmm.h>

class GateIcon : public Gtk::DrawingArea {
	public:
    GateIcon ();
	  enum GateType { NOT, H, X, Y, Z, R, SWAP };
		GateType type;
    virtual bool on_expose_event(GdkEventExpose* event);
};

#endif
