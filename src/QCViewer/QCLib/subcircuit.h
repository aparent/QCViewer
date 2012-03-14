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

Authors: Alex Parent
---------------------------------------------------------------------*/

#ifndef SUBCIRCUIT_H
#define SUBCIRCUIT_H

#include "gate.h"
#include "circuit.h"
#include <string>
#include <map>
//A subcircuit gate contains
class Subcircuit : public Gate
{
public:
    Subcircuit(Circuit*, const std::map<unsigned int,unsigned int>&,unsigned int);
    Gate* clone() const;
    std::string getName() const;
    void setName(std::string name);
    State applyToBasis(index_t) const;
    State applySubcirc(const State&) const;
    unsigned int numGates() const;
    void draw(cairo_t *cr,double &xcurr,double &maxX, std::vector <gateRect> &rects) ;

    //! Steps the subcircuit through the next gate.
    bool step (State &s);
    //! Resets current gate to the beginning of the subcircuit.
    void reset ();


    Gate* getGate(int pos) const;
    Circuit* getCircuit();
    std::vector<unsigned int> getGreedyParallel() const;  //Returns a vector of ints specifying the last gate in each parallel block.

    //! Set true is circuit should be draw in expanded mode
    bool expand;
    //! Set true is circuit should be draw in unrolled mode
    bool unroll;

protected:
    std::map<unsigned int,unsigned int> lineMap;
private:
    gateRect drawBoxed (cairo_t *cr, uint32_t xc) const;
    void drawSubCircBox(cairo_t* cr, gateRect &r) const;
    gateRect drawExp(cairo_t *cr,double xcurr) const;
    index_t BuildBitString (index_t, unsigned int);
    SimState* simState;
    Circuit* circ;
};


#endif
