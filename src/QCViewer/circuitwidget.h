/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

See also 'ADDITIONAL TERMS' at the end of the included LICENSE file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QCViewer is a trademark of the of the The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

Authors: Alex Parent, Jacob Parker
---------------------------------------------------------------------*/


#ifndef CIRCUITWIDGET__INCLUDED
#define CIRCUITWIDGET__INCLUDED

#include <gtkmm/drawingarea.h>
#include <string>

#include "QCLib/circuit.h"
#include "QCLib/circuitImage.h"
#include "QCLib/state.h"
#include "QCLib/gate.h"
#include "draw.h"
#include "QCLib/common.h"
#include <memory>

class CircuitWidget : public Gtk::DrawingArea
{
public:
    CircuitWidget ();
    virtual ~CircuitWidget ();
    std::vector<std::string> load (std::string);
    void loadArch (std::string);
    void arch_set_LNN();

    void clear_selection ();
    void set_window (Gtk::Window *);
    void set_offset (int);
    void set_drawarch (bool);
    void set_drawparallel (bool);

    void savepng (std::string);
    void savesvg (std::string);
    void saveps (std::string);
    void save_circuit (std::string);

    void set_scale (double);
    double get_scale ();
    int get_QCost ();
    int get_Depth ();
    int get_num_gates ();
    unsigned int get_num_lines ();

    void newcircuit (uint32_t);

    void set_state (State*);
    bool step ();
    bool run (bool);
    void reset ();
    void insert_gate_in_new_column (std::shared_ptr<Gate>, uint32_t, std::shared_ptr<Circuit>);
    void insert_gate_in_column (std::shared_ptr<Gate>, uint32_t);
    void insert_gate_at_front (std::shared_ptr<Gate>);
    void generate_layout_rects ();

    std::shared_ptr<Gate> getSelectedGate ();
    void deleteSelectedGate ();
    enum Mode { NORMAL, EDIT_CONTROLS, EDIT_BREAKPOINTS };
    void set_mode (Mode);
    void add_subcirc ();
    void force_redraw ();
    void expand_all ();

    bool is_subcirc (unsigned int);
    std::shared_ptr<Gate> getGate(unsigned int);

protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool onScrollEvent (GdkEventScroll* event);
    virtual bool onMotionEvent (GdkEventMotion* event);
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
                                       int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

private:
    Mode mode;
    //std::vector<LayoutColumn> layout;
    std::vector<uint32_t> breakpoints;

    void getCircuitAndColPosition (double x, double y, std::shared_ptr<Circuit> c, std::vector<gateRect> &rects, std::string &r_name, int &r_pos);
    void check_circuit_size();

    bool simulation_enabled;
    uint32_t NextGateToSimulate;
    bool panning,size_changed;
    double oldmousex, oldmousey;

    gateRect select_rect;

    void toggle_selection (int);
    std::shared_ptr<Gate> getSelectedSubGate (std::shared_ptr<Circuit> circuit, std::vector<Selection> sub);
    void deleteSelectedSubGate (std::shared_ptr<Circuit> circuit, std::vector<Selection> selections);

    State *state;
    bool drawarch, drawparallel;
    Gtk::Window *win;
    int yoffset;
    std::shared_ptr<Circuit> circuit;
    cairo_rectangle_t ext;
    double wirestart, wireend;

    std::vector<gateRect> columns;
    std::vector<gateRect> rects;
    std::vector<Selection> selections;

    double scale;
    double cx, cy;

    uint32_t getFirstWire (double);

    cairo_font_face_t * init_fonts();
    cairo_font_face_t * ft_default;
    CircuitImage circuitDrawer;
};

#endif
