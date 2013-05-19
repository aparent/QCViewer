/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011 Alex Parent and The University of Waterloo,
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


#include "subcircuit.h"

using namespace std;

Subcircuit::Subcircuit() : Gate()
{ }

shared_ptr<Gate> Subcircuit::clone() const
{
  // XXX
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

void Subcircuit::setName(string n_name)
{
    circ->setName(n_name);
}

unsigned int Subcircuit::getNumGates() const
{
    return circ->totalGates();
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

void Subcircuit::swapGate (unsigned int i, unsigned int j)
{
    swap(gates[i],gates[j]);
}

void Subcircuit::addGate(shared_ptr<Gate> newGate)
{
    gates.push_back(newGate);
}

void Subcircuit::addGate(std::shared_ptr<Gate> newGate, unsigned int pos)
{
    gates.insert(gates.begin()+pos, newGate);
}

void Subcircuit::setGate(std::shared_ptr<Gate> newGate, unsigned int pos)
{
    gates.at(pos) = newGate;
}

void Subcircuit::removeGate (unsigned int pos)
{
    gates.erase (gates.begin () + pos);
}

std::shared_ptr<Gate> Subcircuit::getGate(int pos) const
{
    return gates.at(pos);
}

unsigned int Subcircuit::numGates() const
{
    return gates.size();
}

unsigned int Subcircuit::totalGates() const
{
    unsigned int numGates = 0;
    for(unsigned int i = 0; i < gates.size(); i++) {
        numGates += gates.at(i)->getNumGates();
    }
    return numGates;
}

unsigned int Subcircuit::gateCount(string gateName)
{
    unsigned int numGates = 0;
    for(unsigned int i = 0; i < gates.size(); i++) {
        shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(gates.at(i));
        if (s) {
            numGates += s->getCircuit()->gateCount(gateName);
        } else if (gates.at(i)->getName().compare(gateName) == 0) {
            numGates++;
        }
    }
    return numGates;
}

unsigned int Subcircuit::depth()
{
  // XXX replace with simpler algorithm
    unsigned int depth = 0;
    for(unsigned int i = 0; i < gates.size(); i++) {
        shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(gates.at(i));
        if (s) {
            depth += s->getCircuit()->depth()-1;
        }
    }
    depth += getParallel().size() + 1;
    return depth;
}

unsigned int Subcircuit::numLines() const
{
    return lines.size();
}

const Line& Subcircuit::getLine(int pos) const
{
    return lines.at(pos);
}

Line& Subcircuit::getLineModify(int pos)
{
    return lines.at(pos);
}

void Subcircuit::addLine(string lineName)
{
    lines.push_back(Line(lineName));
}

Line::Line(string name)
{
    lineName  = name;
    garbage   = true;
    constant  = true;
    initValue = 0;
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

