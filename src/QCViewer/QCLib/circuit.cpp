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


#include "circuit.h"
#include "utility.h"
#include <map>
#include <algorithm> // for sort, which we should probably cut out
#include <fstream>
#include <iostream>
#include "draw_common.h"
#include "draw_constants.h"

using namespace std;

Circuit::Circuit() : arch(NULL) {}

Circuit::~Circuit ()
{
    removeArch ();
    removeSubcircuits();
    removeGates();
}

void Circuit::newArch ()
{
    arch = new QArch (numLines());
}

void Circuit::removeArch ()
{
    if (arch != NULL) {
        delete arch;
        arch = NULL;
    }
}
void Circuit::removeSubcircuits()
{
    for ( map<string,Circuit*>::iterator it = subcircuits.begin() ; it != subcircuits.end(); it++ ) {
        delete (*it).second;
    }
}

void Circuit::removeGates()
{
    for(unsigned int i = 0; i < gates.size(); i++) {
        delete gates[i];
    }
}

void Circuit::swapGate (unsigned int i, unsigned int j)
{
    Gate *tmp = gates[i];
    gates[i] = gates[j];
    gates[j] = tmp;
}

void Circuit::addGate(Gate *newGate)
{
    gates.push_back(newGate);
    getGreedyParallel();
}

void Circuit::addGate(Gate *newGate, unsigned int pos)
{
    gates.insert(gates.begin()+pos, newGate);
    getGreedyParallel();
}

void Circuit::setGate(Gate *newGate, unsigned int pos)
{
    gates.at(pos) = newGate;
}

void Circuit::removeGate (unsigned int pos)
{
    delete gates[pos];
    gates.erase (gates.begin () + pos);
}

Gate* Circuit::getGate(int pos) const
{
    return gates.at(pos);
}

unsigned int Circuit::numGates() const
{
    return gates.size();
}

int Circuit::QCost()
{
    return 0;
}

string Line::getInputLabel() const
{
    if (constant) {
        return intToString(initValue);
    }
    return lineName;
}

string Line::getOutputLabel() const
{
    if (garbage) {
        return "Garbage";
    }
    if (outLabel.compare("")==0) {
        return lineName;
    }
    return outLabel;
}

unsigned int Circuit::numLines() const
{
    return lines.size();
}

const Line& Circuit::getLine(int pos) const
{
    return lines.at(pos);
}

Line& Circuit::getLineModify(int pos)
{
    return lines.at(pos);
}

Line::Line(string name)
{
    lineName  = name;
    garbage   = true;
    constant  = true;
    initValue = 0;
}

void Circuit::addLine(string lineName)
{
    lines.push_back(Line(lineName));
}

vector<int> Circuit::getParallel() const
{
    vector<int>  returnValue;
    map<int,int> linesUsed;
    for(unsigned int i = 0; i<numGates(); i++) {
        Gate *g = getGate(i);
start:
        for(unsigned int j = 0; j < g->controls.size(); j++) {
            if (linesUsed.find(g->controls[j].wire) != linesUsed.end()) {
                returnValue.push_back(i - 1); //Push back the gate number before this one
                linesUsed.clear();
                goto start; //Go back to begining of main loop (redo this iteration because this gate is in the next block)
            }
            linesUsed[g->controls[j].wire];
        }
        for(unsigned int j = 0; j < g->targets.size(); j++) {
            if (linesUsed.find(g->targets[j]) != linesUsed.end()) {
                returnValue.push_back(i - 1);
                linesUsed.clear();
                goto start;
            }
            linesUsed[g->targets[j]];
        }
    }
    returnValue.push_back (numGates()-1); // for convenience.
    return returnValue;
}

// TODO: this is pretty akward to have outside the drawing code. Reorganize?
vector<int> Circuit::getGreedyParallel()
{
    vector<int> parallel = getParallel (); // doing greedy sometimes "tries too hard"; we need to do greedy within the regions defined here (XXX: explain this better)
    sort (parallel.begin (), parallel.end ());
    vector<int>  returnValue;
    map<int,int> linesUsed;
    unsigned int maxw, minw;
    int k = 0;
    for(unsigned int i = 0; i < numGates(); i++) {
start:
        Gate *g = getGate(i);
        minmaxWire (g->controls, g->targets, minw, maxw);
        for (unsigned int j = minw; j <= maxw; j++) {
            if (linesUsed.find(j) != linesUsed.end()) {
                returnValue.push_back(i - 1);
                linesUsed.clear ();
                goto start;
            }
            linesUsed[j]; //access element so that it exists in map
        }
        if (i == (unsigned int)parallel[k] || std::find(column_breaks.begin(), column_breaks.end(), i) != column_breaks.end()) { // into next parallel group, so force a column move
            returnValue.push_back (i);
            k++;
            linesUsed.clear ();
        }
    }
    for (; k < (int)parallel.size(); k++) {
        returnValue.push_back (k);
    }
    sort (returnValue.begin (), returnValue.end ()); // TODO: needed?
//  returnValue.push_back (numGates()-1); // for convenience.
    columns.clear ();
    for (unsigned int i = 0; i < returnValue.size(); i++) {
        columns.push_back (LayoutColumn(returnValue[i]));
    }
    return returnValue;
}

