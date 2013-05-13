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

Authors: Alex Parent, Marc Burns
---------------------------------------------------------------------*/

#ifndef SUBCIRCUIT_H
#define SUBCIRCUIT_H

#include "gate.h"
#include "circuit.h"
#include <string>
#include <map>

/*!
  \brief Defines a line in a circuit
*/
struct Line
{
    Line (std::string);
    std::string getInputLabel () const;
    std::string getOutputLabel() const;
    std::string lineName;
    std::string outLabel;
    bool constant;
    bool garbage;
    int initValue;
};

//A subcircuit gate contains
class Subcircuit : public Gate
{
public:
    Subcircuit();
    ~Subcircuit();

    /*!
    	\brief Append a new line to circuit.
    	\param line Name of the line
    */
    void addLine(std::string line);

    /*!
    	\brief Returns a const reference to the requested Line
    	\param pos Index of line to return

      Use this function if you do not wish to modify the line so that const
      correctness can be maintained
    */
    const Line& getLine(int pos) const;

    /*!
    	\brief Returns a referance to the requested Line
    	\param pos Index of line to return

      Use this if you wish to get a line and modify it
    */
    Line& getLineModify(int pos);

    //! Returns the circuit name
    std::string getName();

    //! Sets the circuit name
    void setName(std::string);

    //! Returns the number of lines in the circuit
    unsigned int numLines() const;

    /*! @name Gates
    	Functions that have to do with manipulating gates.
    */
    ///@{

    //! Appends gate newGate to the end of the circuit
    void addGate(std::shared_ptr<Gate> newGate);

    //! Inserts gate newGate before the gate at position pos
    void addGate(std::shared_ptr<Gate> newGate, unsigned int pos);

    /*!
    	\brief Replaces gate at position pos by gate newGate
    	\param newGate gate to set
    	\param pos postion of gate to be replaced

    	Use this to replace a gate in the the circuit.
    */
    void setGate(std::shared_ptr<Gate> newGate, unsigned int pos);

    //! Removes gate at position pos
    void removeGate (unsigned int pos);

    //! Swaps gates at positions a and b
    void swapGate (unsigned int a, unsigned int b);

    //! Returns gate at position pos
    std::shared_ptr<Gate> getGate(int pos) const;

    //! Returns the number of gates counting subcircuits as 1 gate
    unsigned int numGates() const;

    ///@}

    /*! @name Statistics
    	These functions are used to retrive circuit statistics
    */
    ///@{

    //! Returns the total number of gates in the circuit counting gates in subcircuits recursively
    unsigned int totalGates() const;

    //! Returns the total number of gates with some name
    unsigned int gateCount(std::string gateName);

    //! Returns the Circuit Depth
    unsigned int depth();

    ///@}

    //! Returns a std::vector of ints specifying the last gate in each parallel block.
    std::vector<int> getParallel() const;

    /*!
      \brief Returns a std::vector of ints specifying the last gate in each
             parallel block (greedy algorithm).
    */
    std::vector<unsigned int> getGreedyParallel ();

private:
    std::string name;
    std::vector <std::shared_ptr<Gate>>           gates;
    std::vector <Line>            lines;
    std::vector <unsigned int> breakpoints;
    std::vector <unsigned int>    columns;
    std::vector<unsigned int> lineMap;
};


#endif
