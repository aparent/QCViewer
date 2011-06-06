#include "gate.h"
#include "utility.h"

Control::Control(int setWire, bool setPol) : wire(setWire), polarity (setPol) {}

void minmaxWire (vector<Control>* ctrl, vector<int>* targ, int *dstmin, int *dstmax) {
  int minw = (*targ)[0];
  int maxw = (*targ)[0];
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
