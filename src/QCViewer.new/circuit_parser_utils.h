#ifndef CIRCUIT_PARSER_UTILS_H
#define CIRCUIT_PARSER_UTILS_H
#include <complex>
#include <string>
#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265
#endif

struct var_list_t {
    var_list_t(std::string v, var_list_t *n)
        : var(v),next(n) {};
    std::string var;
    var_list_t *next;
    void print() {
        std::cout << " " << var;
        if (next!=NULL) next->print();
    }
};

struct map_list_t {
    map_list_t(std::string f, std::string t, map_list_t *n)
        : from(f),to(t),next(n) {};
    std::string from;
    std::string to;
    map_list_t *next;
    void print() {
        std::cout <<" "<< from << "->" << to;
        if (next!=NULL) next->print();
    }
};

struct param_list_t {
    param_list_t(std::complex<float> v, param_list_t *n)
        : val(v),next(n) {};
    std::complex<float> val;
    param_list_t *next;
    void print() {
        std::cout << val;
        if (next!=NULL) {
            std::cout << ",";
            next->print();
        }
    }
};

struct gate_list_t {
    gate_list_t(std::string na,map_list_t *in, gate_list_t *n)
        : name(na),inputs_m(in),inputs_v(NULL),params(NULL),next(n) {};
    gate_list_t(std::string na,map_list_t *in, param_list_t *p, gate_list_t *n)
        : name(na),inputs_m(in),inputs_v(NULL),params(p),next(n) {};
    gate_list_t(std::string na,var_list_t *in, gate_list_t *n)
        : name(na),inputs_m(NULL),inputs_v(in),params(NULL),next(n) {};
    gate_list_t(std::string na,var_list_t *in, param_list_t *p, gate_list_t *n)
        : name(na),inputs_m(NULL),inputs_v(in),params(p),next(n) {};
    std::string name;
    map_list_t *inputs_m;
    var_list_t *inputs_v;
    param_list_t *params;
    gate_list_t *next;
    void print() {
        std::cout << "Gate: " << name << std::endl;
        if (params!=NULL) {
            std::cout << "[";
            params->print();
            std::cout << "]";
        }
        if (inputs_m!=NULL) inputs_m->print();
        if (inputs_v!=NULL) inputs_v->print();
        std::cout << std::endl;
        if (next!=NULL) next->print();
    }
};

struct col_list_t {
    col_list_t(gate_list_t *g,col_list_t *n)
        : gates(g),next(n) {};
    gate_list_t *gates;
    col_list_t *next;
    void print() {
        std::cout << "----Column----" << std::endl;
        if (gates!=NULL) gates->print();
        if (next!=NULL) next->print();
    }
};

struct option_list_t {
    option_list_t(std::string o,var_list_t *v,option_list_t *n)
        : option(o),vals(v),next(n) {};
    std::string option;
    var_list_t *vals;
    option_list_t *next;
    void print() {
        std::cout << "Option: " << option << ": ";
        if (vals!=NULL) vals->print();
        std::cout << std::endl;
        if (next!=NULL) next->print();
    }
};

struct circuit_list_t {
    circuit_list_t(std::string n,option_list_t *o,col_list_t *c)
        : options(o),cols(c),name(n) {};
    option_list_t *options;
    col_list_t *cols;
    std::string name;
    circuit_list_t *next;
    void print() {
        std::cout << "---------------------------" << std::endl;
        std::cout << "Circuit: " << name << std::endl;
        std::cout << "---------------------------" << std::endl;
        if (options!=NULL) options->print();
        if (cols!=NULL) cols->print();
        std::cout << std::endl;
        if (next!=NULL) next->print();
    }
};
#endif
