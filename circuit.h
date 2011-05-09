#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include <vector>
#include <string>

using namespace std;

class Input {
public:
  string inLabel;
  string outLabel;
  bool constant;
  bool garbage;
  int value;
};

class Circuit {
  public:
    vector <Input> inputs;

    int QCost();
    void addGate(Gate newGate); //appends to end
    void addGate(Gate newGate,int pos); //inserts at pos
    Gate getGate(int pos);
    int numGates(); //Returns the number of gates

  private:
    vector <Gate>           gates;
};
#endif
