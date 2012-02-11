#ifndef CIRCUIT_WIDGET__INCLUDED
#define CIRCUIT_WIDGET__INCLUDED

#include <gtkmm/drawingarea.h>
#include "circuit.h"

struct circuit_widget_t : public Gtk::DrawingArea {
public:
    circuit_widget_t ();
    ~circuit_widget_t ();

protected:
    /* Signal handlers */
    virtual bool on_expose_event (GdkEventExpose*);

private:
    circuit_t* circuit;
};

#endif
