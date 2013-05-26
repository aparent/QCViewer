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

Authors: Alex Parent, Marc Burns
---------------------------------------------------------------------*/

#include "QCParserUtils.h"
#include <cstdlib>
#include <cctype>
#include "utility.h"
#include <iostream>
#include <vector>
#include "subcircuit.h"
#include "gates/UGateLookup.h"

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

int findLine(std::shared_ptr<Circuit>circ, string name)
{
    for(unsigned int j = 0; j < circ->numLines(); j++) {
        if (name.compare(circ->getLine(j).lineName)==0) {
            return j;
        }
    }
    std::cout << "WARNING: line " << name << " not found" << std::endl;
    return -1;
}

bool check_names (std::shared_ptr<Circuit> circ, name_node *names,vector<string>& error_log,string id)
{
    name_node *pos = names;
    while(pos) {
        if (findLine(circ,pos->name) == -1) {
            error_log.push_back("Error: line " + pos->name + " in " + id +" not found.");
            return false;
        }
        pos = pos->next;
    }
    return true;
}

void add_lines (std::shared_ptr<Circuit> circ, name_node *names)
{
    while(names) {
        circ->addLine(names->name);
        names = names->next;
    }
    delete names;
}

void add_inputs (std::shared_ptr<Circuit> circ, name_node *names)
{
    while(names) {
        circ->getLineModify(findLine(circ,names->name)).constant=false;
        names = names->next;
    }
    delete names;
}

void add_outputs (std::shared_ptr<Circuit> circ, name_node *names)
{
    while(names) {
        circ->getLineModify(findLine(circ,names->name)).garbage=false;
        names = names->next;
    }
    delete names;
}

void add_outlabels (std::shared_ptr<Circuit> circ, name_node *names)
{
    unsigned int i = 0;
    while(names && i < circ->numLines()) {
        circ->getLineModify(i).outLabel=names->name;
        names = names->next;
        i++;
    }
    delete names;
}

void add_constants (std::shared_ptr<Circuit> circ, name_node *names)
{
  (void)circ;
    //while(names) {
    //circ->getLineModify(findLine(circ,names->name)).initValue = atoi((names->name).c_str()); //TODO:FIXME
    //names = names->next;
    //}
    delete names;
}

void insert_break(std::shared_ptr<Circuit>circ)
{
    if (circ->numGates() > 0)
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

//! Assigns the gate as a UGATE unless it is one of the special types
shared_ptr<Gate> setup_gate_type(string gateName)
{
    if ((sToUpper(gateName)[0] == 'T' && gateName.size()>1 &&
            isdigit(gateName[1])) ||
            (sToUpper(gateName).compare("TOF") == 0)||
            (sToUpper(gateName).compare("NOT") == 0)||
            (sToUpper(gateName).compare("CNOT") == 0)) {
        shared_ptr<Gate> newGate;
        newGate = shared_ptr<Gate>(new UGate("X"));
        newGate->drawType = Gate::NOT;
        return newGate;
    } else if (gateName[0] == 'F'||gateName[0] == 'f'||
               (sToUpper(gateName).compare("FRE") == 0)||
               (sToUpper(gateName).compare("SWAP") == 0)) {
        shared_ptr<Gate> newGate;
        newGate = shared_ptr<Gate>(new UGate("F"));
        newGate->drawType = Gate::FRED;
        return newGate;
    } else if (sToUpper(gateName).compare("MEASURE")==0) {
        shared_ptr<Gate> newGate;
        newGate = shared_ptr<Gate>(new UGate("MEASURE",""));
        newGate->drawType = Gate::MEASURE;
        return newGate;
    } else if (sToUpper(gateName).compare("SELECTZERO")==0) {
        shared_ptr<Gate> newGate;
        newGate = shared_ptr<Gate>(new UGate("SELECTZERO",""));
        newGate->drawType = Gate::SELECTZERO;
        return newGate;
    } else if (sToUpper(gateName).compare("SELECTONE")==0) {
        shared_ptr<Gate> newGate;
        newGate = shared_ptr<Gate>(new UGate("SELECTONE",""));
        newGate->drawType = Gate::SELECTONE;
        return newGate;
    } else {
        gateName = sToUpper(gateName);
        return shared_ptr<Gate>(new UGate(gateName));
    }
}

//! fixes special case for the F gate which has 2 targets
void fix_F_gate(shared_ptr<Gate> g)
{
    if (g->getName().compare("F")==0) {
        g->targets.push_back(g->controls.back().wire);
        g->controls.pop_back();
    }
}

//! For gates without explicitly sepaarated targets and controls
void add_gate (std::shared_ptr<Circuit> circ, string gateName, name_node *names, unsigned int exp, vector<string>& error_log)
{
    if (names == NULL) {
        error_log.push_back("Gate " + gateName + " has no targets or controls. Skipping.");
        return;
    }
    shared_ptr<Gate> newGate = setup_gate_type(gateName);
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
        error_log.push_back("Duplicate targets or controls on: " + gateName);
        return;
    }
    fix_F_gate(newGate);
    newGate->setLoopCount(exp);
    newGate->ctrls = false;
    circ->addGate(newGate);
    delete names;
}

