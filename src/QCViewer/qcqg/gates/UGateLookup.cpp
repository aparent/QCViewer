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

#include "UGateLookup.h"
#include "types.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

map<string,gateMatrix> gateLib;

std::vector<std::string> UGateNames()
{
    vector<string> result;
    map<string,gateMatrix>::iterator it;
    for ( it=gateLib.begin() ; it != gateLib.end(); it++ ) {
        if (((*it).first).compare("F") != 0) { //TODO: this is very special case
            result.push_back((*it).first);
        }
    }
    return result;
}

std::vector<std::string> UGateDNames()
{
    vector<string> result;
    map<string,gateMatrix>::iterator it;
    for ( it=gateLib.begin() ; it != gateLib.end(); it++ ) {
        if (((*it).first).compare("F") != 0) { //TODO: this is very special case
            result.push_back(((*it).second).drawName);
        }
    }
    return result;
}

gateMatrix UGateLookup(string name)
{
    if ( gateLib.find(name) == gateLib.end() ) {
        return gateMatrix(0);
    }
    return gateLib[name];
}

void UGateLoad(string name, gateMatrix mat)
{
    if ( gateLib.find(name) == gateLib.end() ) {
        gateLib[name]=mat;
    } else {
        cerr << "WARNING: The gate "<< name << " already exists overwritting..." << endl;
        gateLib[name]=mat;
    }
}

gateMatrix get_matrix(matrix_row *n)
{
    matrix_row* temp = n;
    unsigned int numRow=0;
    while (temp) {
        temp = temp->next;
        numRow++;
    }
    temp = n;
    while (temp) {
        unsigned int numCol = 0;
        row_terms *terms = temp->terms;
        while(terms) {
            terms = terms->next;
            numCol++;
        }
        if (numCol != numRow) {
            cerr << "Invalid Gate Matrix" << endl;
            return gateMatrix(0);
        }
        temp = temp->next;
    }
    gateMatrix ret;
    ret.data = new complex<float_type>[numRow*numRow];
    for(unsigned int i = 0; i < numRow; i++) {
        row_terms *terms = n->terms;
        for(unsigned int j = 0; j < numRow; j++) {
            ret.data[i*numRow + j] = *terms->val;
            terms = terms->next;
        }
        n = n->next;
    }
    ret.dim = numRow;
    return ret;
}

void add_gates(gate_node *n)
{
    gateMatrix g = get_matrix(n->row);
    g.drawName = n->drawName;
    g.latexName = n->latexName;
    if (g.dim != 0) UGateLoad(n->symbol,g);
    if (n->next != NULL) add_gates(n->next);
}

void UGateSetup()
{
    string line,input;
    ifstream myfile ("gateLib");
    if (myfile.is_open()) {
        while ( myfile.good() ) {
            getline (myfile,line);
            input += line + "\n";
        }
    }
    gate_node * node = parse_gates(input);
    add_gates(node);
    delete node;
}
