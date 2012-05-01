
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

#include "QCParserUtils.h"
#include <cstdlib>
#include "utility.h"
#include <iostream>
#include <vector>
#include "subcircuit.h"
#include "QCLib/gates/UGateLookup.h"

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
    int i = 0;
    while(names&& i < circ->numLines()) {
        circ->getLineModify(i).outLabel=names->name;
        names = names->next;
        i++;
    }
    delete names;
}

void add_constants (Circuit * circ, name_node *names)
{
    //while(names) {
    //circ->getLineModify(findLine(circ,names->name)).initValue = atoi((names->name).c_str()); //TODO:FIXME
    //names = names->next;
    //}
    delete names;
}

void insert_break(Circuit *circ)
{
    circ->getGate(circ->numGates()-1)->colbreak = true;
}

bool check_dup(name_node *names)
{
    if (names == NULL) {
        return false;
    } else {
        name_node *check = names->next;
        while(check) {
            if (names->name == check->name) {
                return true;
            }
            check = check->next;
        }
    }
    return check_dup(names->next);
}

void add_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp,map<string,Circuit*> &subcircuits, vector<string>& error_log)
{
    if (names == NULL) {
        cout << "Gate " << gateName << " has no targets or controls. Skipping." << endl;
        return;
    }
    Gate *newGate = NULL;
    if ((sToUpper(gateName)[0] == 'T' && gateName.size()>1 && isdigit(gateName[1])) ||
            (sToUpper(gateName).compare("TOF") == 0)||
            (sToUpper(gateName).compare("NOT") == 0)||
            (sToUpper(gateName).compare("CNOT") == 0)) {
        newGate = new UGate("X");
        newGate->drawType = Gate::NOT;
    } else if (gateName[0] == 'F'||gateName[0] == 'f'||
               (sToUpper(gateName).compare("FRE") == 0)||
               (sToUpper(gateName).compare("SWAP") == 0)) {
        newGate = new UGate("F");
        newGate->drawType = Gate::FRED;
    } else if (subcircuits.find(gateName) != subcircuits.end() ) {
        Circuit* c = subcircuits[gateName];
        if (circ->numGates()>0) {
            circ->getGate(circ->numGates()-1)->colbreak = true;
        }
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
    if(!check_dup(names)) {
        while(names) {
            if (names->next == NULL) {
                newGate->targets.push_back(findLine(circ,names->name));
            } else {
                newGate->controls.push_back(Control(findLine(circ,names->name),names->neg));
            }
            names = names->next;
        }
    } else {
        cout << "Duplicate targets or controls on: " << gateName << endl;
        delete newGate;
        return;
    }
    if (newGate->getName().compare("F")==0) {
        newGate->targets.push_back(newGate->controls.back().wire);
        newGate->controls.pop_back();
    }
    newGate->setLoopCount(exp);
    newGate->ctrls = false;
    circ->addGate(newGate);
    delete names;
}

void add_gate (Circuit * circ, string gateName, name_node *controls,name_node *targets, unsigned int exp,map<string,Circuit*> &subcircuits, vector<string>& error_log)
{
    if (targets == NULL) {
        cout << "Gate " << gateName << " has no targets or controls. Skipping." << endl;
        return;
    }
    Gate *newGate = NULL;
    if ((sToUpper(gateName)[0] == 'T' && gateName.size()>1 && isdigit(gateName[1])) ||
            (sToUpper(gateName).compare("TOF") == 0)||
            (sToUpper(gateName).compare("NOT") == 0)||
            (sToUpper(gateName).compare("CNOT") == 0)) {
        newGate = new UGate("X");
        newGate->drawType = Gate::NOT;
    } else if (gateName[0] == 'F'||gateName[0] == 'f'||
               (sToUpper(gateName).compare("FRE") == 0)||
               (sToUpper(gateName).compare("SWAP") == 0)) {
        newGate = new UGate("F");
        newGate->drawType = Gate::FRED;
    } else if (subcircuits.find(gateName) != subcircuits.end() ) {
        Circuit* c = subcircuits[gateName];
        if (circ->numGates()>0) {
            circ->getGate(circ->numGates()-1)->colbreak = true;
        }
        map<unsigned int,unsigned int> lineMap;
        int line = 0;
        name_node* start_targs = targets;
        while(targets) {
            lineMap.insert (pair<unsigned int,unsigned int>(line,findLine(circ,targets->name)));
            line++;
            targets = targets->next;
        }
        newGate = new Subcircuit(c, lineMap,exp);
        targets = start_targs;
        while(targets) {
            newGate->targets.push_back(findLine(circ,targets->name));
            cout << "target " << targets->name << " on gate " << gateName << endl;
            targets = targets->next;
        }
        while(controls) {
            newGate->controls.push_back(Control(findLine(circ,controls->name),controls->neg));
            cout << "control " << controls->name << " on gate " << gateName << endl;
            controls = controls->next;
        }
    } else {
        gateName = sToUpper(gateName);
        newGate = new UGate(gateName);
    }
    if(!check_dup(targets)&&!check_dup(controls)) {
        while(targets) {
            newGate->targets.push_back(findLine(circ,targets->name));
            targets = targets->next;
        }
        while(controls) {
            newGate->controls.push_back(Control(findLine(circ,controls->name),controls->neg));
            controls = controls->next;
        }
    } else {
        cout << "Duplicate targets or controls on: " << gateName << endl;
        delete newGate;
        return;
    }
    if (newGate->getName().compare("F")==0) {
        newGate->targets.push_back(newGate->controls.back().wire);
        newGate->controls.pop_back();
    }
    newGate->setLoopCount(exp);

    cout << newGate->getName() << ": t " << newGate->targets.size() << " c " << newGate->controls.size() << endl;
    newGate->ctrls = true;
    circ->addGate(newGate);
    delete targets;
    delete controls;
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
    newGate->ctrls = false;
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
            cout << g->getName() << ": t " << g->targets.size() << " c " << g->controls.size() << endl;
            if (subcircs.find(g->getName()) != subcircs.end() ) {
                map<unsigned int,unsigned int> lineMap;
                unsigned int line = 0;
                if (!g->ctrls) {
                    for(unsigned int j = 0; j < g->controls.size(); j++) {
                        lineMap.insert (pair<unsigned int,unsigned int>(line,g->controls.at(j).wire));
                        line++;
                    }
                }
                for(unsigned int j = 0; j < g->targets.size(); j++) {
                    lineMap.insert (pair<unsigned int,unsigned int>(line,g->targets.at(j)));
                    line++;
                }
                Gate *n_g = new Subcircuit(subcircs[g->getName()],lineMap,g->getLoopCount());
                if (g->ctrls) {
                    for(unsigned int j = 0; j < g->controls.size(); j++) {
                        n_g->controls.push_back(g->controls.at(j));
                    }
                } else {
                    for(unsigned int j = 0; j < g->controls.size(); j++) {
                        n_g->targets.push_back(g->controls.at(j).wire);
                    }
                }
                for(unsigned int j = 0; j < g->targets.size(); j++) {
                    n_g->targets.push_back(g->targets.at(j));
                }
                c->setGate(n_g,i);
                delete g;
            }
        }
    }
}

void remove_bad_gates(Circuit * c, vector<string>& error_log )
{
    for (unsigned int i = 0; i < c->numGates(); i++) {
        string name = c->getGate(i)->getName();
        if (c->getGate(i)->type!=Gate::RGATE && c->getGate(i)->type!=Gate::SUBCIRC && name.compare("tof")!=0 && UGateLookup(name) == NULL ) {
            error_log.push_back("Gate: " + name + " is unrecognized. Excluding.");
            c->removeGate (i);
            i--;
        }
    }
}

void cleanup_bad_gates(Circuit * circ, vector<string>& error_log)
{
    remove_bad_gates (circ,error_log);
    map<string,Circuit*> subcircs = circ->subcircuits;
    for ( map<string,Circuit*>::iterator it = subcircs.begin(); it != subcircs.end(); it++) {
        remove_bad_gates(it->second, error_log);
    }
}
