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
    std::string symbol;
    matrix_row *row;
    gate_node(char* n_name, char* n_symbol, matrix_row *n_row ) {
        next = NULL;
        row = n_row;
        name = std::string(n_name);
        symbol = std::string(n_symbol);
    }
    ~gate_node() {
        if (next!= NULL) delete next;
        if (row != NULL) delete row;
    }
};
#endif
