#include "window.h"
#include <gtkmm/stock.h>
#include <iostream>

QCViewer::QCViewer() : m_button1("Button 1"), m_button2("Button 2"), drawparallel(false), drawarch (false) {
  set_title("QCViewer");
  set_border_width(0);

  add(m_vbox);

  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("File", "File"));
  m_refActionGroup->add(Gtk::Action::create("Circuit", "Circuit"));
  m_refActionGroup->add(Gtk::Action::create("Arch", "Architecture"));
  m_refActionGroup->add(Gtk::Action::create("Diagram", "Diagram"));

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

  m_refActionGroup->add(Gtk::Action::create("SimulateMenu", "Simulate"));
  m_refActionGroup->add(Gtk::Action::create ("SimulateLoad", Gtk::Stock::ADD, "Load state", "Enter a state for input into the circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create ("SimulateRun", Gtk::Stock::GOTO_LAST, "Run", "Simulate the entire circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create ("SimulateStep", Gtk::Stock::GO_FORWARD, "Step", "Advance the simulation through a single gate"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create ("SimulateReset", Gtk::Stock::STOP, "Reset", "Reset the simulation to the start of the circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create ("SimulateDisplay", "Display state"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_parallel));

  m_refActionGroup->add(Gtk::Action::create("ArchitectureMenu", "Architecture"));
  m_refActionGroup->add(Gtk::ToggleAction::create ("DiagramParallel", "Show parallel guides"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_parallel));
  m_refActionGroup->add(Gtk::ToggleAction::create ("DiagramArch", Gtk::Stock::DIALOG_WARNING, "Show warnings", "Show architecture alignment warnings"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_arch));
  

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='File'>"
        "        <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='Circuit'>"
        "        <menuitem action='CircuitNew'/>"
        "        <menuitem action='CircuitOpen'/>"
        "        <menuitem action='CircuitSave'/>"
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
        "      <menuitem action='SimulateLoad'/>"
        "      <menuitem action='SimulateRun'/>"
        "      <menuitem action='SimulateStep'/>"
        "      <menuitem action='SimulateReset'/>"
        "      <separator/>"
        "      <menuitem action='SimulateDisplay'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='CircuitOpen'/>"
        "    <separator/>"
        "    <toolitem action='SimulateLoad'/>"
        "    <toolitem action='SimulateRun'/>"
        "    <toolitem action='SimulateStep'/>"
        "    <toolitem action='SimulateReset'/>"
        "    <separator/>"
        "    <toolitem action='ZoomIn'/>"
        "    <toolitem action='ZoomOut'/>"
        "    <toolitem action='Zoom100'/>"
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

  Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
  if(pToolbar)
    m_vbox.pack_start(*pToolbar, Gtk::PACK_SHRINK);

  c.set_window (this);
  c.show();
  m_vbox.pack_start(c);

//  m_vbox.pack_start (m_cmdOut);
//  m_vbox.pack_start (m_cmdIn, Gtk::PACK_SHRINK);
//  m_cmdOut.show();
//  m_cmdOut.set_editable (false);
//  m_cmdIn.show ();

  m_vbox.show();

  show_all_children ();
}

QCViewer::~QCViewer() {}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void QCViewer::on_button_clicked(Glib::ustring data) {
  std::cout << "Hello World - " << data << " was pressed" << std::endl;
}

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
    c.set_drawparallel (drawparallel);
    c.set_drawarch (drawarch);
    c.set_scale (1);
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
