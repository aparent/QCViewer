#include <iostream>
#include <sstream>
#include "window.h"
/*
circuit_t* make_circuit ()
{
  circuit_t* c = new circuit_t;
  c->add_qubit(qubit_t(""));
  c->add_qubit(qubit_t(""));
  c->add_qubit(qubit_t(""));

  matrix_gate_t* g = new matrix_gate_t("H");
  g->qubits.push_back(gate_t::qubit_usage_t(0, gate_t::qubit_usage_t::POSITIVE));
  g->qubits.push_back(gate_t::qubit_usage_t(2, gate_t::qubit_usage_t::TARGET));
  c->append_gate(g, false);
  g = new matrix_gate_t("NOT");
  g->qubits.push_back(gate_t::qubit_usage_t(1, gate_t::qubit_usage_t::TARGET));
  c->append_gate(g, false);
  g = new matrix_gate_t ("Z");
  g->qubits.push_back(gate_t::qubit_usage_t(2, gate_t::qubit_usage_t::TARGET));
  c->append_gate(g, true);
  std::cout << c->num_gates() << "\n";
  return c;
}
*/

window_t::window_t ()
{
    set_title("QCViewer");
    set_border_width(0);
    set_default_size(1000,1000);

    /* Main layout */
    add(gtk_layout_vbox);
    gtk_layout_vbox.pack_end(gtk_layout_hbox);
    gtk_layout_hbox.pack_end(circuit_widget);

    /* Menu and toolbar */
    init_menu ();
    Gtk::Widget* menubar = gtk_ui_manager->get_widget("/MenuBar");
    assert (menubar);
    gtk_layout_vbox.pack_start(*menubar, Gtk::PACK_SHRINK);


    show_all_children();
}

window_t::~window_t ()
{
}

void window_t::on_quit ()
{
    // TODO: prompt for save etc.
    hide ();
}

void window_t::on_about ()
{
    // TODO
}

void window_t::init_menu ()
{
    gtk_action_group = Gtk::ActionGroup::create();

    /* File */
    gtk_action_group->add(Gtk::Action::create("File", "File"));

    gtk_action_group->add(
        Gtk::Action::create("File_Quit", Gtk::Stock::QUIT, "Quit"),
        sigc::mem_fun(*this, &window_t::on_quit));

    /* About */
    gtk_action_group->add(Gtk::Action::create("Help", "Help"));

    gtk_action_group->add(
        Gtk::Action::create("Help_About", Gtk::Stock::ABOUT, "About"),
        sigc::mem_fun(*this, &window_t::on_about));

    gtk_ui_manager = Gtk::UIManager::create();
    gtk_ui_manager->insert_action_group(gtk_action_group);
    add_accel_group(gtk_ui_manager->get_accel_group());

    Glib::ustring ui_layout =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='File'>"
        "      <menuitem action='File_Quit'/>"
        "    </menu>"
        "    <menu action='Help'>"
        "      <menuitem action='Help_About'/>"
        "    </menu>"
        "  </menubar>"
        "</ui>";

    try {
        gtk_ui_manager->add_ui_from_string(ui_layout);
    } catch (const Glib::Error& ex) {
        std::stringstream reason;
        reason << "Building menubar/toolbar failed: " << ex.what();
        panic(reason.str());
    }

}
