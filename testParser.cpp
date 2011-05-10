#include "circuitParser.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
void draw (Circuit*);
int main(){
  Circuit c = parseCircuit("testCircuits/foo.tfc");
  cout << "QCost: " << c.QCost() << endl;
  cout << "Line Names: " ;
  for(int i = 0; i < c.numLines(); i++){
    cout << c.getLine(i)->lineName << " ";
  }
  cout << endl << "Inputs: ";
  for(int i = 0; i < c.numLines(); i++){
    cout << c.getLine(i)->getInputLabel() <<  " ";
  }
  cout << endl << "Outputs: ";
  for(int i = 0; i < c.numLines(); i++){
    cout << c.getLine(i)->getOutputLabel() << " ";
  }
  cout << endl;
  for(int i = 0; i < c.numGates(); i++){
    cout << "Gate Name: "<< c.getGate(i)->name << " ";
    cout << " Controls: ";
    for(int j = 0; j < c.getGate(i)->controls.size(); j++){
      cout << " " <<c.getGate(i)->controls.at(j).wire;
    }
    cout << " Targets: ";
    for(int j = 0; j < c.getGate(i)->targets.size(); j++){
      cout << " " << c.getGate(i)->targets.at(j);
    }
    cout << endl;
  }
  return 0;
}