vector<int> Circuit::getArchWarnings () const
{
    vector<int> warnings;
    vector<unsigned int> wires;
    if (arch == 0) return warnings; // Assume "no" architecture by default.
    for (unsigned int g = 0; g < gates.size(); g++) {
        wires = getGate(g)->targets;
        Gate* gg = getGate (g);
        for (unsigned int i = 0; i < gg->controls.size(); i++) {
            wires.push_back (gg->controls[i].wire);
        }
        bool badgate = false;
        for (unsigned int i = 0; i < wires.size () && !badgate; i++) {
            for (unsigned int j = i + 1; j < wires.size () && !badgate; j++) {
                if (!arch->query (wires[i],wires[j])) badgate = true;
            }
        }
        if (badgate) warnings.push_back(g);
    }
    return warnings;
}

// TODO: check for errors
void Circuit::parseArch (const string filename)
{
    ifstream file;
    file.open (filename.c_str(), ios::in);
    if (!file.is_open()) return;
    unsigned int n;
    file >> n;
    arch = new QArch(n);
    for (unsigned int i = 0; i < n; i++) {
        int m;
        file >> m;
        for (int j = 0; j < m; j++) {
            int q;
            file >> q;
            arch->set (i, q);
        }
    }
    file.close ();
}

void Circuit::arch_set_LNN()
{
    removeArch();
    arch = new QArch(numLines());
    for(unsigned int i=0; i < numLines()-1; i++) {
        arch->set(i,i+1);
    }
}


void Circuit::setName(string n_name)
{
    this->name = n_name;
}

string Circuit::getName()
{
    return this->name;
}


vector<gateRect> Circuit::draw (cairo_t* cr, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, const vector<Selection> &selections, cairo_font_face_t * ft_default) const
{
    cairo_scale (cr, scale, scale);
    cairo_set_font_face (cr,ft_default);
    cairo_set_font_size(cr, 18);

    vector<gateRect> rects;
    //Push the gate drawing into a group so that wireend can be determined and wires can be drawn first
    cairo_push_group (cr);
    rects = draw_circ (cr, &wirestart, &wireend, true);
    cairo_pattern_t *group = cairo_pop_group (cr);
    drawbase (cr, ext.width+ext.x, ext.height+ext.y+thickness, wirestart, wireend);
    cairo_set_source (cr, group);
    //Draw the gates
    cairo_paint(cr);
    cairo_pattern_destroy (group);
    if (drawParallel) drawParallelSectionMarkings (cr, rects, numLines(), getParallel());
    if (drawArch) drawArchitectureWarnings (cr, rects, getArchWarnings());
    if (!selections.empty()) drawSelections (cr, rects, selections);

    return rects;
}

vector<gateRect> Circuit::draw_circ (cairo_t *cr, double *wirestart, double *wireend, bool forreal) const
{
    vector <gateRect> rects;
    cairo_set_source_rgb (cr, 0, 0, 0);

    // input labels
    double xinit = 0.0;
    for (uint32_t i = 0; i < numLines(); i++) {
        string label = getLine(i).getInputLabel();
        cairo_text_extents_t extents;
        cairo_text_extents(cr, label.c_str(), &extents);

        double x = 0, y = 0;
        if (forreal) {
            x = *wirestart - extents.width;
            y = wireToY(i) - (extents.height/2 + extents.y_bearing);
        }
        cairo_move_to (cr, x, y);
        cairo_show_text (cr, label.c_str());
        xinit = max (xinit, extents.width);
    }

    if (!forreal) *wirestart = xinit;

    // gates
    double xcurr = xinit+2.0*gatePad;
    uint32_t mingw, maxgw;
    uint32_t i = 0;
    double maxX;
    if (columns.empty()) cout << "WARNING: invalid layout detected in " << __FILE__ << " at line " << __LINE__ << "!\n";
    for (uint32_t j = 0; j < columns.size(); j++) {
        maxX = 0.0;
        for (; i <= columns.at(j).lastGateID; i++) {
            Gate* g = getGate (i);
            minmaxWire (g->controls, g->targets, mingw, maxgw);
            g->draw(cr,xcurr,maxX,rects);
        }
        xcurr += gatePad;
        xcurr += maxX;
    }
    xcurr -= maxX;
    xcurr += gatePad;
    gateRect fullCirc;
    if (rects.size() > 0) {
        fullCirc = rects[0];
        for (unsigned int i = 1; i < rects.size(); i++) {
            fullCirc = combine_gateRect(fullCirc,rects[i]);
        }
    }
    *wireend = *wirestart + fullCirc.width + gatePad*2;

    // output labels
    cairo_set_source_rgb (cr, 0, 0, 0);
    for (uint32_t i = 0; i < numLines (); i++) {
        string label = getLine (i).getOutputLabel();
        cairo_text_extents_t extents;
        cairo_text_extents (cr, label.c_str(), &extents);

        double x, y;
        x = *wireend + xoffset;
        y = wireToY(i) - (extents.height/2+extents.y_bearing);
        cairo_move_to (cr, x, y);
        cairo_show_text (cr, label.c_str());
    }
    return rects;
}

