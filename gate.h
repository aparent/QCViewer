#ifndef GATE
#define GATE

#include <string>
#include <vector>

using namespace std;

class Control {
  public:
    Control (int,bool);
    int wire;
    bool polarity;
};

class Gate {
  public:
    string name;

    vector <Control> controls;
    vector <int> targets;

    int QCost() { return 0; }
};

class CNOTGate : public Gate {
  public:
    int QCost () { return 0; }
};

#endif
