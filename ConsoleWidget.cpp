#include "ConsoleWidget.h"
#include "window.h"
#include <iostream>
using namespace std;

ConsoleWidget::ConsoleWidget (void* w) : window(w)  {
  log.set_editable (false);
  log.set_cursor_visible (false);
  log.set_size_request (0, 100);
	//log.set_buffer (logbuffer);
  entry.set_has_frame (false);
  entry.signal_activate ().connect(sigc::mem_fun(*this, &ConsoleWidget::eval));
  pack_start (log, Gtk::PACK_EXPAND_WIDGET);
  pack_start (entry, Gtk::PACK_SHRINK);

  show_all_children ();
}



void ConsoleWidget::eval () {
  Glib::ustring text = "> " + entry.get_text () + "\n";
  log.get_buffer()->insert (log.get_buffer()->end(), text);
	
	string result = interp.runLine(entry.get_text ());


  entry.set_text ("");
}
