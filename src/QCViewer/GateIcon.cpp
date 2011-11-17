#include <iostream>

#include "GateIcon.h"
#include "draw_internal.h"

GateIcon::GateIcon () : type(DEFAULT)
{
    set_size_request (20,20);
}

GateIcon::GateIcon (GateType t) : type(t)
{
    set_size_request (20,20);
}

GateIcon::GateIcon (string s) : type(DEFAULT), symbol(s)
{
    set_size_request (20,20);
}

bool GateIcon::on_expose_event(GdkEventExpose* event)
{
    (void)event; // placate compiler..
    Glib::RefPtr<Gdk::Window> window = get_window();
    if(window) {
        double scale = 0.90; // drawing to the borders tends to look bad in buttons
        Gtk::Allocation allocation = get_allocation();
        const double width = allocation.get_width();
        const double height = allocation.get_height();
        Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
        const double xc = width/2.0/scale;
        const double yc = height/2.0/scale;
        cr->scale (scale, scale);
        switch (type) {
        case NOT:
            drawNOT (cr->cobj(), xc, yc, min(width/2.0, height/2.0),false);
            break;
        case R:
            drawShowRotation (cr->cobj(), xc, yc, min(width/2.0, height/2.0));
            break;
        case SWAP:
            drawShowFred (cr->cobj(), width/scale, height/scale);
            break;
        case DEFAULT:
            drawShowU (cr->cobj (), xc, yc, min(width, height), symbol);
        }
    }
    return true;
}
