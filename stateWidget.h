#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>
#include <vector>

const unsigned int numBuckets = 256;

class StateWidget : public Gtk::DrawingArea {
public:
  StateWidget (Gtk::Statusbar*);
  void set_state(State *state);
  void parse_state ();
  void reset ();
  enum DrawMode {STATEDRAW_EXPECTED, STATEDRAW_REAL, STATEDRAW_IMAG } drawmode;

protected:
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool onMotionEvent (GdkEventMotion* event);
private:
  double width, height;
  double xborder, yborder;
  double tickwidth;
  double barWidth, barHeight;

  void force_redraw ();
  State *state;
  Gtk::Statusbar* status;
  int mousex, mousey;
  bool draw_compressed;
  unsigned int num_draw; // number of buckets to draw
  unsigned int bucketID[numBuckets]; // XXX: need to be bitstrings
  std::complex<float_t> bucket[numBuckets];
};

class StateViewWidget : public Gtk::VBox {
public:
  StateViewWidget (Gtk::Statusbar*, Gtk::HBox*, std::vector<StateViewWidget*>*, Gtk::VPaned*);
  void reset ();
  void set_state (State*);

protected:
  void set_style ();
  void close ();

private:
  StateWidget sw;
  Gtk::HBox buttonbox;
  Gtk::VBox layoutbox;
  Gtk::RadioButton btn_expected, btn_real, btn_imag;
  Gtk::Button btn_close;
  Gtk::Statusbar* status;

  // These support removing a svw from the main window. It is very ugly to
  // have them here. Perhaps something with signals would be a better design?
  Gtk::HBox* visbox;
  std::vector<StateViewWidget*>* svwList;
  Gtk::VPaned* vispane;
};

std::string draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height, float mousex, float mousey);
