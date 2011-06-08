#ifndef GATE
#define GATE

#include <string>
#include <vector>
#include "state.h" // float_t

using namespace std;

enum gateType {RGATE, UGATE};
enum dType {NOT, FRED, DEFAULT};

//used to specify a control number and polarity
class Control{
  public:
    Control (int,bool);
    unsigned int wire;
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
		virtual ~Gate();

		virtual string getName()=0;
		virtual State *applyToBasis(index_t)=0;

		gateType type; //used with enum gateType
		dType drawType;
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
		UGate(string);

		string getName();
		State *applyToBasis(index_t);

		void setName(string);
	private:
		unsigned int ExtractInput (index_t);
		index_t BuildBitString (index_t, unsigned int);
		State* ApplyU(index_t);
    gateMatrix *matrix;
		string name;
};

//An arbitrary rotation gate
class RGate : public Gate {
  public:
		RGate(float_t);

		string getName();
		State *applyToBasis(index_t);

	private:
		index_t BuildBitString (index_t, unsigned int);
		State* ApplyU(index_t);
    float_t rot;
		string name;
};

void minmaxWire (vector<Control>* ctrl, vector<unsigned int>* targ, unsigned int *dstmin, unsigned int *dstmax);
#endif
