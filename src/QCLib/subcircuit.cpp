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
Subcircuit::Subcircuit(Circuit* n_circ, map <unsigned int,unsigned int> n_linemap, int loops)
{
  drawType = D_SUBCIRC;
 	type = SUBCIRC;
	circ = n_circ;
	lineMap = n_linemap;
	loop_count = loops;
}

Gate* Subcircuit::clone() const
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
	s = applySubcirc (s);
	return s;
}

State Subcircuit::applySubcirc(const State& in) const{
	State s = in;
	for (int i = 0; i < loop_count; i++){
    for (unsigned int i = 0; i < circ->numGates(); i++) {
        s = ApplyGate(s,getGate(i));
    }
	}
	return s;
}

Gate* Subcircuit::getGate(int pos) const{
  Gate *g = circ->getGate(pos)->clone();
  for (unsigned int i = 0; i < g->targets.size(); i++) {
		map<unsigned int,unsigned int>::const_iterator it = lineMap.find(g->targets[i]);
		if (it!= lineMap.end()){
			g->targets[i] = it->second;	
			std::cout << g->targets[i] << ":" << it->second << std::endl;
		}
	}
  for (unsigned int i = 0; i < g->controls.size(); i++) {
		map<unsigned int,unsigned int>::const_iterator it = lineMap.find(g->controls[i].wire);
		if (it!= lineMap.end()){
			g->controls[i].wire = it->second;	
		}
	}
	return g;
}

int Subcircuit::numGates() const
{
	return circ->numGates();
}

vector<int> Subcircuit::getGreedyParallel() const  //Returns a vector of ints specifying the last gate in each parallel block.
{
	return circ->getGreedyParallel();
}
