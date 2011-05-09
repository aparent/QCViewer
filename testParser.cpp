#include "circuitParser.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(){
  Circuit c = parseCircuit("testCircuits/mod5d4.tfc");
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
    cout << "Gate Name: "<< c.getGate(i).name << " ";
  }
  return 0;
}
