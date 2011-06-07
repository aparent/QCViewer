#ifndef GATE
#define GATE

#include <string>
#include <vector>
#include "state.h" // float_t

using namespace std;

enum gateType {R, U};

//used to specify a control number and polarity
class Control{
  public:
    Control (int,bool);
    int wire;
    bool polarity;
};

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
		virtual State applyGate(State);

		int gateType; //used with enum gateType
		vector <Control> controls;
		vector <unsigned int> targets;
};

//A gate matrix struct for UGate
struct gateMatrix{
	unsigned int dim;
	complex<float_t> * data;
};

//A general unitary gate
class UGate : public Gate {
  public:
		UGate();

		string getName();
		State applyGate(State);

		void setName(string);
	private:
    gateMatrix *matrix;
		string name;
};

//An arbitrary rotation gate
class RGate : public Gate {
  public:
		string getName();
		State applyGate(State);

		void setRotation(float_t);
	private:
    float rot;
		string name;
};

void minmaxWire (vector<Control>* ctrl, vector<unsigned int>* targ, int *dstmin, int *dstmax);
#endif
