#include "drawState.h"
#include <iostream> //XXX
#include <sstream>
#include <string>
using namespace std;


// XXX
string convBase(unsigned long v, long base)
{
	string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}

string draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height, float mousex, float mousey){
	map<index_t, complex<float_t> >::const_iterator it;
  float xborder = width/12;
  float yborder = height/12;
	float rValue,iValue;//expectation value, real value, imaginary value
	float eValue, scale = 0;
  for (unsigned int i = 0; i < state->dim; i++) {//find max value for scaling
		rValue = state->data.find(i)->second.real();
		iValue = state->data.find(i)->second.imag();
		eValue = rValue*rValue+iValue*iValue;
    if (eValue > scale){
			scale = eValue;
		}
  }
	//scaled values
	float barWidth = (width-2*xborder)/((float)state->dim);
	float sHeight = height -2*yborder;

  for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = iValue*iValue + rValue*rValue;
			cr->set_source_rgb (1,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->fill();
	for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = iValue*iValue + rValue*rValue;
			cr->set_source_rgb (0,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->stroke();
	// XXX: hack
	int i = floor((float)(mousex-xborder)/barWidth);
  if (i >= 0 && (unsigned int)i < state->dim) {
		stringstream oss;
		string mystr;
    rValue = state->data.find(i)->second.real ();
		iValue = state->data.find(i)->second.imag ();
		eValue = iValue*iValue + rValue*rValue;

		oss << "State " << convBase ((unsigned long)i, 2) << " (" << i << ") amplitude " << rValue << " + " << iValue << "i (" << 100*eValue << "%)";
		return oss.str();
	}
	return "";
}

