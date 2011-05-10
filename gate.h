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

    virtual int QCost(int numLines) = 0; 
    int numCont();//total number of negative and positive controls
    int numNegCont();
};

class CNOTGate : public Gate {
  public:
    int QCost(int numLines);
};

#endif
