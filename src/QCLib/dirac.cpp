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


#include "dirac.h"
#include "state.h"
#include "utility.h"
#include "diracParser.h"
#include "parseNode.h"
#include <iostream>//for error messages
#include <stdlib.h> //atof

using namespace std;

struct diracTerm {
    int type;
    State vecValue;
    complex<float_type> numValue;
};

parseNode *parseDirac(std::string input); //Defined in diracParser.y
string printTree(parseNode *node);
diracTerm evalTree(parseNode *node);

State *getStateVec (std::string input, bool normalize)
{
    parseNode *node = parseDirac(input);
    if(node!=NULL) {
        diracTerm term = evalTree(node);
        if (term.type != KET ) {
            return NULL;
        }
        if (normalize) {
            term.vecValue.normalize();
        }
        State *ret = new State(term.vecValue);
        return ret;
    } else {
        cerr << "PARSE ERROR" << endl;
        return NULL;
    }
}

string printTree(parseNode *node)
{
    if (node->type==SQRT) 																							return node->value + "(" + printTree(node->right)+")";
    if (node->left == NULL && node->right == NULL && node->type==KET ) 	return "|" + node->value+">";
    if (node->left == NULL && node->right == NULL ) return node->value;
    if (node->left == NULL && node->right == NULL && node->type==CNUM ) return node->value+"i";
    return "("+printTree(node->left)+ " " +node->value + " " + printTree(node->right)+")";
}

diracTerm stringToKet(string value)
{
    diracTerm ret;
    index_t basis = 0;
    ret.type = KET;
    for (unsigned int i = 0; i < value.size(); i++) {
        if      (value[i] == '1') {
            basis = SetRegister(basis,i);
        } else if (value[i] == '0') {
        } else    {
            cerr << "ERROR BAD CHAR IN KET: " << value[i] << endl;
        }
    }
    ret.vecValue = State(1,basis);
    ret.vecValue.dim = ipow(2,value.size());
    return ret;
}

diracTerm constAdd(diracTerm a, diracTerm b)
{
    a.numValue += b.numValue;
    return a;
}

diracTerm constSub(diracTerm a, diracTerm b)
{
    a.numValue -= b.numValue;
    return a;
}

diracTerm constMult(diracTerm a, diracTerm b)
{
    a.numValue *= b.numValue;
    return a;
}

diracTerm ketAdd(diracTerm a, diracTerm b)
{
    a.vecValue += b.vecValue;
    return a;
}

diracTerm ketSub(diracTerm a, diracTerm b)
{
    a.vecValue -= b.vecValue;
    return a;
}

diracTerm ketKron(diracTerm a, diracTerm b)
{
    diracTerm ret;
    ret.vecValue = kron(a.vecValue,b.vecValue);
    ret.type = KET;
    return ret;
}

diracTerm constKetMult(diracTerm a, diracTerm b)
{
    b.vecValue*=a.numValue;
    return b;
}

diracTerm evalTree(parseNode *node)
{
    diracTerm ret;
    if(node->type == CNUM) {
        ret.type = NUM;
        ret.numValue = complex<float_type>(0,atof((node->value).c_str()));
        return ret;
    }
    if(node->type == NUM) {
        ret.type = NUM;
        ret.numValue = complex<float_type>(atof((node->value).c_str()),0);
        return ret;
    }
    if(node->type == KET) {
        return stringToKet(node->value);
    }

    diracTerm left;
    diracTerm right;

    if(node->right == NULL) {
        cerr << "ERROR right is NULL."<<endl;
        return ret;
    } else right = evalTree(node->right);

    if (node->type == SQRT) {
        if (right.type == NUM) {
            ret.numValue=sqrt(right.numValue);
            ret.type = NUM;
            return ret;
        } else {
            cerr << "ERROR cannot subtract vector and a number"<<endl;
            return ret;
        }
    }


    if(node->right == NULL) {
        cerr << "ERROR left is NULL."<<endl;
        return ret;
    } else left = evalTree(node->left);
    if (node->type == EXPON) {
        if (left.type == KET) {
            ret.vecValue = State(left.vecValue);
            for(unsigned int i = 1; i < (unsigned int)real(right.numValue); i++) {
                ret.vecValue = kron(ret.vecValue,left.vecValue);
            }
            ret.type = KET;
            return ret;
        } else {
            cerr << "ERROR exponent must be used on vector and a number"<<endl;
            return ret;
        }
    }

    if (node->type == PLUS) {
        if      (left.type == NUM && right.type == NUM) return constAdd(left,right);
        else if (left.type == KET && right.type == KET) return ketAdd(left,right);
        else {
            cerr << "ERROR cannot add vector and a number"<<endl;
            return ret;
        }
    }
    if (node->type == MINUS) {
        if      (left.type == NUM && right.type == NUM) return constSub(left,right);
        else if (left.type == KET && right.type == KET) return ketSub(left,right);
        else {
            cerr << "ERROR cannot subtract vector and a number"<<endl;
            return ret;
        }
    }
    if (node->type == TIMES) {
        if      (left.type == NUM && right.type == NUM) return constMult(left,right);
        else if (left.type == KET && right.type == KET) return ketKron(left,right);
        else {
            if    (left.type == KET && right.type == NUM) return constKetMult(right,left);
            else                                          return constKetMult(left,right);
        }
    }
    if (node->type == DIV) {
        if (left.type == NUM && right.type == NUM) {
            ret.type = NUM;
            ret.numValue = left.numValue / right.numValue;
            return ret;
        } else {
            cerr << "ERROR cannot divide a vector"<< endl;
            return ret;
        }
    } else {
        cerr << "ERROR unrecognized type: "<< node->type << endl;
        return ret;
    }
}
