#include "circuitParser.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(){
  Circuit c = parseCircuit("testCircuits/veryLargeTest.tfc");
  /*cout << "Line Names: ";
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
  for(int i = 0; i < c.gates.size(); i++){
    cout << "Gate Name: "<< c.gates.at(i).name << " " <<endl;
    cout << "Gate varibles: ";
    for(int j = 0; j < c.gates.at(i).inputs.size(); j++){
      cout << c.gates.at(i).inputs.at(j) << " ";
    }
    cout << endl;
  }*/
  return 0;
}
