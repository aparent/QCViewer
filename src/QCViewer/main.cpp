#include "window.h"
#include <gates/UGateLookup.h>
#include <iostream>

int main (int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    UGateSetup();
    init_fonts();
    QCViewer window;
    window.set_default_size (800,600);
    std::cerr << "Running window\n";
    Gtk::Main::run(window);


    return EXIT_SUCCESS;
}
