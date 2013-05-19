/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011 Alex Parent and The University of Waterloo,
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


#include "circuit.h"
#include "subcircuit.h"
#include "utility.h"
#include <map>
#include <fstream>
#include <iostream>

using namespace std;

Circuit::Circuit()
{
  root = shared_ptr<Subcircuit>(new Subcircuit());
  root_attr = GateAttributes();
}

Circuit::~Circuit ()
{ }

string Circuit::getName() const
{
  return name;
}

void Circuit::setName(string n_name)
{
    name = n_name;
}

std::shared_ptr<Subcircuit> Circuit::getRootSubcircuit()
{
  return root;
}

const GateAttributes & Circuit::getRootSubcircuitAttributes()
{
  return root_attr;
}

void Circuit::clear()
{
  root = shared_ptr<Subcircuit>(new Subcircuit());
  root_attr = GateAttributes();
  subcircuits.clear();
}

void Circuit::readQuiglFile(std::string filename)
{

}

void Circuit::readQCFile(std::string filename)
{

}

void Circuit::readQuiglBuffer(std::string data)
{

}

void Circuit::readQCBuffer(std::string data)
{

}

void Circuit::writeQuiglFile(std::string filename) const
{

}

void Circuit::writeQCFile(std::string filename) const
{

}

void Circuit::writeQuiglBuffer(std::string & data) const
{

}

void Circuit::writeQCBuffer(std::string & data) const
{

}