//! For gates that have explicitly separated targets and controls
void add_gate (std::shared_ptr<Circuit> circ, string gateName, name_node *controls,name_node *targets, unsigned int exp, vector<string>& error_log)
{
    if (targets == NULL) {
        error_log.push_back("Gate " + gateName + " has no targets or controls. Skipping.");
        return;
    }
    shared_ptr<Gate> newGate = setup_gate_type(gateName);
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
        error_log.push_back("Duplicate targets or controls on: " + gateName);
        return;
    }
    fix_F_gate(newGate);
    newGate->setLoopCount(exp);
    newGate->ctrls = true;
    circ->addGate(newGate);
    delete targets;
    delete controls;
}

void addRGate (std::shared_ptr<Circuit> circ, string gateName, name_node *names, unsigned int exp, double rot)
{
    RGate::Axis rot_type;
    if (gateName=="RX") {
        rot_type = RGate::X;
    } else if (gateName=="RY") {
        rot_type = RGate::Y;
    } else if (gateName=="RZ") {
        rot_type = RGate::Z;
    } else {
        cout << "Bad R-Gate type. See: " << __FILE__ << __LINE__ << endl;
    }
    shared_ptr<Gate> g = shared_ptr<Gate>(new RGate(rot, rot_type));
    while(names) {
        if (names->next == NULL) {
            g->targets.push_back(findLine(circ,names->name));
        } else {
            g->controls.push_back(Control(findLine(circ,names->name),names->neg));
        }
        names = names->next;
    }

    g->setLoopCount(exp);
    g->ctrls = false;
    circ->addGate(g);
    delete names;
}


void addFracRGate (std::shared_ptr<Circuit> circ, string gateName, name_node *names, unsigned int exp, int numer, int denom)
{
    RGate::Axis rot_type;
    if (gateName=="RX") {
        rot_type = RGate::X;
    } else if (gateName=="RY") {
        rot_type = RGate::Y;
    } else if (gateName=="RZ") {
        rot_type = RGate::Z;
    } else {
        cout << "Bad R-Gate type. See: " << __FILE__ << __LINE__ << endl;
    }
    shared_ptr<Gate> g = shared_ptr<Gate>(new RGate(rot_type,numer,denom));
    while(names) {
        if (names->next == NULL) {
            g->targets.push_back(findLine(circ,names->name));
        } else {
            g->controls.push_back(Control(findLine(circ,names->name),names->neg));
        }
        names = names->next;
    }
    g->setLoopCount(exp);
    g->ctrls = false;
    circ->addGate(g);
    delete names;
}

void add_one_bit_gates (shared_ptr<Circuit> circ, string qubit, string gateStr)
{
    unsigned int line = findLine(circ,qubit);
    for(unsigned int i = 0; i < gateStr.size(); i++) {
        shared_ptr<Gate> newGate;
        string name = string(1,toupper(gateStr.at(i)));
        if ( (i+1) < gateStr.size() && gateStr[i+1] == '*') {
            name += gateStr[i+1];
            i++;
        }
        newGate = shared_ptr<Gate>(new UGate(name));
        newGate->targets.push_back(line);
        newGate->ctrls = false;
        circ->addGate(newGate);
    }
}

void link_circuit(shared_ptr<Circuit> c, const map<string,shared_ptr<Circuit>> &subcircs)
{
    for (unsigned int i = 0; i < c->numGates(); i++) {
        shared_ptr<Gate> g = c->getGate(i);
        if (subcircs.find(g->getName()) != subcircs.end() ) {
            vector<unsigned int> lineMap;
            if (!g->ctrls) {
                for(unsigned int j = 0; j < g->controls.size(); j++) {
                    lineMap.push_back (g->controls.at(j).wire);
                }
            }
            for(unsigned int j = 0; j < g->targets.size(); j++) {
                lineMap.push_back (g->targets.at(j));
            }
            shared_ptr<Gate> n_g = shared_ptr<Gate>(new Subcircuit(subcircs.find(g->getName())->second,lineMap,g->getLoopCount()));
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
        }
    }
}

void link_subcircs(std::shared_ptr<Circuit> circ)
{
    map<string,shared_ptr<Circuit>> subcircs = circ->subcircuits;
    for ( map<string,shared_ptr<Circuit>>::iterator it = subcircs.begin(); it != subcircs.end(); ++it) {
        shared_ptr<Circuit> c = it->second;
        link_circuit(c,subcircs);
    }
    link_circuit(circ, subcircs);
}



void remove_bad_gates(std::shared_ptr<Circuit> c, vector<string>& error_log )
{
    (void)c;
    (void)error_log;
    /*for (unsigned int i = 0; i < c->numGates(); i++) {
        shared_ptr<Gate> g = c->getGate(i);
        shared_ptr<Subcircuit> s = dynamic_pointer_cast<Subcircuit>(g);
        shared_ptr<RGate>      r = dynamic_pointer_cast<RGate>(g);
        string name = g->getName();
        if ( !r &&
                !s &&
                g->drawType!=Gate::MEASURE &&
                g->drawType!=Gate::SELECTZERO &&
                g->drawType!=Gate::SELECTONE &&
                name.compare("tof")!=0 &&
                UGateLookup(name).dim == 0 ) {
            error_log.push_back("Warning: Gate '" + name + "' is unrecognized.");
        }
    }*/
}

void cleanup_bad_gates(std::shared_ptr<Circuit> circ, vector<string>& error_log)
{
    remove_bad_gates (circ,error_log);
    map<string,shared_ptr<Circuit>> subcircs = circ->subcircuits;
    for ( map<string,shared_ptr<Circuit>>::iterator it = subcircs.begin(); it != subcircs.end(); ++it) {
        remove_bad_gates(it->second, error_log);
    }
}
