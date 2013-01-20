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

using namespace std;

Subcircuit::Subcircuit(shared_ptr<Circuit> n_circ, const vector <unsigned int>& n_linemap, unsigned int loops) : Gate()
{
    drawType = DEFAULT;
    circ = n_circ;
    lineMap = n_linemap;
    loop_count = loops;
    expand = false;
    unroll = false;
    ctrls = false;
    simState = shared_ptr<SimState>(new SimState());
}

shared_ptr<Gate> Subcircuit::clone() const
{
    Subcircuit *g  = new Subcircuit(circ,lineMap,loop_count);
    g->controls = controls;
    g->targets = targets;
    g->expand = expand;
    g->simState = simState;
    g->breakpoint = breakpoint;
    return shared_ptr<Gate>(g);
}

string Subcircuit::getName() const
{
    if (circ) return circ->getName();
    return "NULL";
}

std::string Subcircuit::getDrawName()
{
    return getName();
}

std::string Subcircuit::getLatexName()
{
    return getName();
}

void Subcircuit::setName(string n_name)
{
    circ->setName(n_name);
}

unsigned int Subcircuit::getNumGates() const
{
    return circ->totalGates();
}

State Subcircuit::applyToBasis(index_t in) const
{
    State s = State(1,in);
    s = applySubcirc (s);
    return s;
}

State Subcircuit::applySubcirc(const State& in) const
{
    State s = in;
    for (unsigned int i = 0; i < circ->numGates(); i++) {
        s = ApplyGate(s,getGate(i));
    }
    return s;
}

shared_ptr<Gate> Subcircuit::getGate(int pos) const
{
    shared_ptr<Gate> g = circ->getGate(pos)->clone();
    for (unsigned int i = 0; i < g->targets.size(); i++) {
        g->targets.at(i) = lineMap.at(g->targets[i]);
    }
    for (unsigned int i = 0; i < g->controls.size(); i++) {
        g->controls[i].wire = lineMap.at(g->controls[i].wire);
    }
    shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(g);
    if (s) { //Combine the maps if it is a subcircuit so we have the correct global map
        for (unsigned int i = 0; i < s->lineMap.size(); i++) {
            s->lineMap.at(i) = lineMap.at(s->lineMap.at(i));
        }
    }
    return g;
}

unsigned int Subcircuit::numGates() const
{
    return circ->numGates();
}

vector<unsigned int> Subcircuit::getGreedyParallel() const  //Returns a vector of ints specifying the last gate in each parallel block.
{
    return circ->getGreedyParallel();
}


shared_ptr<Circuit> Subcircuit::getCircuit()
{
    return circ;
}

bool Subcircuit::step (State& state)
{
    simState->simulating = true;
    if (simState->gate < numGates () ) {
        shared_ptr<Gate> g = getGate(simState->gate);
        shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(g);
        bool bp = false;
        if (g->breakpoint) {
            bp = true;
        }
        if ( !s || !s->expand ) {
            state = ApplyGate(state,g);
            simState->gate++;
        } else {
            if ( !s->step(state)) {
                simState->gate++;
                step(state);
            }
        }
        if (bp) {
            return false;
        }
        if (s && s->simState->simulating) {
            return false;
        }
        return true;
    }
    if (simState->loop < getLoopCount()) {
        simState->gate = 0;
        simState->nextGate = true;
        simState->loop++;
        step(state);
        return true;
    }
    reset();
    return false;
}

void Subcircuit::reset ()
{
    simState->gate = 0;
    simState->loop = 1;
    simState->nextGate = true;
    simState->simulating = false;
    for ( unsigned int i = 0; i < numGates(); i++ ) {
        shared_ptr<Gate> g = getGate(i);
        shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(g);
        if (s) {
            s->reset();
        }
    }
}
