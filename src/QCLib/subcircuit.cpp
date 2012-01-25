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


#include "subcircuit.h"
#include "simulate.h"
#include <iostream> //XXX
Subcircuit::Subcircuit(Circuit* n_circ, map <int,int> n_linemap, int loops)
{
 	type = SUBCIRC;
	circ = n_circ;
	lineMap = n_linemap;
	loop_count = loops;
}

Gate* Subcircuit::clone()
{
    Subcircuit* g = new Subcircuit(circ,lineMap,loop_count);
    g->controls = controls;
    g->targets = targets;
    return g;
}

string Subcircuit::getName() const
{
	if (circ != NULL) return circ->getName();
	return "NULL";
}

int Subcircuit::getLoopCount() const
{
	return loop_count; 
}

void Subcircuit::setLoopCount(int loops)
{
	loop_count = loops;
}


State Subcircuit::applyToBasis(index_t in) const
{
	
	State s = State(1,in); 
	for (int i = 0; i < loop_count; i++){
		s = ApplyCircuit (s, *circ);
	}
	s.print();
	return s;
}
