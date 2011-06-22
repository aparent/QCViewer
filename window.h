#ifndef WINDOW__INCLUDED
#define WINDOW__INCLUDED

#include <gtkmm.h>
#include <vector>
#include "circuitwidget.h"
#include "stateWidget.h"
#include "GateIcon.h"

class QCViewer : public Gtk::Window
{
public:
  QCViewer ();
  virtual ~QCViewer();

  void set_selection (int);
protected:
  void dummy(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData&, guint, guint);
  void setup_gate_button (Gtk::Button&, GateIcon&, vector<Gtk::TargetEntry> &);

  // Signal handlers:
  void on_menu_file_open_circuit ();
  void on_menu_file_open_arch ();
  void on_menu_mode_edit ();
  void on_menu_mode_simulate ();
//  void on_menu_file_save ();
  void on_menu_file_quit ();
  void on_menu_save_png ();
  void on_menu_save_svg ();
  void on_menu_save_circuit ();
  void on_menu_options_parallel ();
  void on_menu_options_arch ();
  void on_menu_zoom_in ();
  void on_menu_zoom_out ();
  void on_menu_zoom_100 ();
  void on_menu_run ();
  void on_menu_step();
  void on_menu_reset();
  void on_menu_delete();
  void unimplemented ();
  void on_menu_simulate_show_stateView ();
  void on_menu_load_state ();

//  void on_architecture_load ();

//  void on_menu_simulation_reset ();
//  void on_menu_simulation_run ();
//  void on_menu_simulation_step ();

//  void on_options_arch_warnings ();
//  void on_options_parallel_guides ();

  // Child widgets:
  Gtk::HBox m_VisBox;
  std::vector <StateViewWidget*> viz;
  Gtk::VPaned m_EditVisPane;
  Gtk::HBox m_hbox;
  Gtk::VBox m_vbox;
  Gtk::TextView m_cmdOut;
  Gtk::Entry m_cmdIn;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  CircuitWidget c;
  //stateView sView;
  Gtk::Statusbar m_statusbar;

  Gtk::Widget* m_EditToolbar;
  Gtk::Widget* m_SimulateToolbar;


  // Editing sidebar
  Gtk::VBox        m_EditSidebar;
  Gtk::Frame       m_GatesFrame;
  Gtk::Table       m_GatesTable;
  Gtk::Button      btn_H, btn_X, btn_Y, btn_Z, btn_NOT, btn_R, btn_SWAP;
  GateIcon NOTicon, Hicon, Xicon, Yicon, Zicon, Ricon, SWAPicon;
  Gtk::Frame       m_PropFrame;
  Gtk::Table       m_PropTable;
  Gtk::Button      btn_delete;

  Gtk::Frame       m_SimulationFrame;
  Gtk::Table       m_SimulationTable;

  void set_selected (int i);

  Gtk::ToggleButton btn_editbreakpoints, btn_editcontrols;

  Gtk::ToolItemGroup* group_gates;
  Gtk::ToolItemGroup* group_prop;
  Gtk::ToolItemGroup* group_misc;



private:
  void update_mode ();

  State *state;
  bool drawparallel;
  bool drawarch;
  int selection;
};

#endif
