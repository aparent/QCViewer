#ifndef WINDOW__INCLUDED
#define WINDOW__INCLUDED

#include <gtkmm.h>
#include "circuit_widget.h"

struct window_t : public Gtk::Window {
  public:
    window_t ();
    virtual ~window_t ();

  protected:
    /* Signal handlers */
    void on_quit ();
    void on_about ();

    /* Main layout */
    Gtk::VBox gtk_layout_vbox;
    Gtk::HBox gtk_layout_hbox;

    /* Custom widgets */
    circuit_widget_t circuit_widget;

    /* Menubar and toolbar */
    Glib::RefPtr<Gtk::UIManager> gtk_ui_manager;
    Glib::RefPtr<Gtk::ActionGroup> gtk_action_group;

  private:
    void init_menu ();
};

#endif
