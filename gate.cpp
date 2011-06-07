#include "gate.h"
#include "utility.h"

Control::Control(int setWire, bool setPol) : wire(setWire), polarity (setPol) {}

Gate::~Gate(){}

void minmaxWire (vector<Control>* ctrl, vector<unsigned int>* targ, int *dstmin, int *dstmax) {
  unsigned int minw = (*targ)[0];
  unsigned int maxw = (*targ)[0];
  for (unsigned int i = 1; i < targ->size (); i++) {
    minw = min (minw, (*targ)[i]);
    maxw = max (maxw, (*targ)[i]);
  }

  for (unsigned int i = 0; i < ctrl->size (); i++) {
    minw = min (minw, (*ctrl)[i].wire);
    maxw = max (maxw, (*ctrl)[i].wire);
  }
  *dstmin = minw;
  *dstmax = maxw;
}
