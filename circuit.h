#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include <vector>
#include <string>

using namespace std;

class Line {
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

class Circuit {
  public:

    int QCost();

    void   addLine(string line);
    Line*  getLine(int pos);
    int    numLines();

    void addGate(Gate *newGate); //appends to end
    void addGate(Gate *newGate,int pos); //inserts at pos
    Gate* getGate(int pos);
    int numGates(); //Returns the number of gates

		vector<int> getParallel();  //Returns a vector of ints specifying the last gate in each parallel block.

  private:
    vector <Gate*>           gates;
    vector <Line>            lines;
};
#endif
