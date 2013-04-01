#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>
#include <exception>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <iostream>
#include "circuit.h"
#include "circuitParser.h"
#include "QCParserUtils.h"

using namespace std;
using namespace boost;

struct quigl_statement {
    string oper; /* operator name */
    vector<int> arguments; /* operator arguments, as indices into the labels array. */
    vector<int> controls; /* operator controls, as indices into the labels array. */
};

struct quigl_procedure {
    string name; /* procedure name */
    vector<string> labels; /* procedure labels (lines) */
    vector<quigl_statement> stmts; /* procedure statements */

    void load(string filename);
    void save(string filename);
};

std::ostream & operator<<(std::ostream & ou, const quigl_statement & s)
{
    ou << " " << s.oper << " (";
    for(std::vector<int>::const_iterator p = s.arguments.begin(); p != s.arguments.end(); ++p) {
        ou << *p;
        if(p+1 != s.arguments.end()) {
            ou << " ";
        }
    }
    ou << ") (";
    for(std::vector<int>::const_iterator p = s.controls.begin(); p != s.controls.end(); ++p) {
        ou << *p;
        if(p+1 != s.controls.end()) {
            ou << " ";
        }
    }
    ou << ")\n";
    return ou;
}
std::ostream & operator<<(std::ostream & ou, const quigl_procedure & c)
{
    ou << "\"" << c.name << "\" (";
    for(std::vector<std::string>::const_iterator p = c.labels.begin(); p != c.labels.end(); ++p) {
        ou << *p;
        if(p+1 != c.labels.end()) {
            ou << ",";
        }
    }
    ou << "):\n";
    for(auto & p : c.stmts) {
        ou << p;
    }
    return ou;
}

void quigl_procedure::load(string filename)
{
    using property_tree::ptree;
    ptree pt;

    read_xml(filename, pt);

    name = pt.get<string>("program.procedure.<xmlattr>.name");

    {
        string label_list = pt.get<string>("program.procedure.<xmlattr>.quantum");
        tokenizer<> label_tok(label_list);
        for(auto & p : label_tok) {
            labels.push_back(p);
        }
    }

    map<string, int> label_lookup;
    {
        int i = 0;
        for(auto & p : labels) {
            label_lookup[p] = i++;
        }
    }

    for(ptree::value_type &ctrl : pt.get_child("program.procedure.body")) {
        quigl_statement stmt;
        for(ptree::value_type &t : ctrl.second) {
            if(t.first == "control") {
                string var = t.second.get_child("variable").data();
                auto it = label_lookup.find(var);
                if(it == label_lookup.end()) {
                    throw std::runtime_error(string("Control label not in line labels list: '") + var + "'");
                }
                stmt.controls.push_back(it->second);
            } else if(t.first == "body") {
                for(ptree::value_type &op : t.second.get_child("statement")) {
                    if(op.first == "operator") {
                        stmt.oper = op.second.data();
                    } else if(op.first == "arguments") {
                        for(ptree::value_type &arg : op.second) {
                            if(arg.first == "variable") {
                                string var = arg.second.data();
                                auto it = label_lookup.find(var);
                                if(it == label_lookup.end()) {
                                    throw std::runtime_error(string("Operator input label not in line labels list: '") + var + "'");
                                }
                                stmt.arguments.push_back(it->second);
                            } else {
                                throw std::runtime_error(string("Operator argument list contains unexpected tag: '") + arg.first + "'.");
                            }
                        }
                    } else {
                        throw std::runtime_error(string("Control body contains unexpected tag: '") + op.first + "'.");
                    }
                }
            } else {
                throw std::runtime_error(string("Procedure body contains unexpected tag: '") + t.first + "'.");
            }
        }
        stmts.push_back(stmt);
    }
}

void quigl_procedure::save(string filename)
{
    //
}

std::shared_ptr<Circuit> circuit_from_quigl(std::string filename)
{
    quigl_procedure p;
    p.load(filename);

    std::shared_ptr<Circuit> c(new Circuit());

    c->setName(p.name);
    for(auto &ln : p.labels) {
        c->addLine(ln);
    }

    for(int i=0; i<c->numLines(); i++) {
        Line & l = c->getLineModify(i);
        l.constant = l.garbage = false;
    }

    for(auto &stmt : p.stmts) {
        name_node * init_args = NULL, * init_ctrls = NULL;
        name_node **args = &init_args, **ctrls = &init_ctrls;

        for(auto &arg : stmt.arguments) {
            *args = new name_node(p.labels.at(arg), NULL);
            args = &((*args)->next);
        }

        for(auto &ctrl : stmt.controls) {
            *ctrls = new name_node(p.labels.at(ctrl), NULL);
            ctrls = &((*ctrls)->next);
        }

        vector<string> errors;
        add_gate(c, stmt.oper, init_ctrls, init_args, 1, errors);

        if(!errors.empty()) {
            for(auto &e : errors) {
                cerr << e << "\n";
            }
            throw std::runtime_error("Errors in add_gate.");
        }
    }

    return c;
}

int main()
{
    std::shared_ptr<Circuit> c = circuit_from_quigl("test.xml");
    saveCircuit(c, "test.qc");
}

