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


#include "circuitParser.h"
#include "utility.h"
#include "subcircuit.h"
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

string allLines(const Circuit &circ)
{
    string ret = "";
    for (unsigned int i = 0 ; i < circ.numLines() ; i++) {
        ret = ret+ " "+circ.getLine(i).lineName;
    }
    return ret;
}

string getGates(const Circuit &circ)
{
    string ret;
    for (unsigned int i = 0; i < circ.numGates(); i++) {
        Gate *gate = circ.getGate(i);
        cout << gate->getName() << endl;
        ret += gate->getName();
        if (gate->type==Gate::SUBCIRC) {
            ret+="^"+intToString(((Subcircuit*)gate)->getLoopCount());
        }
        for (unsigned int j = 0; j < gate->controls.size() ; j++) {
            ret += " " + circ.getLine(gate->controls[j].wire).lineName;
            if (gate->controls[j].polarity==true) {
                ret += "'";
            }
        }
        for (unsigned int j = 0; j < gate->targets.size() ; j++) {
            ret += " " + circ.getLine(gate->targets[j]).lineName;
        }
        ret += "\n";
    }
    return ret;
}

string getSubcircuits(const map<string,Circuit*> &subcircs)
{
    string ret;
    for ( map<string,Circuit*>::const_iterator it = subcircs.begin(); it != subcircs.end(); it++) {
        ret += "BEGIN " + (*it).first + " (";
        for (unsigned int i = 0; i < (*it).second->numLines(); i++) {
            ret += (*it).second->getLine(i).lineName + " ";
        }
        ret+=")\n";
        cout << "Getting gates for: " << ((*it).second)->getName() << endl;
        ret += getGates(*((*it).second));
        ret += "END " + (*it).first +"\n\n";
    }
    return ret;
}

string getGateInfo(const Circuit &circ)
{
    stringstream ret;
    ret << "BEGIN\n";
    ret << getGates(circ);
    ret << "END";
    return ret.str();
}


string getCircuitInfo(const Circuit &circ)
{
    stringstream v,in,o,ol,c,ret;  //correspond to simlarly named sections in the file
    v << ".v" ;
    in << ".i";
    o << ".o";
    ol << ".ol";
    c << ".c";
    for (unsigned int i = 0; i< circ.numLines(); i++) {
        Line line = circ.getLine(i);
        v << " " << line.lineName;
        if (line.constant) {
            c << " " << line.initValue;
        } else {
            in << " " << line.lineName;
        }
        if (!line.garbage) {
            o << " "<< line.lineName;
        }
        if (line.outLabel.compare("")!=0) {
            ol << " "<< line.lineName;
        }
    }
    v << "\n";
    in << "\n";
    o << "\n";
    ol << "\n";
    c << "\n";
    ret << v.str() << in.str() << o.str() << ol.str() << c.str();
    return ret.str();
}


void saveCircuit(Circuit *circ, string filename)
{
    ofstream f;
    f.open (filename.c_str());
    string circInfo = getCircuitInfo(*circ);
    string subcircs = getSubcircuits(circ->subcircuits);
    string gateInfo = getGateInfo(*circ);
    f <<  circInfo << subcircs << gateInfo;
    f.close();
}
