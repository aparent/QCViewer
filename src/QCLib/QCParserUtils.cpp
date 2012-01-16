
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

#include "QCParserUtils.h"
#include <cstdlib>
#include "utility.h"
#include <iostream>

int findLine(Circuit *circ, string name)
{
    for(unsigned int j = 0; j < circ->numLines(); j++) {
        if (name.compare(circ->getLine(j)->lineName)==0) {
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
        circ->getLine(findLine(circ,names->name))->constant=false;
        names = names->next;
    }
    delete names;
}

void add_outputs (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLine(findLine(circ,names->name))->garbage=false;
        names = names->next;
    }
    delete names;
}

void add_outlabels (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLine(findLine(circ,names->name))->outLabel=names->name;
        names = names->next;
    }
}

void add_constants (Circuit * circ, name_node *names)
{
    while(names) {
        circ->getLine(findLine(circ,names->name))->initValue = atoi((names->name).c_str());
        names = names->next;
    }
}

void add_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp,map<string,Circuit> &subcircuits)
{
    if (names == NULL) {
        cout << "Gate " << gateName << " has no targets or controls. Skipping." << endl;
        return;
    }
    unsigned int start = circ->numGates();
    bool is_subcirc = false;
    Gate *newGate = NULL;
    if ((gateName[0] == 'T'||gateName[0] == 't') && (gateName.size()==1 || isdigit(gateName[1]))) {
        newGate = new UGate("X");
        newGate->drawType = Gate::NOT;
    } else if (gateName[0] == 'F'||gateName[0] == 'f') {
        newGate = new UGate("F");
        newGate->drawType = Gate::FRED;
    } else if (subcircuits.find(gateName) != subcircuits.end() ) {
        is_subcirc = true;
        Circuit c = subcircuits[gateName];
        circ->column_breaks.push_back(circ->numGates()-1);
        for(unsigned int i = 0; i < c.column_breaks.size(); i++) {
            circ->column_breaks.push_back(c.column_breaks[i] + circ->numGates());
        }
        map<int,int> lineMap;
        map<int,int>::iterator lit=lineMap.begin();
        int line = 0;
        while(names) {
            lineMap.insert (lit, pair<int,int>(line,findLine(circ,names->name)));
            line++;
            names = names->next;
        }
        for (unsigned int i = 0; i < c.numGates(); i++) {
            Gate *g = c.getGate(i)->clone();
            for (unsigned int j = 0; j < c.getGate(i)->controls.size(); j++) {
                g->controls[j].wire = lineMap[c.getGate(i)->controls[j].wire];
            }
            for (unsigned int j = 0; j < c.getGate(i)->targets.size(); j++) {
                g->targets[j] = lineMap[c.getGate(i)->targets[j]];
            }
            circ->addGate(g);
        }
    } else {
        gateName = sToUpper(gateName);
        newGate = new UGate(gateName);
    }
    if (!is_subcirc) {
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
        circ->addGate(newGate);
    }
    if (exp > 1) {
        Loop l;
        l.n = l.sim_n = exp;
        l.first = start;
        l.last = circ->numGates()-1;
        l.label = gateName;
        circ->add_loop(l);
    }
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
    circ->addGate(newGate);
    if (exp > 1) {
        Loop l;
        l.n = l.sim_n = exp;
        l.first = circ->numGates()-1;
        l.label = gateName;
        circ->add_loop(l);
    }
    delete names;
}
