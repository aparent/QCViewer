#include "window.h"
#include <gtkmm/stock.h>
#include <iostream>
#include <string>
#include <state.h>
#include <dirac.h>

void QCViewer::setup_gate_button (Gtk::Button& btn, GateIcon& g, vector<Gtk::TargetEntry> &listTargets) {
  btn.set_image (g);
  btn.drag_source_set (listTargets);
  btn.signal_drag_data_get().connect(sigc::mem_fun(*this, &QCViewer::dummy));
}

void QCViewer::dummy(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& selection_data, guint, guint){
  selection_data.set(selection_data.get_target(), 8 /* 8 bits format */,
                     (const guchar*)"I'm Data!",
                     9 /* the length of I'm Data! in bytes */);
}

QCViewer::QCViewer() {
  breakpointmode = false;
  drawparallel = drawarch = false;
  set_title("QCViewer-v0.1");
  set_border_width(0);
  set_default_size(1000,1000);
  state = NULL;
  NOTicon.type = GateIcon::NOT;
  Hicon.type = GateIcon::H;
  Xicon.type = GateIcon::X;
  Yicon.type = GateIcon::Y;
  Zicon.type = GateIcon::Z;
  Ricon.type = GateIcon::R;
  SWAPicon.type = GateIcon::SWAP;

  add(m_vbox);
   m_vbox.pack_end(m_statusbar,Gtk::PACK_SHRINK);

  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("File", "File"));
  m_refActionGroup->add(Gtk::Action::create("Mode", "Mode"));
  m_refActionGroup->add(Gtk::Action::create("Circuit", "Circuit"));
  m_refActionGroup->add(Gtk::Action::create("Arch", "Architecture"));
  m_refActionGroup->add(Gtk::Action::create("Diagram", "Diagram"));

//  m_refActionGroup->add(Gtk::Action::create("ModeEdit", "Edit"), sigc::mem_fun (*this, &QCViewer::on_menu_mode_edit));
//  m_refActionGroup->add(Gtk::Action::create("ModeDelete", Gtk::Stock::DELETE, "Delete Gate"), sigc::mem_fun (*this, &QCViewer::on_menu_delete));

