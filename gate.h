#ifndef GATE
#define GATE

#include <string>
#include <vector>

using namespace std;

class Control {
  public:
    int wire;
    bool polarity;
};

class Gate {
  public:
    string name;

    vector <Control> controls;

    virtual int QCost() = 0;
};

class CNOTGate : public Gate {
  public:
    vector <int> targets
    int QCost () { return 0; }
};

#endif
