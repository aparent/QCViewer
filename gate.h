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

class NOTGate : public Gate {
  public:
    virtual int QCost () { return 0; }
};

#endif
