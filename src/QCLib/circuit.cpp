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

Authors: Alex Parent, Jakub Parker
---------------------------------------------------------------------*/


#include "circuit.h"
#include "utility.h"
#include <map>
#include <algorithm> // for sort, which we should probably cut out
#include <fstream>
#include <iostream>

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
	for ( map<string,Circuit*>::iterator it = subcircuits.begin() ; it != subcircuits.end(); it++ ){
    delete (*it).second;
	}
}

void Circuit::removeGates(){
	for(unsigned int i = 0; i < gates.size(); i++){
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
}

void Circuit::addGate(Gate *newGate, unsigned int pos)
{
    gates.insert(gates.begin()+pos, newGate);
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

string Line::getInputLabel()
{
    if (constant) {
        return intToString(initValue);
    }
    return lineName;
}

string Line::getOutputLabel()
{
    if (garbage) {
        return "Garbage";
    }
    if (outLabel.compare("")==0) {
        return lineName;
    }
    return outLabel;
}

unsigned int Circuit::numLines()
{
    return lines.size();
}

Line* Circuit::getLine(int pos)
{
    return &lines.at(pos);
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
vector<int> Circuit::getGreedyParallel() const
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
    return returnValue;
}

vector<int> Circuit::getArchWarnings ()
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

void Circuit::add_loop (Loop l)
{
    for (vector<Loop>::iterator it = loops.begin(); it != loops.end(); it++) {
        // make sure that this loop is either distinct from or contained in every other loop
        if (l.last < it->first || it->last < l.first) continue; // distinct
// for now loops cannot overlap
        std::cout << "failed!\n";
        std::cout << "candidate: " << l.first << " " << l.last <<"\n";
        std::cout << "opposer: " << it->first << " " << it->last << "\n";

        return; // don't add this loop then
    }
    loops.push_back(l);
    //std::cout << "\nnew loop!\n";
}

void Circuit::setName(string n_name)
{
	this->name = n_name;
}

string Circuit::getName()
{
		return this->name;
}
