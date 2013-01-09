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
#include "subcircuit.h"
#include "utility.h"
#include <map>
#include <algorithm> // for sort, which we should probably cut out
#include <fstream>
#include <iostream>
#include "simulate.h"

using namespace std;

Circuit::Circuit()
{
    allExpanded = false;
    arch = NULL;
}

Circuit::~Circuit ()
{
    removeArch ();
}

void Circuit::newArch ()
{
    if (arch) {
        delete arch;
    }
    arch = new QArch (numLines());
}

void Circuit::removeArch ()
{
    if (arch != NULL) {
        delete arch;
        arch = NULL;
    }
}

void Circuit::expandAll()
{
    allExpanded = !(allExpanded);
    for(unsigned int i = 0; i<numGates(); i++) {
        shared_ptr<Gate> g = getGate(i);
        if (g->type == Gate::SUBCIRC) {
            shared_ptr<Subcircuit> sub = dynamic_pointer_cast<Subcircuit>(g);
            sub->expand = allExpanded;
        }
    }
    for ( map<string,std::shared_ptr<Circuit>>::iterator it = subcircuits.begin(); it != subcircuits.end(); ++it) {
        it->second->expandAll();
    }
}

void Circuit::swapGate (unsigned int i, unsigned int j)
{
    swap(gates[i],gates[j]);
}

void Circuit::addGate(shared_ptr<Gate> newGate)
{
    gates.push_back(newGate);
}

void Circuit::addGate(std::shared_ptr<Gate> newGate, unsigned int pos)
{
    gates.insert(gates.begin()+pos, newGate);
}

void Circuit::setGate(std::shared_ptr<Gate> newGate, unsigned int pos)
{
    gates.at(pos) = newGate;
}

void Circuit::removeGate (unsigned int pos)
{
    gates.erase (gates.begin () + pos);
    getGreedyParallel();
}

std::shared_ptr<Gate> Circuit::getGate(int pos) const
{
    return gates.at(pos);
}

unsigned int Circuit::numGates() const
{
    return gates.size();
}

unsigned int Circuit::totalGates() const
{
    unsigned int numGates = 0;
    for(unsigned int i = 0; i < gates.size(); i++) {
        numGates += gates.at(i)->getNumGates();
    }
    return numGates;
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
        shared_ptr<Gate> g = getGate(i);
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
    return returnValue;
}

vector<unsigned int> Circuit::getGreedyParallel()
{
    vector<int> parallel = getParallel (); // doing greedy sometimes "tries too hard"; we need to do greedy within the regions defined here (XXX: explain this better)
    map<int,int> linesUsed;
    unsigned int maxw, minw;
    int k = 0;
    columns.clear ();
    for(unsigned int i = 0; i < numGates(); i++) {
start:
        shared_ptr<Gate> g = getGate(i);
        minmaxWire (g->controls, g->targets, minw, maxw);
        for (unsigned int j = minw; j <= maxw; j++) {
            if (linesUsed.find(j) != linesUsed.end()) {
                columns.push_back(i - 1);
                k++;
                linesUsed.clear ();
                goto start;
            }
            linesUsed[j]; //access element so that it exists in map
        }
        if (g->breakpoint||g->colbreak) {
            columns.push_back (i);
            k++;
            linesUsed.clear ();
            continue;
        }
    }
    for (; k < (int)parallel.size(); k++) {
        columns.push_back (parallel.at(k) );
    }
    sort (columns.begin (), columns.end ()); // TODO: needed?
    columns.push_back (numGates()-1); // for convenience.
    return columns;
}

vector<int> Circuit::getArchWarnings () const
{
    vector<int> warnings;
    vector<unsigned int> wires;
    if (arch == 0) return warnings; // Assume "no" architecture by default.
    for (unsigned int g = 0; g < gates.size(); g++) {
        wires = getGate(g)->targets;
        shared_ptr<Gate> gg = getGate (g);
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
    name = n_name;
}

string Circuit::getName()
{
    return name;
}

bool Circuit::run (State& state)
{
    simState.simulating = true;
    if (simState.gate == numGates ()) simState.gate = 0;
    // always step over first breakpoint if it is around
    while (simState.gate < numGates ()) {
        bool bp = false;
        // check if we have reached a breakpoint
        shared_ptr<Gate> g = getGate (simState.gate);
        if (g->breakpoint) bp = true;

        if (g->type != Gate::SUBCIRC || !dynamic_pointer_cast<Subcircuit>(g)->expand ) {
            state = ApplyGate (state, getGate (simState.gate));
            simState.gate++;
        }	else {
            shared_ptr<Subcircuit> sub = dynamic_pointer_cast<Subcircuit>(g);
            while (sub->step(state)) {
                cout << "sStep" << endl;
            }
            if( sub->simState->simulating == false) {
                simState.gate++;
            } else {
                return true;
            }
        }
        if (bp) {
            cout << "BREAK" << endl;
            return true;
        }
    }
    return false;
}

void Circuit::reset ()
{
    simState.gate = 0;
    simState.nextGate = true;
    simState.simulating = false;
    for ( unsigned int i = 0; i < numGates(); i++ ) {
        shared_ptr<Gate> g = getGate(i);
        if (g->type == Gate::SUBCIRC) {
            shared_ptr<Subcircuit> sub = dynamic_pointer_cast<Subcircuit>(g);
            sub->reset();
        }
    }
}

bool Circuit::step (State& state)
{
    simState.simulating = true;
    if (simState.gate < numGates () ) {
        shared_ptr<Gate> g = getGate(simState.gate);
        if (g->type != Gate::SUBCIRC || !dynamic_pointer_cast<Subcircuit>(g)->expand ) {
            state = ApplyGate(state,g);
            simState.gate++;
        } else {
            shared_ptr<Subcircuit> sub = dynamic_pointer_cast<Subcircuit>(g);
            if (!sub->step(state) && !sub->simState->simulating) {
                simState.gate++;
                step(state);
            }
        }
        return true;
    }
    return false;
}