//  m_refActionGroup->add(Gtk::Action::create("ModeSimulate", "Simulate"), sigc::mem_fun (*this, &QCViewer::on_menu_mode_simulate));

  m_refActionGroup->add(Gtk::Action::create("CircuitNew", Gtk::Stock::NEW, "New", "Create new circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("ArchNew", Gtk::Stock::NEW, "New", "Create new architecture"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));

  m_refActionGroup->add(Gtk::Action::create("CircuitOpen", Gtk::Stock::OPEN, "Open", "Open a circuit file"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_file_open_circuit));
  m_refActionGroup->add(Gtk::Action::create("ArchOpen", Gtk::Stock::OPEN, "Open", "Open an architecture file"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_file_open_arch));

  m_refActionGroup->add(Gtk::Action::create("DiagramSave", Gtk::Stock::SAVE, "_Save",
                                            "Save the circuit diagram to an image file"));
  m_refActionGroup->add(Gtk::Action::create("CircuitSave", Gtk::Stock::SAVE, "Save", "Save circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("ArchSave", Gtk::Stock::SAVE, "Save", "Save architecture"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("DiagramSavePng", "P_NG",
                                            "Save circuit diagram as a Portable Network Graphics file"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_save_png));
  m_refActionGroup->add(Gtk::Action::create("DiagramSaveSvg", "S_VG",
                                            "Save circuit diagram as a Scalable Vector Graphics file"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_save_svg));
  m_refActionGroup->add(Gtk::Action::create("DiagramSavePs", "_Postscript", "Save circuit diagram as a Postscript file"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));


  m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT, "Quit"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_file_quit));
  m_refActionGroup->add(Gtk::Action::create("ZoomIn", Gtk::Stock::ZOOM_IN, "Zoom In"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_zoom_in));
  m_refActionGroup->add(Gtk::Action::create("ZoomOut", Gtk::Stock::ZOOM_OUT, "Zoom Out"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_zoom_out));
  m_refActionGroup->add(Gtk::Action::create("Zoom100", Gtk::Stock::ZOOM_100, "100%"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_zoom_100));
  m_refActionGroup->add(Gtk::ToggleAction::create ("Pan", "Pan"), sigc::mem_fun (*this, &QCViewer::on_menu_pan));

  m_refActionGroup->add(Gtk::Action::create("SimulateMenu", "Simulate"));
  m_refActionGroup->add(Gtk::Action::create ("SimulateLoad", Gtk::Stock::ADD, "Load state", "Enter a state for input into the circuit"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_load_state));
  m_refActionGroup->add(Gtk::Action::create ("SimulateRun", Gtk::Stock::GOTO_LAST, "Run", "Simulate the entire circuit"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_run));
  m_refActionGroup->add(Gtk::Action::create ("SimulateStep", Gtk::Stock::GO_FORWARD, "Step", "Advance the simulation through a single gate"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_step));
  m_refActionGroup->add(Gtk::Action::create ("SimulateReset", Gtk::Stock::STOP, "Reset", "Reset the simulation to the start of the circuit"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_reset));
  m_refActionGroup->add(Gtk::Action::create ("SimulateDisplay", "Display state"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_simulate_show_stateView));
  m_refActionGroup->add(Gtk::ToggleAction::create ("EditBreakpoints", "Edit Breakpoints"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_edit_breakpoints));

  m_refActionGroup->add(Gtk::Action::create("ArchitectureMenu", "Architecture"));
  m_refActionGroup->add(Gtk::ToggleAction::create ("DiagramParallel", "Show parallel guides"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_parallel));
  m_refActionGroup->add(Gtk::ToggleAction::create ("DiagramArch", Gtk::Stock::DIALOG_WARNING, "Show warnings", "Show architecture alignment warnings"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_arch));

  m_refActionGroup->add(Gtk::ToggleAction::create ("InsertTest", "Insert Hadamard"), sigc::mem_fun(*this, &QCViewer::on_menu_inserttest));


  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='File'>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='Circuit'>"
        "      <menuitem action='CircuitNew'/>"
        "      <menuitem action='CircuitOpen'/>"
        "      <menuitem action='CircuitSave'/>"
        "    </menu>"
        "    <menu action='Arch'>"
        "      <menuitem action='ArchNew'/>"
        "      <menuitem action='ArchOpen'/>"
        "      <menuitem action='ArchSave'/>"
        "    </menu>"
        "    <menu action='Diagram'>"
        "      <menu action='DiagramSave'>"
        "        <menuitem action='DiagramSavePng'/>"
        "        <menuitem action='DiagramSaveSvg'/>"
        "        <menuitem action='DiagramSavePs'/>"
        "        <separator/>"
        "      </menu>"
        "      <menuitem action='DiagramParallel'/>"
        "      <menuitem action='DiagramArch'/>"
        "    </menu>"
        "    <menu action='SimulateMenu'>"
        "      <menuitem action='SimulateDisplay'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='SimulateToolbar'>"
        "    <toolitem action='CircuitOpen'/>"
        "    <separator/>"
        "    <toolitem action='Zoom100'/>"
        "    <separator/>"
        "    <toolitem action='SimulateLoad'/>"
        "    <toolitem action='SimulateRun'/>"
        "    <toolitem action='SimulateStep'/>"
        "    <toolitem action='SimulateReset'/>"
        "    <separator/>"
        "    <toolitem action='EditBreakpoints'/>"
        "  </toolbar>"
        "</ui>";

  try
  {
    m_refUIManager->add_ui_from_string(ui_info);
  }
  catch(const Glib::Error& ex)
  {
    std::cerr << "building menus failed: " <<  ex.what();
  }
  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
  if(pMenubar)
    m_vbox.pack_start(*pMenubar, Gtk::PACK_SHRINK);
  m_SimulateToolbar = m_refUIManager->get_widget("/SimulateToolbar");

  m_GatesFrame.set_label ("Gates");
  m_GatesFrame.add (m_GatesTable);
  m_GatesTable.resize (2, 4);
  m_EditSidebar.pack_start (m_GatesFrame, Gtk::PACK_SHRINK);
  m_EditSidebar.set_homogeneous (false);

  vector<Gtk::TargetEntry> listTargets;
  listTargets.push_back(Gtk::TargetEntry ("STRING"));
  listTargets.push_back(Gtk::TargetEntry ("text/plain"));
  c.drag_dest_set (listTargets);

  setup_gate_button (btn_NOT, NOTicon, listTargets);
  setup_gate_button (btn_H, Hicon, listTargets);
  setup_gate_button (btn_X, Xicon, listTargets);
  setup_gate_button (btn_Y, Yicon, listTargets);
  setup_gate_button (btn_Z, Zicon, listTargets);
  setup_gate_button (btn_R, Ricon, listTargets);
  setup_gate_button (btn_SWAP, SWAPicon, listTargets);
  m_GatesTable.attach (btn_NOT,0,1,0,1);
  m_GatesTable.attach (btn_H,1,2,0,1);
  m_GatesTable.attach (btn_SWAP, 2,3,0,1);
  m_GatesTable.attach (btn_R, 3,4,0,1);
  m_GatesTable.attach (btn_X,0,1,1,2);
  m_GatesTable.attach (btn_Y,1,2,1,2);
  m_GatesTable.attach (btn_Z,2,3,1,2);
  m_GatesTable.set_homogeneous ();

  if (!m_SimulateToolbar) { cout << "warning failed to create toolbar" << endl; return; }
  m_vbox.pack_start(*m_SimulateToolbar, Gtk::PACK_SHRINK);
  m_SimulateToolbar->show ();
  c.set_window (this);
  c.show();
  m_VisBox.set_homogeneous ();
  m_hbox.pack_end (c);
  m_hbox.pack_start (m_EditSidebar, Gtk::PACK_SHRINK);
  m_EditVisPane.pack1 (m_hbox, true, true);
  m_EditVisPane.pack2 (m_VisBox, true, true);
  m_vbox.pack_start (m_EditVisPane);

