
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

#ifndef QC_PARSER_UTILS
#define QC_PARSER_UTILS
#include<string>
#include "circuit.h"
class name_node
{
public:
    name_node(std::string n_name, name_node *n_next) {
        name = n_name;
        next = n_next;
        neg = false;
    }
    name_node(std::string n_name, name_node *n_next, bool n_neg) {
        name = n_name;
        next = n_next;
        neg = n_neg;
    }
    ~name_node() {
        if (next != NULL) delete next;
    }
    std::string name;
    bool neg;
    name_node *next;
};

void add_lines (Circuit * circ, name_node *names);
void add_inputs (Circuit * circ, name_node *names);
void add_outputs (Circuit * circ, name_node *names);
void add_constants (Circuit * circ, name_node *names);
void add_outlabels (Circuit * circ, name_node *names);
void add_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp, map<string,Circuit*> &subcircuits);
void add_R_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp, double rot);
#endif
