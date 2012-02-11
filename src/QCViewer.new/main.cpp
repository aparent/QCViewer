#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include "window.h"

int main (int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    window_t window;
    Gtk::Main::run(window);
    return 0;
}
