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

#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include "types.h"
#include <vector>
#include <string>
#include <map>
using namespace std;

class Line
{
public:
    Line (string);
    string getInputLabel ();
    string getOutputLabel();
    string lineName;
    string outLabel;
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

class Loop
{
public:
    uint32_t first;
    uint32_t last;
    uint32_t n;
    uint32_t sim_n;
    std::string label;
};

class Circuit
{
public:
    Circuit ();
    ~Circuit ();

    int QCost();

    void   addLine(string line);
    Line*  getLine(int pos);
		string getName();
		void setName(string);
    unsigned int numLines();

    void addGate(Gate *newGate); //appends to end
    void addGate(Gate *newGate, unsigned int pos); //inserts at pos
    void removeGate (unsigned int);
    Gate* getGate(int pos) const;
    unsigned int numGates() const; //Returns the number of gates
    void swapGate (unsigned int, unsigned int);

    void add_loop (Loop);
    void newArch ();
    void removeArch ();
    void parseArch (string);
    void arch_set_LNN();

    vector<int> getParallel();  //Returns a vector of ints specifying the last gate in each parallel block.
    vector<int> getGreedyParallel (); // used for drawing gates in the same column
    vector<int> getArchWarnings ();
    vector<int> column_breaks;
		map<string,Circuit*> subcircuits;

    QArch *arch;
    vector <Loop>            loops;
private:
    string name;
    vector <Gate*>           gates;
    vector <Line>            lines;
};


#endif
