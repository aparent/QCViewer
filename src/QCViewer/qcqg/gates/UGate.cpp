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


//Implimentation code for the UGate
#include "gate.h"
#include "UGateLookup.h"
#include "utility.h"
#include <complex>
#include <iostream>


using namespace std;

UGate::UGate(string n_name) : Gate(), name(n_name)
{
    drawType = DEFAULT;
    loop_count = 1;
    dname_checked = false;
}

UGate::UGate(string n_name, string n_dname) : Gate(), name(n_name)
{
    drawType = DEFAULT;
    loop_count = 1;
    dname = n_dname;
    dname_checked = true;
}

shared_ptr<Gate> UGate::clone() const
{
    UGate* g = new UGate(name);
    g->controls = controls;
    g->targets = targets;
    g->drawType = drawType;
    g->breakpoint = breakpoint;
    return shared_ptr<Gate>(g);
}

string UGate::getName() const
{
    if (drawType == NOT) {
        return "tof";
    } else {
        return name;
    }
}

string UGate::getDrawName()
{
    gateMatrix matrix = UGateLookup(name);
    if (matrix.dim == 0) {
        cerr << "Game matrix not found for " << name  << "!" << endl;
        return "";
    } else {
        return matrix.drawName;
    }
}

string UGate::getLatexName()
{
    gateMatrix matrix = UGateLookup(name);
    if (matrix.dim == 0) {
        cerr << "Game matrix not found for " << name  << "!" << endl;
        return "";
    } else {
        return matrix.latexName;
    }
}

void UGate::setName(string n_name)
{
    name = n_name;
}

