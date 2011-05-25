#include "window.h"
#include <gtkmm/stock.h>
#include <iostream>

QCViewer::QCViewer() : m_button1("Button 1"), m_button2("Button 2"), drawarch(false), drawparallel (false) {
  set_title("QCViewer");
  set_border_width(0);

  add(m_vbox);

  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("FileMenu", "File"));
  m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW));
  m_refActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN));
  m_refActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE));

  m_refActionGroup->add(Gtk::Action::create("FileNewCircuit", "_Circuit", "Create new circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("FileNewArch", "_Architecture", "Create new architecture"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));

  m_refActionGroup->add(Gtk::Action::create("FileOpenCircuit", "_Circuit", "Open a circuit file"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_file_open_circuit));
  m_refActionGroup->add(Gtk::Action::create("FileOpenArch", "_Architecture", "Open an architecture file"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));

  m_refActionGroup->add(Gtk::Action::create("FileSaveImage", "_Diagram", 
                                            "Save the circuit diagram to an image file"));
  m_refActionGroup->add(Gtk::Action::create("FileSaveCircuit", "_Circuit", "Save circuit"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("FileSaveArchitecture", "_Architecture", "Save architecture"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("FileSaveImagePng", "P_NG", 
                                            "Save circuit diagram as a Portable Network Graphics file"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_save_png));
  m_refActionGroup->add(Gtk::Action::create("FileSaveImageSvg", "S_VG", 
                                            "Save circuit diagram as a Scalable Vector Graphics file"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));
  m_refActionGroup->add(Gtk::Action::create("FileSaveImagePs", "_Postscript", "Save circuit diagram as a Postscript file"),
                        sigc::mem_fun(*this, &QCViewer::unimplemented));


  m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
                        sigc::mem_fun(*this, &QCViewer::on_menu_file_quit));

  m_refActionGroup->add(Gtk::Action::create("EditMenu", "Edit"));
  m_refActionGroup->add(Gtk::Action::create("SimulateMenu", "Simulate"));
  m_refActionGroup->add(Gtk::Action::create("ArchitectureMenu", "Architecture"));
  m_refActionGroup->add(Gtk::Action::create("OptionsMenu", "Options"));
  m_refActionGroup->add(Gtk::ToggleAction::create ("OptionsParallel", "Parallel section guides"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_parallel));
  m_refActionGroup->add(Gtk::ToggleAction::create ("OptionsArch", "Architecture warnings"),
                        sigc::mem_fun(*this, &QCViewer::on_menu_options_arch));

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menu action='FileNew'>"
        "        <menuitem action='FileNewCircuit'/>"
        "        <menuitem action='FileNewArch'/>"
        "      </menu>"
        "      <menu action='FileOpen'>"
        "        <menuitem action='FileOpenCircuit'/>"
        "        <menuitem action='FileOpenArch'/>"
        "      </menu>"
        "      <menu action='FileSave'>"
        "        <menu action='FileSaveImage'>"
        "          <menuitem action='FileSaveImagePng'/>"
        "          <menuitem action='FileSaveImageSvg'/>"
        "          <menuitem action='FileSaveImagePs'/>"
        "        </menu>"
        "        <menuitem action='FileSaveCircuit'/>"
        "        <menuitem action='FileSaveArchitecture'/>"
        "      </menu>"
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "    </menu>"
        "    <menu action='SimulateMenu'>"
        "    </menu>"
        "    <menu action='OptionsMenu'>"
        "      <menuitem action='OptionsParallel'/>"
        "      <menuitem action='OptionsArch'/>"
        "    </menu>"
        "  </menubar>"
      //  "  <toolbar  name='ToolBar'>"
      //  "    <toolitem action='FileOpenCircuit'/>"
      //  "  </toolbar>"
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
  cout << m_vbox.get_height () << endl;
}

QCViewer::~QCViewer() {}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void QCViewer::on_button_clicked(Glib::ustring data) {
  std::cout << "Hello World - " << data << " was pressed" << std::endl;
}

void QCViewer::unimplemented () {
  std::cout << "Sorry bro\n";
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
  }
  c.set_drawparallel (drawparallel);
  c.set_drawarch (drawarch);
}

void QCViewer::on_menu_file_quit () {
  hide ();
}

void QCViewer::on_menu_save_png () {

}

void QCViewer::on_menu_options_parallel () {
  drawparallel = !drawparallel;
  c.set_drawparallel (drawparallel);
}

void QCViewer::on_menu_options_arch () {
  drawarch = !drawarch;
  c.set_drawarch (drawarch);
}
