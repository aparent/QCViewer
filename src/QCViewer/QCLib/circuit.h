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

/*! \file circuit.h
    \brief Defines the circuit class and some other related classes
*/

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

class SimState
{
public:
    SimState () : gate(0),nextGate(true),simulating(false),loop(1) {}
    unsigned int gate;
    bool nextGate;
    bool simulating;
    unsigned int loop;
};

/*!
	\brief Defines a circuit
*/
class Circuit
{
public:
    Circuit ();
    ~Circuit ();

    int QCost();


    /*!
    	\brief Append a new line to circuit.
    	\param line Name of the line
    */
    void addLine(std::string line);

    /*!
    	\brief Returns a const Line for reading
    	\param pos Line number to get

    	Use this function if you do not wish to
    	modify the line so that const correctness
    	can be maintained
    */
    const Line& getLine(int pos) const;
    /*!
    	\brief Returns a referance to Line for modification
    	\param pos Line number to get

    	Use this if you wish to get a line and want to modify
    	it
    */
    Line& getLineModify(int pos);
    //! Returns the circuit name
    std::string getName();
    //! Sets the circuit name
    void setName(std::string);
    //! Expands all subcircuits
    void expandAll();
    unsigned int numLines() const;

    /*! @name Gates
    	Functions that have to do with manipulating gates.
    */
    ///@{
    //! Appends gate to the end of the circuit
    void addGate(Gate *newGate);
    //! Inserts gate at pos
    void addGate(Gate *newGate, unsigned int pos);
    /*!
    	\brief Sets gate at pos
    	\param newGate gate to set
    	\param pos postion of gate to be replaced

    	Use this to replace a gate in the the circuit.
    	Note: You may want to delete the old gate if it is no longer in use
    */
    void setGate(Gate *newGate, unsigned int pos);
    //! removes gate at pos
    void removeGate (unsigned int pos);
    //! Swaps gates at a and b
    void swapGate (unsigned int a, unsigned int b);
    //! returns gate at pos
    Gate* getGate(int pos) const;

    //! Returns the number of gates counting subcircuits as 1 gate
    unsigned int numGates() const;
    ///@}

    /*! @name Statistics
    	These functions are used to retrive circuit statistics
    */
    ///@{
    //! Returns the total number of gates in the circuit counting all gates in subcircuits
    unsigned int totalGates() const;
    ///@}

    /*! @name Simulation
    		These functions are used to run circuit simulation
    	*/
    ///@{
    //! Runs the full circuit on the input state.
    bool run (State &s);
    //! Steps the circuit through the next gate.
    bool step (State &s);
    //! Resets current gate to the beginning of the circuit.
    void reset ();
    ///@}

    /* architecture */
    void newArch ();
    void parseArch (std::string);
    void arch_set_LNN();

    //! Returns a std::vector of ints specifying the last gate in each parallel block.
    std::vector<int> getParallel() const;
    std::vector<unsigned int> getGreedyParallel (); // used for drawing gates in the same column
    std::vector<int> getArchWarnings () const;
    std::vector <unsigned int>    columns;


    //! Mapping of names to circuits storing subcircuits that might appear in this circuit
    std::map<std::string,Circuit*> subcircuits;

    /*! @name Drawing
    	These functions have to do with produceing images of the circuit
    */
    ///@{
    std::vector<gateRect> draw (cairo_t* cr, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, const std::vector<Selection> &selections,cairo_font_face_t * ft_default) const;
    cairo_rectangle_t get_circuit_size (double* wirestart, double* wireend, double scale, cairo_font_face_t * ft_default) const;
    //!Saves a png of the circuit
    void savepng (std::string filename, cairo_font_face_t * ft_default);
    //!Saves an svg of the circuit
    void savesvg (std::string filename, cairo_font_face_t * ft_default);
    //!Saves an eps of the circuit
    void saveps (std::string filename, cairo_font_face_t * ft_default);
    ///@}
private:
    QArch *arch;
    std::string name;
    std::vector <Gate*>           gates;
    std::vector <Line>            lines;
    std::vector <unsigned int> breakpoints;

    SimState simState;

    bool allExpanded;

    std::vector<gateRect> draw_circ (cairo_t *cr, double *wirestart, double *wireend, bool forreal) const;
    void drawbase (cairo_t *cr, double w, double h, double wirestart, double wireend) const;
    void drawParallelSectionMarkings (cairo_t* cr, const std::vector<gateRect> &rects, int numLines, const std::vector<int> &pLines) const;
    void drawArchitectureWarnings (cairo_t* cr, const std::vector<gateRect> &rects, const std::vector<int> &badGates) const;
    void drawSelections (cairo_t* cr, const std::vector<gateRect> &rects, const std::vector<Selection> &selections) const;
    void drawPWire (cairo_t *cr, double x, int numLines) const;
    void write_to_png (cairo_surface_t* surf, std::string filename) const;
    cairo_surface_t* make_svg_surface (std::string filename, cairo_rectangle_t ext) const;
    cairo_surface_t* make_ps_surface (std::string filename, cairo_rectangle_t ext) const;

    unsigned int findcolumn (unsigned int gate) const;
    //for deconstructor
    void removeSubcircuits();
    void removeArch ();
    void removeGates();
};


#endif
