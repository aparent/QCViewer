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
    Subcircuit(Circuit*, std::map<unsigned int,unsigned int>,unsigned int);
    Gate* clone() const;
    std::string getName() const;
    void setName(std::string name);
    State applyToBasis(index_t) const;
    State applySubcirc(const State&) const;
    int numGates() const;

    Gate* getGate(int pos) const;
    Circuit* getCircuit();
    std::vector<int> getGreedyParallel() const;  //Returns a vector of ints specifying the last gate in each parallel block.
    bool expand;
protected:
    std::map<unsigned int,unsigned int> lineMap;
private:
    index_t BuildBitString (index_t, unsigned int);
    Circuit* circ;
};


#endif
