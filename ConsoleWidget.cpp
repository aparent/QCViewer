#include "ConsoleWidget.h"
#include "window.h"
#include <iostream>
#include <sstream>
using namespace std;

ConsoleWidget::ConsoleWidget () {
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

void ConsoleWidget::set_window(void* w){
	window = w;
}

void ConsoleWidget::eval () {
  Glib::ustring text = "> " + entry.get_text () + "\n";
  log.get_buffer()->insert (log.get_buffer()->end(), text);
	REPLInterpreter::evalTerm result = interp.runLine(entry.get_text ());
	if (result.error){
		cout << "An error occurred" <<endl;
	}
	stringstream ss;
	switch (result.type){
		case INT:
			ss << result.value.INT << endl;
  		log.get_buffer()->insert (log.get_buffer()->end(), ss.str());
			break;
		case FLOAT:
			ss << result.value.FLOAT << endl;
  		log.get_buffer()->insert (log.get_buffer()->end(), ss.str());
			break;
		case COMPLEX:
			ss << *result.value.COMPLEX << endl;
  		log.get_buffer()->insert (log.get_buffer()->end(), ss.str());
			break;
		case MESSAGE:
			if (result.value.MESSAGE == REPLInterpreter::SHOW_STATE){
				((QCViewer*)window)->load_state(interp.Sim_State);
			}
			break;
		default:
  		log.get_buffer()->insert (log.get_buffer()->end(), "Unrecognized Type\n");
	}
  entry.set_text ("");
}
