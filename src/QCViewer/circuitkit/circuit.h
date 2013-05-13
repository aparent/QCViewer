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

/*! \file circuit.h
    \brief Defines the circuit class and some other related classes
*/

#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include "types.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

/*!
	\brief Defines a circuit
*/
class Circuit
{
public:
    Circuit ();
    ~Circuit ();

    //! Returns the circuit name
    std::string getName();

    //! Sets the circuit name
    void setName(std::string);

    //! Get root subcircuit
    std::shared_ptr<Subcircuit> getRootSubcircuit();

    //! Clear the circuit
    void clear();

    //! Clear and read circuit from a Quigl file
    void readQuiglFile(std::string filename);

    //! Clear and read circuit from a .qc file
    void readQCFile(std::string filename);

    //! Clear and read circuit from a buffer containing Quigl-formatted data
    void readQuiglBuffer(std::string data);

    //! Clear and read circuit from a buffer containing .qc-formatted data
    void readQCBuffer(std::string data);

    //! Write circuit to a file as Quigl
    void writeQuiglFile(std::string filename);

    //! Write circuit to a file as .qc
    void writeQCFile(std::string filename);

    //! Write circuit to a buffer as Quigl-formatted data
    void writeQuiglBuffer(std::string & data);

    //! Write circuit to a buffer as .qc-formatted data
    void writeQCBuffer(std::string & data);

private:
    std::string name;

    //! Root subcircuit
    std::shared_ptr<Subcircuit> root;

    //! Mapping of names to circuits storing subcircuits that might appear in this circuit
    std::map<std::string,std::shared_ptr<Subcircuit>> subcircuits;
};


#endif
