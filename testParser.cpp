#include "circuitParser.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(){
  Circuit c = parseCircuit("testCircuits/mod5d4.tfc");
  for(int i = 0; i < c.lNames.size(); i++){
    cout << c.lNames.at(i) << endl;
  }
  return 0;
}
