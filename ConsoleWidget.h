#ifndef __CONSOLEWIDGET__INCLUDED
#define __CONSOLEWIDGET__INCLUDED
#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>
#include <vector>
#include <QCLang/REPLInterpreter.h>

class ConsoleWidget : public Gtk::VBox {
public:
  ConsoleWidget ();
	void set_window(void*);
protected:
  void eval ();
private:
	REPLInterpreter interp;
  Gtk::Entry entry;
	void* window;
  Gtk::TextView log;
//  Gtk::TextBuffer logbuffer;
};

#endif
