#ifndef __CONSOLEWIDGET__INCLUDED
#define __CONSOLEWIDGET__INCLUDED
#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>
#include <vector>


class ConsoleWidget : public Gtk::VBox {
public:
  ConsoleWidget ();

protected:
  void eval ();
private:
  Gtk::Entry entry;
  Gtk::TextView log;
//  Gtk::TextBuffer logbuffer;
};

#endif
