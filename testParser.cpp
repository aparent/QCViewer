#include "circuitParser.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(){
  Circuit c = parseCircuit("testCircuits/mod5d4.tfc");
  cout << "QCost: " << c.QCost() << endl;
  cout << "Line Names: " ;
  for(int i = 0; i < c.lNames.size(); i++){
    cout << c.lNames.at(i) << " ";
  } 
  cout << endl << "Inputs: ";
  for(int i = 0; i < c.inputs.size(); i++){
    cout << c.inputs.at(i) <<  " ";
  }
  cout << endl << "Outputs: ";
  for(int i = 0; i < c.outputs.size(); i++){
    cout << c.outputs.at(i) << " ";
  }
  cout << endl << "Constants: ";
  for(int i = 0; i < c.constants.size(); i++){
    cout << c.constants.at(i) << " ";
  }
  cout << endl << "Output Labels: ";
  for(int i = 0; i < c.outputLabels.size(); i++){
    cout << c.outputLabels.at(i) << " ";
  }
  cout << endl;
  for(int i = 0; i < c.numGates(); i++){
    cout << "Gate Name: "<< c.getGate(i).name << " ";
    cout << " Varibles: ";
    for(int j = 0; j < c.getGate(i).inputs.size(); j++){
      cout << c.getGate(i).inputs.at(j) << " ";
    }
    cout << endl;
  }
  return 0;
}
