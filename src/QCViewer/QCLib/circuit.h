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

#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include "types.h"
#include <vector>
#include <string>
#include <map>
#include "common.h"

class Line
{
public:
    Line (std::string);
    std::string getInputLabel () const;
    std::string getOutputLabel() const;
    std::string lineName;
    std::string outLabel;
    bool constant;
    bool garbage;
    int initValue;
};

class QArch   // Less space efficient than it could trivially be.
{
public:
    ~QArch() {
        delete graph;
    }
    QArch (int nn) : n(nn) {
        graph = new char[n*n];
        for (int i = 0; i < n*n; i++) graph[i] = 0;
    }

    bool query (int i, int j) {
        return (0 != graph[i+n*j]);
    }
    void set (int i, int j) {
        graph[i+n*j] = graph[j+n*i] = 1;
    }
private:
    int n;
    char *graph;
};

class Circuit
{
public:
    Circuit ();
    ~Circuit ();

    int QCost();

    void addLine(std::string line);
    const Line& getLine(int pos) const;
    Line& getLineModify(int pos);
    std::string getName();
    void setName(std::string);
    unsigned int numLines() const;

    void addGate(Gate *newGate); //appends to end
    void addGate(Gate *newGate, unsigned int pos); //inserts at pos
    void setGate(Gate *newGate, unsigned int pos);
    void removeGate (unsigned int);
    Gate* getGate(int pos) const;
    unsigned int numGates() const; //Returns the number of gates
    void swapGate (unsigned int, unsigned int);
    void removeGates();

    void newArch ();
    void removeArch ();
    void parseArch (std::string);
    void arch_set_LNN();

    std::vector<int> getParallel() const;  //Returns a std::vector of ints specifying the last gate in each parallel block.
    std::vector<int> getGreedyParallel () const; // used for drawing gates in the same column
    std::vector<int> getArchWarnings () const;
    std::vector<int> column_breaks;

    std::map<std::string,Circuit*> subcircuits;

    void removeSubcircuits();

    QArch *arch;


    std::vector<gateRect> draw (cairo_t* cr, std::vector<LayoutColumn>& columns, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, const std::vector<Selection> &selections,cairo_font_face_t * ft_default) const;
    cairo_rectangle_t get_circuit_size (std::vector<LayoutColumn>& columns, double* wirestart, double* wireend, double scale, cairo_font_face_t * ft_default) const;
    //void savepng (std::string filename, cairo_font_face_t * ft_default) const;
private:
    std::string name;
    std::vector <Gate*>           gates;
    std::vector <Line>            lines;

    std::vector<gateRect> draw_circ (cairo_t *cr, std::vector<LayoutColumn>& columns, double *wirestart, double *wireend, bool forreal) const;
    void drawbase (cairo_t *cr, double w, double h, double wirestart, double wireend) const;
    void drawParallelSectionMarkings (cairo_t* cr, const std::vector<gateRect> &rects, int numLines, const std::vector<int> &pLines) const;
    void drawArchitectureWarnings (cairo_t* cr, const std::vector<gateRect> &rects, const std::vector<int> &badGates) const;
    void drawSelections (cairo_t* cr, const std::vector<gateRect> &rects, const std::vector<Selection> &selections) const;
    void drawPWire (cairo_t *cr, double x, int numLines) const;
    //void write_to_png (cairo_surface_t* surf, std::string filename) const;
};


#endif
