#include "circuit.h"
#include "window.h"
#include "draw.h"
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gates/UGateLookup.h>//XXX

int main (int argc, char *argv[]) {
	Gtk::Main kit(argc, argv);

	UGateSetup();//XXX
	init_fonts();

  QCViewer window;
  window.set_default_size (800,600);
	Gtk::Main::run(window);


	return EXIT_SUCCESS;
}
