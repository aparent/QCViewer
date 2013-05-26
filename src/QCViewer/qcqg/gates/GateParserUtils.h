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

#ifndef GATE_PARSE_UTILS
#define GATE_PARSE_UTILS
#include <complex>
#include <string>
struct row_terms {
    std::complex<float> *val;
    row_terms *next;
    row_terms(std::complex<float> *n_val, row_terms *n_next) {
        next = n_next;
        val = n_val;
    }
    row_terms(std::complex<float> *n_val) {
        next = NULL;
        val = n_val;
    }
    ~row_terms() {
        if (next!= NULL) delete next;
        if (val != NULL) delete val;
    }
};

struct matrix_row {
    row_terms *terms;
    matrix_row *next;
    matrix_row(matrix_row *n_next) {
        next = n_next;
        terms = NULL;
    }
    matrix_row(row_terms *n_val, matrix_row *n_next) {
        next = n_next;
        terms = n_val;
    }
    ~matrix_row() {
        if (next!= NULL) delete next;
        if (terms != NULL) delete terms;
    }
};

struct gate_node {
    gate_node* next;
    std::string name;
    std::string drawName;
    std::string latexName;
    std::string symbol;
    matrix_row *row;
    gate_node(char* n_name, char* n_dname, char* n_latex_name, char* n_symbol, matrix_row *n_row )
        : name(n_name), drawName(n_dname), latexName(n_latex_name), symbol(n_symbol)  {
        next = NULL;
        row = n_row;
    }
    gate_node(char* n_name, char* n_symbol, matrix_row *n_row )
        : name(n_name), drawName(n_symbol), latexName(n_symbol), symbol(n_symbol)  {
        next = NULL;
        row = n_row;
    }
    gate_node(char* n_name, char* n_dname, char* n_symbol, matrix_row *n_row )
        : name(n_name), drawName(n_dname), latexName(n_symbol), symbol(n_symbol) {
        next = NULL;
        row = n_row;
    }
    ~gate_node() {
        if (next!= NULL) delete next;
        if (row != NULL) delete row;
    }
};
#endif
