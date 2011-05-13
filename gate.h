#ifndef GATE
#define GATE

#include <string>
#include <vector>

using namespace std;

enum gateType {NOT, H};

class Control {
  public:
    Control (int,bool);
    int wire;
    bool polarity;
};

class Gate {
  public:
    string name;
		int gateType;

    vector <Control> controls;
    vector <int> targets;

    virtual int QCost(int numLines) = 0;
    int numCont();//total number of negative and positive controls
    int numNegCont();
};

class NOTGate : public Gate {
  public:
    int QCost(int numLines);
    NOTGate();
};

class HGate : public Gate {//hadamard gate
  public:
    int QCost(int numLines);
		HGate();
};

#endif
