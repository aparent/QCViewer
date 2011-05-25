#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm.h>
#include "circuitwidget.h"

class QCViewer : public Gtk::Window
{
public:
  QCViewer ();
  virtual ~QCViewer();

protected:

  // Signal handlers:
  // Our new improved on_button_clicked(). (see below)
  void on_button_clicked(Glib::ustring data);
	void on_menu_file_open_circuit ();
//	void on_menu_file_save ();
	void on_menu_file_quit ();
	void on_menu_save_png ();
  void unimplemented ();

//  void on_architecture_load ();

//	void on_menu_simulation_reset ();
//	void on_menu_simulation_run ();
//	void on_menu_simulation_step ();

//  void on_options_arch_warnings ();
//	void on_options_parallel_guides ();

  // Child widgets:
  Gtk::VBox m_vbox;
	Gtk::TextView m_cmdOut;
	Gtk::Entry m_cmdIn;
	Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;


  Gtk::Button m_button1, m_button2;
  CircuitWidget c;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
