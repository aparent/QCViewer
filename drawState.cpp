#include "drawState.h"
#include "iostream" //XXX
using namespace std;


void draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height){
	map<index_t, complex<float_t> >::const_iterator it;
	float xborder = width/12;
	float yborder = height/12;
	float eValue, scale = 0;
  for (it = state->data.begin(); it != state->data.end(); it++) {//find max value for scaling
		eValue = pow(it->second.real(),2)+pow(it->second.imag(),2);
    if (eValue > scale){
			scale = eValue;
		}
  }
	float rValue,iValue;//expectation value, real value, imaginary value
	//scaled values
	float barWidth = (width-2*xborder)/((float)state->dim);
	float sHeight = height -2*yborder;

  for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = pow(iValue,2) + pow(rValue,2);
			cr->set_source_rgb (1,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->fill();
	for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = pow(iValue,2) + pow(rValue,2);
			cr->set_source_rgb (0,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->stroke();
}

