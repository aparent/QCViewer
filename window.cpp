#include "window.h"
#include <gtkmm/stock.h>
#include <iostream>
#include <string>
#include <state.h>
#include <dirac.h>

QCViewer::QCViewer() : drawparallel(false), drawarch (false) {
	mode = EDIT_MODE;
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

  m_refActionGroup->add(Gtk::Action::create("ModeEdit", "Edit"), sigc::mem_fun (*this, &QCViewer::on_menu_mode_edit));
	m_refActionGroup->add(Gtk::Action::create("ModeDelete", Gtk::Stock::DELETE, "Delete Gate"), sigc::mem_fun (*this, &QCViewer::on_menu_delete));

	m_refActionGroup->add(Gtk::Action::create("ModeSimulate", "Simulate"), sigc::mem_fun (*this, &QCViewer::on_menu_mode_simulate));

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
				"    <menu action='Mode'>"
				"      <menuitem action='ModeEdit'/>"
				"      <menuitem action='ModeSimulate'/>"
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
        "  </toolbar>"
				"  <toolbar name='EditToolbar'>"
				"    <toolitem action='CircuitOpen'/>"
				"    <separator/>"
        "    <toolitem action='Zoom100'/>"
        "    <separator/>"
				"    <toolitem action='ModeDelete'/>"
        "    <separator/>"
        "    <toolitem action='DiagramArch'/>"
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
	m_EditToolbar = m_refUIManager->get_widget ("/EditToolbar");

  group_gates = Gtk::manage(new Gtk::ToolItemGroup("Gates         "));
  group_gateprops = Gtk::manage(new Gtk::ToolItemGroup("Properties"));
  group_warnings = Gtk::manage(new Gtk::ToolItemGroup("Warnings"));
  m_Palette.add(*group_gates);
	m_Palette.add(*group_gateprops);
//	m_Palette.add(*group_warnings);

	m_Palette.set_orientation (Gtk::ORIENTATION_VERTICAL);

	Gtk::ToolButton* NOTbrush = Gtk::manage(new Gtk::ToolButton(NOTicon));
  NOTbrush->set_tooltip_text ("NOT");
	group_gates->insert(*NOTbrush);

	Gtk::ToolButton* Hbrush = Gtk::manage(new Gtk::ToolButton(Hicon));
	Hbrush->set_tooltip_text ("Hadamard");
	group_gates->insert(*Hbrush);

	Gtk::ToolButton* Xbrush = Gtk::manage(new Gtk::ToolButton(Xicon));
	Xbrush->set_tooltip_text ("Pauli-X");
	group_gates->insert(*Xbrush);

	Gtk::ToolButton* Ybrush = Gtk::manage(new Gtk::ToolButton(Yicon));
  Ybrush->set_tooltip_text ("Pauli-Y");
	group_gates->insert(*Ybrush);

	Gtk::ToolButton* Zbrush = Gtk::manage(new Gtk::ToolButton(Zicon));
	Zbrush->set_tooltip_text ("Pauli-Z");
	group_gates->insert(*Zbrush);

	Gtk::ToolButton* Rbrush = Gtk::manage(new Gtk::ToolButton(Ricon));
	Rbrush->set_tooltip_text ("Phase Shift");
	group_gates->insert(*Rbrush);

	Gtk::ToolButton* SWAPbrush = Gtk::manage(new Gtk::ToolButton(SWAPicon));
	SWAPbrush->set_tooltip_text ("SWAP");
	group_gates->insert(*SWAPbrush);


	Gtk::ToolButton* brush1 = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::CANCEL));
	Gtk::ToolButton* brush2 = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::CANCEL));
	group_gateprops->insert(*brush1);
	group_warnings->insert(*brush2);
	m_Palette.add_drag_dest (c);

	if (!m_SimulateToolbar || !m_EditToolbar) { cout << "warning failed to create toolbars" << endl; return; }
  m_vbox.pack_start(*m_SimulateToolbar, Gtk::PACK_SHRINK);
	m_vbox.pack_start(*m_EditToolbar, Gtk::PACK_SHRINK);
  c.set_window (this);
  c.show();
	m_hbox.pack_end (c);
	m_hbox.pack_start (m_Palette, Gtk::PACK_SHRINK);
  m_vbox.pack_start (m_hbox);

//  m_vbox.pack_start (m_cmdOut);
//  m_vbox.pack_start (m_cmdIn, Gtk::PACK_SHRINK);
//  m_cmdOut.show();
//  m_cmdOut.set_editable (false);
//  m_cmdIn.show ();

  m_vbox.show();
  m_hbox.show();
  show_all_children ();
	m_SimulateToolbar->hide ();
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
	Gtk::Main::run(sView);
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
		sView.set_state(state);
		c.set_state(state);
	}
  c.reset ();
}

void QCViewer::on_menu_step () {
  c.step();
	sView.redraw();
}

void QCViewer::on_menu_reset () {
  c.reset ();
}

void QCViewer::on_menu_run () {
  while (c.step()){
		sView.redraw();
    while (gtk_events_pending()) gtk_main_iteration(); // yield the cpu to pending ui tasks (e.g. drawing progress)
	}
}

void QCViewer::on_menu_mode_edit () {
	mode = EDIT_MODE;
	c.reset ();
	m_SimulateToolbar->hide();
	m_EditToolbar->show ();
	m_Palette.show ();
}

void QCViewer::on_menu_mode_simulate () {
	mode = SIMULATE_MODE;
	m_EditToolbar->hide ();
	m_SimulateToolbar->show();
	m_Palette.hide ();
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

void QCViewer::set_selection (int i) {
  selection = i;
	if (i == -1) {
//	  m_Palette.remove(*group_gateprops);
  } else {
	//  m_Palette.add(*group_gateprops);
	}
}
