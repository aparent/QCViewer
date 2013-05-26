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

Authors: Alex Parent, Marc Burns
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
    Subcircuit(std::shared_ptr<Circuit>, const std::vector<unsigned int>&,unsigned int);
    std::shared_ptr<Gate> clone() const;
    std::string getName() const;
    std::string getDrawName();
    std::string getLatexName();
    void setName(std::string name);
    unsigned int getNumGates() const;
    unsigned int numGates() const;

    std::shared_ptr<Gate> getGate(int pos) const;
    std::shared_ptr<Circuit> getCircuit();

    //! Set true is circuit should be draw in expanded mode
    bool expand;
    //! Set true is circuit should be draw in unrolled mode
    bool unroll;
    std::shared_ptr<Circuit> circ;


protected:
    std::vector<unsigned int> lineMap;
};


#endif