//  m_vbox.pack_start (m_cmdOut);
//  m_vbox.pack_start (m_cmdIn, Gtk::PACK_SHRINK);
//  m_cmdOut.show();
//  m_cmdOut.set_editable (false);
//  m_cmdIn.show ();

  m_vbox.show();
  m_hbox.show();
  m_VisBox.show ();
  show_all_children ();
  show ();
}

QCViewer::~QCViewer() {}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.

void QCViewer::unimplemented () {
  Gtk::MessageDialog dialog(*this, "Feature Unimplemented");
  dialog.set_secondary_text(
          "This feature doesn't exist yet/is currently disabled.");
  dialog.run();
}

void QCViewer::on_menu_file_open_circuit () {
  Gtk::FileChooserDialog dialog("Please choose a circuit file",
            Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  int result = dialog.run();
  if (result == Gtk::RESPONSE_OK) {
    c.load (dialog.get_filename ());
    selection = -1;
    c.set_drawparallel (drawparallel);
    c.set_drawarch (drawarch);
    c.set_scale (1);
    if (breakpointmode) { c.toggle_breakpoint_edit (); }
    std::stringstream ss;
    ss << "QCost: " << c.get_QCost()<< " Depth: " << c.get_Depth() << " Gates: " << c.get_NumGates();
    m_statusbar.push(ss.str());
    c.reset ();
  }
}

void QCViewer::on_menu_file_open_arch () {
  Gtk::FileChooserDialog dialog ("Please choose an architecture file",
                                 Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for (*this);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  int result = dialog.run ();
  if (result == Gtk::RESPONSE_OK) {
    c.loadArch (dialog.get_filename ());
  }
}
void QCViewer::on_menu_simulate_show_stateView(){
  StateViewWidget* sw = new StateViewWidget (&m_statusbar, &m_VisBox, &viz, &m_EditVisPane);
  sw->set_state (state);
  m_VisBox.add (*sw);
  viz.push_back (sw);
  sw->show ();
  if (viz.size() == 1) {
    int w, h;
    get_size (w, h);
    m_EditVisPane.set_position (h - 200);
  }
}

void QCViewer::on_menu_file_quit () {
  hide ();
}

void QCViewer::on_menu_save_png () {
  Gtk::FileChooserDialog dialog ("Please choose a png file to save to",
                                 Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for (*this);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  int result = dialog.run ();
  if (result == Gtk::RESPONSE_OK) {
    c.savepng (dialog.get_filename ());
  }
}
void QCViewer::on_menu_save_svg () {
  Gtk::FileChooserDialog dialog ("Please choose a svg file to save to",
                                 Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for (*this);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  int result = dialog.run ();
  if (result == Gtk::RESPONSE_OK) {
    c.savesvg (dialog.get_filename ());
  }
}

void QCViewer::on_menu_options_parallel () {
  drawparallel = !drawparallel;
  c.set_drawparallel (drawparallel);
}

void QCViewer::on_menu_options_arch () {
  drawarch = !drawarch;
  c.set_drawarch (drawarch);
}

void QCViewer::on_menu_zoom_in () {
  double scale = c.get_scale ();
  c.set_scale (scale*1.25);
}

void QCViewer::on_menu_zoom_out () {
  double scale = c.get_scale ();
  c.set_scale (scale/1.25);
}

void QCViewer::on_menu_zoom_100 () {
  c.set_scale (1);
}

void QCViewer::on_menu_load_state () {
  Gtk::Dialog enterState("Enter State");
  enterState.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  enterState.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  Gtk::Entry stateEntry;
  stateEntry.set_max_length(5000);
  stateEntry.show();
  enterState.get_vbox()->pack_start(stateEntry,Gtk::PACK_SHRINK);
  int result = enterState.run();
  if (result == Gtk::RESPONSE_OK){
    if (state!=NULL) delete state;
    state = getStateVec (stateEntry.get_text(), true);
    state->print();
    for (unsigned int i = 0; i < viz.size(); i++) viz[i]->set_state(state);
    c.set_state(state);
  }
  c.reset ();
}

void QCViewer::on_menu_step () {
  c.step();
  for (unsigned int i = 0; i < viz.size(); i++) viz[i]->reset ();
}

void QCViewer::on_menu_reset () {
  c.reset ();
}

void QCViewer::on_menu_run () {
//  while (c.step()){
    c.run (true);
    for (unsigned int i = 0; i < viz.size(); i++)  viz[i]->reset ();
//    while (gtk_events_pending()) gtk_main_iteration(); // yield the cpu to pending ui tasks (e.g. drawing progress)
//  }
}

void QCViewer::on_menu_delete () {
  if (selection == -1) return;
  c.delete_gate ((unsigned int)selection);
}

void QCViewer::on_menu_inserttest () {
  c.set_insert (true);
}

void QCViewer::on_menu_pan () {
  c.set_insert (false);
}

void QCViewer::on_menu_edit_breakpoints () {
  breakpointmode = !breakpointmode;
  c.toggle_breakpoint_edit ();
}

void QCViewer::set_selection (int i) {
  selection = i;
  if (i == -1) {
//    m_Palette.remove(*group_gateprops);
  } else {
  //  m_Palette.add(*group_gateprops);
  }
}
