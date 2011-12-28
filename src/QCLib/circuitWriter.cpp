#include "circuitParser.h"
#include "utility.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>


string allLines(Circuit &circ)
{
    string ret = "";
    for (unsigned int i = 0 ; i < circ.numLines() ; i++) {
        ret = ret+ " "+circ.getLine(i)->lineName;
    }
    return ret;
}

string getGates(string ret, Circuit &circ, unsigned int a, unsigned int b)
{
    for (unsigned int i = a; i <= b; i++) {
        bool subcirc = false;
        for (unsigned int j = 0 ; j < circ.loops.size(); j++) {
            if ( i == circ.loops[j].first && i<circ.loops[j].last) {
                circ.loops[j].first = -1;
                ret += circ.loops[j].label + "^" + intToString(circ.loops[j].n) + " " + allLines(circ) + "\n";
                string sub_s =  "\nBEGIN " + circ.loops[j].label + "(" + allLines(circ) + ")" + "\n";
                string sub = getGates(sub_s,circ,i,circ.loops[j].last);
                sub += "END " + circ.loops[j].label + "\n\n";
                ret = sub + ret;
                i = circ.loops[j].last;
                subcirc = true;
            } else if ( i == circ.loops[j].first && i == circ.loops[j].last) {
                ret += circ.loops[j].label + "^" + intToString(circ.loops[j].n) ;
                Gate *gate = circ.getGate(i);
                for (unsigned int j = 0; j < gate->controls.size() ; j++) {
                    ret += " " + circ.getLine(gate->controls[j].wire)->lineName;
                }
                for (unsigned int j = 0; j < gate->targets.size() ; j++) {
                    ret += " " + circ.getLine(gate->targets[j])->lineName;
                }
                ret += "\n";
                subcirc = true;
            }
            if (subcirc) break;
        }
        if (subcirc) continue;
        Gate *gate = circ.getGate(i);
        ret += gate->getName();
        for (unsigned int j = 0; j < gate->controls.size() ; j++) {
            ret += " " + circ.getLine(gate->controls[j].wire)->lineName;
        }
        for (unsigned int j = 0; j < gate->targets.size() ; j++) {
            ret += " " + circ.getLine(gate->targets[j])->lineName;
        }
        ret += "\n";
    }
    return ret;
}

string getGateInfo(Circuit &circ)
{
    stringstream ret;
    ret << getGates("BEGIN\n",circ, 0,circ.numGates()-1);
    ret << "END";
    return ret.str();
}


string getCircuitInfo(Circuit &circ)
{
    stringstream v,in,o,ol,c,ret;  //correspond to simlarly named sections in the file
    v << ".v" ;
    in << ".i";
    o << ".o";
    ol << ".ol";
    c << ".c";
    Line *line;  //for current line
    for (unsigned int i = 0; i< circ.numLines(); i++) {
        line = circ.getLine(i);
        v << " " << line->lineName;
        if (line->constant) {
            c << " " << line->initValue;
        } else {
            in << " " << line->lineName;
        }
        if (!line->garbage) {
            o << " "<< line->lineName;
        }
        if (line->outLabel.compare("")!=0) {
            ol << " "<< line->lineName;
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
    Circuit c = *circ;
    f.open (filename.c_str());
    string circInfo = getCircuitInfo(c);
    string gateInfo = getGateInfo(c);
    f << circInfo << gateInfo;
    f.close();
}
