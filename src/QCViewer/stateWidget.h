#ifndef __STATEWIDGET__INCLUDED
#define __STATEWIDGET__INCLUDED

#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>
#include <vector>

const unsigned int numBuckets = 512;

class StateWidget : public Gtk::DrawingArea {
public:
  StateWidget (Gtk::Statusbar*);
	void set_state(State *state);
	void set_trace(index_t);
	void parse_state ();
	void parse_state_trace ();
	void reset ();
	enum DrawMode {EXPECTED,EXPECTED_TRACED, REAL, IMAG } drawmode;

protected:
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool onMotionEvent (GdkEventMotion* event);
private:
  double width, height;
	double xborder, yborder;
	double tickwidth;
	double barWidth, barHeight;
	double t_barWidth;

	void force_redraw ();
	State *state;
	Gtk::Statusbar* status;
	int mousex, mousey;
	bool draw_compressed;
	unsigned int num_draw; // number of buckets to draw
  index_t bucketID[numBuckets]; // XXX: need to be bitstrings
	std::complex<float_type> bucket[numBuckets];

	index_t trace;  // This is a bit string where 1s indicate parts to be included in the expectation
	float_type traced_bucket[numBuckets];// These hold the expectation value of a traced out state
	unsigned int num_draw_traced;
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
	Gtk::CheckButton btn_trace;
  Gtk::Statusbar* status;

  // These support removing a svw from the main window. It is very ugly to
  // have them here. Perhaps something with signals would be a better design?
  Gtk::HBox* visbox;
  std::vector<StateViewWidget*>* svwList;
  Gtk::VPaned* vispane;
};

std::string draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height, float mousex, float mousey);

#endif
