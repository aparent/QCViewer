#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include <vector>
#include <string>

using namespace std;

class Circuit {
  public:
  vector <vector<Gate> >  gateColumn;
  vector <string>         lNames;
  vector <string>         outputLabels;
  vector <int>            inputs;
  vector <int>            outputs;
  vector <int>            constants;
};
#endif
