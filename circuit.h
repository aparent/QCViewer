#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include <vector>
#include <string>

using namespace std;

class Circuit {
  public:
    vector <string>         lNames;
    vector <string>         outputLabels;
    vector <int>            inputs;
    vector <int>            outputs;
    vector <int>            constants;

    int QCost();
    void addGate(Gate newGate); //appends to end
    void addGate(Gate newGate,int pos); //inserts at pos
    Gate getGate(int pos);
    int numGates(); //Returns the number of gates

  private:
    vector <Gate>           gates;
};
#endif
