#ifndef WINDOW__INCLUDED
#define WINDOW__INCLUDED

#include <gtkmm.h>
#include <vector>
#include <stdint.h>
#include "circuitwidget.h"
#include "stateWidget.h"
#include "GateIcon.h"

class QCViewer : public Gtk::Window
{
public:
  QCViewer ();
  virtual ~QCViewer();

  void set_selection (vector<uint32_t>);
	void load_state (State* s);
protected:
  void dummy(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData&, guint, guint);
  void setup_gate_button (Gtk::Button*, GateIcon*, vector<Gtk::TargetEntry> &);

  // Signal handlers:
  void delete_loop();
  void set_loop_label ();
  void set_loop_iter ();
  
void on_menu_file_open_circuit ();
  void on_menu_file_open_arch ();
  void on_menu_mode_edit ();
  void on_menu_new ();
  void on_menu_about ();
  void on_menu_move ();
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
	void set_rval();
  void on_menu_zoom_100 ();
  void on_menu_run ();
  void on_menu_step();
  void on_menu_reset();
  void on_menu_delete();
	void set_raxis ();
  void unimplemented ();
  void on_menu_simulate_show_stateView ();
  void on_menu_load_state ();
	void on_menu_set_arch_LNN();
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
  Gtk::Statusbar m_statusbar;

  Gtk::Widget* m_EditToolbar;
  Gtk::Widget* m_SimulateToolbar;


  // Editing sidebar
  Gtk::VBox        m_EditSidebar;
  Gtk::Frame       m_GatesFrame;
  Gtk::Table       m_GatesTable;
  Gtk::Frame       m_FlowFrame;
  Gtk::Table       m_FlowTable;
  Gtk::Button      m_AddLoop;
  Gtk::Frame       m_EditLoop;
  Gtk::Table       m_EditLoopTable;
  Gtk::Label       m_EditLoopNameLbl;
  Gtk::Entry       m_EditLoopNameEntry;
  Gtk::Label       m_EditLoopIterLbl;
  Gtk::Entry       m_EditLoopIterEntry;
  Gtk::Button      m_EditLoopDelButton;

  std::vector<Gtk::Button*> gate_buttons;
  std::vector<GateIcon*>    gate_icons;  
  Gtk::Frame       m_PropFrame;
  Gtk::Table       m_PropTable;

  Gtk::Button      btn_delete;
  Gtk::Frame       m_RGateEditFrame;
  Gtk::Table       m_RGateEditTable;
  Gtk::RadioButton::Group m_RAxisGroup;
  Gtk::RadioButton btn_RX, btn_RY, btn_RZ;
  Gtk::Label       m_RValLabel;
  Gtk::Entry       m_RValEntry;

  Gtk::Frame       m_SimulationFrame;
  Gtk::Table       m_SimulationTable;

  Gtk::ToggleButton btn_editbreakpoints, btn_editcontrols;

  std::vector<Gtk::TargetEntry> listTargets;
private:
  void setup_menu_actions();
  void setup_menu_layout();
  void setup_gate_icons();
  void register_stock_items();
  void add_stock_item(const Glib::RefPtr<Gtk::IconFactory>& , const std::string& , const Glib::ustring& , const Glib::ustring& );
  void update_mode ();
  void add_loop();

  State *state;
  bool drawparallel;
  bool drawarch;
  bool panning;
  vector<uint32_t> selections;
};

#endif
