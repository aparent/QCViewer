#ifndef GATE
#define GATE

#include <string>
#include <vector>
#include "state.h" // float_t

using namespace std;

enum gateType {NOT, H, FRED, U};

class Control{
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

    complex<float_t> *matrix;
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

class FredGate : public Gate {//hadamard gate
  public:
    int QCost(int numLines);
		FredGate();
};

class UGate : public Gate {//hadamard gate
  public:
    int QCost(int numLines);
    void setQCost(int QCost);
		UGate(int QCost = 0);
	private:
		int cost; //stores quantum cosst
};

void minmaxWire (vector<Control>* ctrl, vector<int>* targ, int *dstmin, int *dstmax);
#endif
