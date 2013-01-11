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

Authors: Alex Parent, Jacob Parker
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

class Line
{
public:
    Line (std::string);
    std::string getInputLabel () const;
    std::string getOutputLabel() const;
    std::string lineName;
    std::string outLabel;
    bool constant;
    bool garbage;
    int initValue;
};

class QArch   // Less space efficient than it could trivially be.
{
public:
    ~QArch() {
        delete graph;
    }
    QArch (int nn) : n(nn) {
        graph = new char[n*n];
        for (int i = 0; i < n*n; i++) graph[i] = 0;
    }

    bool query (int i, int j) {
        return (0 != graph[i+n*j]);
    }
    void set (int i, int j) {
        graph[i+n*j] = graph[j+n*i] = 1;
    }
private:
    int n;
    char *graph;
};

class SimState
{
public:
    SimState () : gate(0),nextGate(true),simulating(false),loop(1) {}
    unsigned int gate;
    bool nextGate;
    bool simulating;
    unsigned int loop;
};

/*!
	\brief Defines a circuit
*/
class Circuit
{
public:
    Circuit ();
    ~Circuit ();

    int QCost();


    /*!
    	\brief Append a new line to circuit.
    	\param line Name of the line
    */
    void addLine(std::string line);

    /*!
    	\brief Returns a const Line for reading
    	\param pos Line number to get

    	Use this function if you do not wish to
    	modify the line so that const correctness
    	can be maintained
    */
    const Line& getLine(int pos) const;
    /*!
    	\brief Returns a referance to Line for modification
    	\param pos Line number to get

    	Use this if you wish to get a line and want to modify
    	it
    */
    Line& getLineModify(int pos);
    //! Returns the circuit name
    std::string getName();
    //! Sets the circuit name
    void setName(std::string);
    //! Expands all subcircuits
    void expandAll();
    unsigned int numLines() const;

    /*! @name Gates
    	Functions that have to do with manipulating gates.
    */
    ///@{
    //! Appends gate to the end of the circuit
    void addGate(std::shared_ptr<Gate> newGate);
    //! Inserts gate at pos
    void addGate(std::shared_ptr<Gate> newGate, unsigned int pos);
    /*!
    	\brief Sets gate at pos
    	\param newGate gate to set
    	\param pos postion of gate to be replaced

    	Use this to replace a gate in the the circuit.
    	Note: You may want to delete the old gate if it is no longer in use
    */
    void setGate(std::shared_ptr<Gate> newGate, unsigned int pos);
    //! removes gate at pos
    void removeGate (unsigned int pos);
    //! Swaps gates at a and b
    void swapGate (unsigned int a, unsigned int b);
    //! returns gate at pos
    std::shared_ptr<Gate> getGate(int pos) const;

    //! Returns the number of gates counting subcircuits as 1 gate
    unsigned int numGates() const;
    ///@}

    /*! @name Statistics
    	These functions are used to retrive circuit statistics
    */
    ///@{
    //! Returns the total number of gates in the circuit counting all gates in subcircuits
    unsigned int totalGates() const;
    //! Returns the total number of gates with some name
    unsigned int gateCount(std::string gateName);
    //! Returns the Circuit Depth
    unsigned int depth();
    ///@}

    /*! @name Simulation
    		These functions are used to run circuit simulation
    	*/
    ///@{
    //! Runs the full circuit on the input state.
    bool run (State &s);
    //! Steps the circuit through the next gate.
    bool step (State &s);
    //! Resets current gate to the beginning of the circuit.
    void reset ();
    ///@}

    /* architecture */
    void newArch ();
    void parseArch (std::string);
    void arch_set_LNN();

    //! Returns a std::vector of ints specifying the last gate in each parallel block.
    std::vector<int> getParallel() const;
    std::vector<unsigned int> getGreedyParallel (); // used for drawing gates in the same column
    std::vector<int> getArchWarnings () const;
    std::vector <unsigned int>    columns;


    //! Mapping of names to circuits storing subcircuits that might appear in this circuit
    std::map<std::string,std::shared_ptr<Circuit>> subcircuits;

    SimState simState; //XXX: made public until a better solution is had (separating simulation from circuit)

private:
    QArch *arch;
    std::string name;
    std::vector <std::shared_ptr<Gate>>           gates;
    std::vector <Line>            lines;
    std::vector <unsigned int> breakpoints;


    bool allExpanded;

    //for deconstructor
    void removeArch ();
};


#endif
