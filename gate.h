#ifndef GATE
#define GATE

#include <string>
#include <vector>
#include "state.h" // float_t

using namespace std;

enum gateType {R, U};

class Control{
  public:
    Control (int,bool);
    int wire;
    bool polarity;
};

/*class Gate {
  public:
    string name;
		int gateType;

    vector <Control> controls;
    vector <int> targets;

    virtual int QCost(int numLines) = 0;
    int numCont();//total number of negative and positive controls
    int numNegCont();

    complex<float_t> *matrix;
};*/

/*
GATE
---------------
This is the general gate class from which all gates are derived.
Name is kept private since in the case of gates like the arbitray
rotation gate we may want the name to be dependent on the rot amount
*/
class Gate {
	public:
		virtual string getName(); 
		virtual void setName(string);
		virtual State applyGate(State);

		int gateType; //used with enum gateType
		vector <Control> controls;
		vector <unsigned int> targets;
};

//A general unitary gate
class UGate : public Gate {
  public:
		string getName(); 
		void setName(string);
		State applyGate(State);

		void setMatrix(complex<float_t>*)
	private:
    complex<float_t> *matrix;
		string name;
};

//An arbitrary rotation gate
class RGate : public Gate {
  public:
		string getName(); 
		void setName(string);
		State applyGate(State);

		void setRotation(float_t)
	private:
    float *matrix;
		string name;
};

void minmaxWire (vector<Control>* ctrl, vector<int>* targ, int *dstmin, int *dstmax);
#endif