void Circuit::drawbase (cairo_t *cr, double w, double h, double wirestart, double wireend) const
{
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_rectangle (cr, 0, 0, w, h); // TODO: document why the scale factors are here
    cairo_fill (cr);

    for (uint32_t i = 0; i < numLines(); i++) {
        double y = wireToY (i);
        drawWire (cr, wirestart+xoffset, y, wireend, y);
    }
}

void Circuit::drawArchitectureWarnings (cairo_t* cr, const vector<gateRect> &rects, const vector<int> &badGates) const
{
    for (uint32_t i = 0; i < badGates.size(); i++) {
        drawRect (cr, rects[badGates[i]], Colour(0.8,0.1,0.1,0.7), Colour(0.8,0.4,0.4,0.3));
    }
}

void Circuit::drawParallelSectionMarkings (cairo_t* cr, const vector<gateRect> &rects, int numLines, const vector<int> &pLines) const
{
    for (uint32_t i = 0; i < pLines.size() - 1; i++) {
        int gateNum = pLines[i];
        double x = (rects[gateNum].x0 + rects[gateNum].width + rects[gateNum+1].x0)/2;
        drawPWire (cr, x, numLines);
    }
}

//for parallism wires
void Circuit::drawPWire (cairo_t *cr, double x, int numLines) const
{
    cairo_set_line_width (cr, thickness);
    cairo_set_source_rgba (cr, 0.4, 0.4, 0.4,0.4);
    cairo_move_to (cr, x, wireToY(0));
    cairo_line_to (cr, x, wireToY(numLines-1));
    cairo_stroke (cr);
    cairo_set_source_rgb (cr, 0, 0, 0);
}

void Circuit::drawSelections (cairo_t* cr, const vector<gateRect> &rects, const vector<Selection> &selections) const
{
    for (unsigned int i = 0; i < selections.size (); i++) {
        if (selections[i].gate < rects.size()) { //XXX Why is this needed?
            drawRect(cr, rects[selections[i].gate], Colour (0.1, 0.2, 0.7, 0.7), Colour (0.1,0.2,0.7,0.3));
            if (selections[i].sub != NULL && rects[selections[i].gate].subRects != NULL) {
                drawSelections (cr, *rects[selections[i].gate].subRects, *selections[i].sub);
            }
        }
    }
}

cairo_rectangle_t Circuit::get_circuit_size (double* wirestart, double* wireend, double scale, cairo_font_face_t * ft_default) const
{
    cairo_surface_t *unbounded_rec_surface = cairo_recording_surface_create (CAIRO_CONTENT_COLOR, NULL);
    cairo_t *cr = cairo_create(unbounded_rec_surface);
    cairo_set_source_surface (cr, unbounded_rec_surface, 0.0, 0.0);
    cairo_scale (cr, scale, scale);
    cairo_set_font_face (cr,ft_default);
    cairo_set_font_size(cr, 18);
    draw_circ (cr, wirestart, wireend, false); // XXX fix up these inefficienies!!
    cairo_rectangle_t ext;
    cairo_recording_surface_ink_extents (unbounded_rec_surface, &ext.x, &ext.y, &ext.width, &ext.height);
    cairo_destroy (cr);
    cairo_surface_destroy (unbounded_rec_surface);
    return ext;
}

void Circuit::savepng (string filename, cairo_font_face_t * ft_default)
{
    double wirestart, wireend;
    getGreedyParallel();
    cairo_rectangle_t ext = get_circuit_size (&wirestart, &wireend, 1.0,ft_default);
    cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, ext.width+ext.x, thickness+ext.height+ext.y);
    cairo_t* cr = cairo_create (surface);
    cairo_set_source_surface (cr, surface, 0, 0);
    draw( cr, false, false,  ext, wirestart, wireend, 1.0, vector<Selection>(), ft_default);
    write_to_png (surface, filename);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

void Circuit::write_to_png (cairo_surface_t* surf, string filename) const
{
    cairo_status_t status = cairo_surface_write_to_png (surf, filename.c_str());
    if (status != CAIRO_STATUS_SUCCESS) {
        cout << "Error saving to png." << endl;
        return;
    }
}
