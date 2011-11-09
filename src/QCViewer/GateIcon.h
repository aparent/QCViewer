#ifndef GATEICON__INCLUDED
#define GATEICON__INCLUDED
#include <gtkmm.h>
#include <string>

class GateIcon : public Gtk::DrawingArea {
    public:
    enum GateType { NOT, R, SWAP, DEFAULT };
    GateIcon ();
    GateIcon (std::string);
    GateIcon (GateType);
    GateType type;
    std::string symbol;
    virtual bool on_expose_event(GdkEventExpose* event);
};

#endif
