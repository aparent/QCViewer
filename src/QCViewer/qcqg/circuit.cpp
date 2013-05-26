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

Authors: Alex Parent, Jacob Parker, Marc Burns
---------------------------------------------------------------------*/


#include "circuit.h"
#include "subcircuit.h"
#include "utility.h"
#include <map>
#include <fstream>
#include <iostream>

using namespace std;

Circuit::Circuit()
{
    allExpanded = false;
}

Circuit::~Circuit ()
{ }

void Circuit::expandAll()
{
    allExpanded = !(allExpanded);
    for(unsigned int i = 0; i<numGates(); i++) {
        shared_ptr<Gate> g = getGate(i);
        shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(g);
        if (s) {
            s->expand = allExpanded;
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

unsigned int Circuit::gateCount(string gateName)
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

unsigned int Circuit::depth()
{
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
    returnValue.push_back(numGates()-1);
    return returnValue;
}

void Circuit::setName(string n_name)
{
    name = n_name;
}

string Circuit::getName()
{
    return name;
}

