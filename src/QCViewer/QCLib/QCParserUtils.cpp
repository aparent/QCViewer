
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

#include "QCParserUtils.h"
#include <cstdlib>
#include "utility.h"
#include <iostream>
#include "subcircuit.h"

using namespace std;

name_node::name_node(std::string n_name, name_node *n_next)
{
    name = n_name;
    next = n_next;
    neg = false;
}

name_node::name_node(std::string n_name, name_node *n_next, bool n_neg)
{
    name = n_name;
    next = n_next;
    neg = n_neg;
}

name_node::~name_node()
{
    if (next != NULL) delete next;
}

int findLine(Circuit *circ, string name)
{
    for(unsigned int j = 0; j < circ->numLines(); j++) {
        if (name.compare(circ->getLine(j).lineName)==0) {
            return j;
        }
    }
    std::cout << "WARNING: line " << name << " not found" << std::endl;
    return 0;
}

void add_lines (Circuit * circ, name_node *names)
{
    while(names) {
        circ->addLine(names->name);
        names = names->next;
    }
    delete names;
}

void add_inputs (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLineModify(findLine(circ,names->name)).constant=false;
        names = names->next;
    }
    delete names;
}

void add_outputs (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLineModify(findLine(circ,names->name)).garbage=false;
        names = names->next;
    }
    delete names;
}

void add_outlabels (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLineModify(findLine(circ,names->name)).outLabel=names->name;
        names = names->next;
    }
    delete names;
}

void add_constants (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLineModify(findLine(circ,names->name)).initValue = atoi((names->name).c_str());
        names = names->next;
    }
    delete names;
}

void insert_break(Circuit *circ)
{
    circ->column_breaks.push_back(circ->numGates()-1);
}

void add_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp,map<string,Circuit*> &subcircuits)
{
    if (names == NULL) {
        cout << "Gate " << gateName << " has no targets or controls. Skipping." << endl;
        return;
    }
    Gate *newGate = NULL;
    if (sToUpper(gateName)[0] == 'T' && gateName.size()>1 && isdigit(gateName[1])) {
        newGate = new UGate("X");
        newGate->drawType = Gate::NOT;
    } else if (gateName[0] == 'F'||gateName[0] == 'f') {
        newGate = new UGate("F");
        newGate->drawType = Gate::FRED;
    } else if (subcircuits.find(gateName) != subcircuits.end() ) {
        Circuit* c = subcircuits[gateName];
        circ->column_breaks.push_back(circ->numGates()-1);
        map<unsigned int,unsigned int> lineMap;
        int line = 0;
        name_node* start_names = names;
        while(names) {
            lineMap.insert (pair<unsigned int,unsigned int>(line,findLine(circ,names->name)));
            line++;
            names = names->next;
        }
        newGate = new Subcircuit(c, lineMap,exp);
        names = start_names;
        while(names) {
            newGate->targets.push_back(findLine(circ,names->name));
            names = names->next;
        }
    } else {
        gateName = sToUpper(gateName);
        newGate = new UGate(gateName);
    }
    while(names) {
        if (names->next == NULL) {
            newGate->targets.push_back(findLine(circ,names->name));
        } else {
            newGate->controls.push_back(Control(findLine(circ,names->name),names->neg));
        }

        names = names->next;
    }
    if (newGate->getName().compare("F")==0) {
        newGate->targets.push_back(newGate->controls.back().wire);
        newGate->controls.pop_back();
    }
    newGate->setLoopCount(exp);
    circ->addGate(newGate);
    delete names;
}

void add_R_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp, double rot)
{
    RGate::Axis rot_type;
    if (gateName=="RX") {
        rot_type = RGate::X;
    } else if (gateName=="RY") {
        rot_type = RGate::Y;
    } else if (gateName=="RZ") {
        rot_type = RGate::Z;
    } else {
        rot_type = RGate::Z;
    }
    Gate *newGate = new RGate(rot, rot_type);
    while(names) {
        if (names->next == NULL) {
            newGate->targets.push_back(findLine(circ,names->name));
        } else {
            newGate->controls.push_back(Control(findLine(circ,names->name),names->neg));
        }

        names = names->next;
    }
    newGate->setLoopCount(exp);
    circ->addGate(newGate);
    delete names;
}

void link_subcircs(Circuit * circ)
{
    map<string,Circuit*> subcircs = circ->subcircuits;
    for ( map<string,Circuit*>::iterator it = subcircs.begin(); it != subcircs.end(); it++) {
        Circuit *c = it->second;
        for (unsigned int i = 0; i < c->numGates(); i++) {
            Gate *g = c->getGate(i);
            if (subcircs.find(g->getName()) != subcircs.end() ) {
                map<unsigned int,unsigned int> lineMap;
                unsigned int line = 0;
                for(unsigned int j = 0; j < g->controls.size(); j++) {
                    lineMap.insert (pair<unsigned int,unsigned int>(line,g->controls.at(j).wire));
                    line++;
                }
                for(unsigned int j = 0; j < g->targets.size(); j++) {
                    lineMap.insert (pair<unsigned int,unsigned int>(line,g->targets.at(j)));
                    line++;
                }
                Gate *n_g = new Subcircuit(subcircs[g->getName()],lineMap,g->getLoopCount());
                for(unsigned int j = 0; j < g->controls.size(); j++) {
                    n_g->targets.push_back(g->controls.at(j).wire);
                }
                for(unsigned int j = 0; j < g->targets.size(); j++) {
                    n_g->targets.push_back(g->targets.at(j));
                }
                cout << "Set: " << c->getGate(i)->getName() << endl;
                c->setGate(n_g,i);
                //i++;
                //delete g;
            }
        }
    }
}
