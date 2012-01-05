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
void add_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp, map<string,Circuit> &subcircuits);
void add_R_gate (Circuit * circ, string gateName, name_node *names, unsigned int exp, double rot);
#endif
