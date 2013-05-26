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

#ifndef QC_PARSER_UTILS
#define QC_PARSER_UTILS
#include <string>
#include "circuit.h"
class name_node
{
public:
    name_node(std::string n_name, name_node *n_next);
    name_node(std::string n_name, name_node *n_next, bool n_neg);
    ~name_node();
    std::string name;
    bool neg;
    name_node *next;
};

void add_one_bit_gates (std::shared_ptr<Circuit> circ, std::string qubit, std::string gateStr);
void cleanup_bad_gates(std::shared_ptr<Circuit> c,std::vector<std::string>& error_log);
void add_lines (std::shared_ptr<Circuit> circ, name_node *names);
void add_inputs (std::shared_ptr<Circuit> circ, name_node *names);
void add_outputs (std::shared_ptr<Circuit> circ, name_node *names);
void add_constants (std::shared_ptr<Circuit> circ, name_node *names);
void add_outlabels (std::shared_ptr<Circuit> circ, name_node *names);
void add_gate (std::shared_ptr<Circuit> circ, std::string gateName, name_node *names, unsigned int exp,std::vector<std::string>& error_log);
void add_gate (std::shared_ptr<Circuit> circ, std::string gateName, name_node *controls,name_node *targets, unsigned int exp, std::vector<std::string>& error_log);
void addRGate (std::shared_ptr<Circuit> circ, std::string gateName, name_node *names, unsigned int exp, double rot);
void addFracRGate (std::shared_ptr<Circuit> circ, std::string gateName, name_node *names, unsigned int exp, int num, int denom);
void link_subcircs (std::shared_ptr<Circuit> circ);
void insert_break(std::shared_ptr<Circuit>circ);
//! Checks to see if lines with these names exist in the circuit
bool check_names (std::shared_ptr<Circuit> circ, name_node *names,std::vector<std::string>& error_log,std::string id);
#endif
