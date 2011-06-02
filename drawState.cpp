#include "drawState.h"
using namespace std;


void draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height){
	map<index_t, complex<float_t> >::const_iterator it;
	float barWidth = width/((float)state->dim);
	float eValue, scale = 0;
  for (it = state->data.begin(); it != state->data.end(); it++) {//find max value for scaling
		eValue = pow(it->second.real(),2)+pow(it->second.imag(),2);
    if (eValue > scale){
			scale = eValue;
		}
  }
	float rValue,iValue;//expectation value, real value, imaginary value
  for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = pow(iValue,2) + pow(rValue,2);
			cr->set_source_rgb (0,0,0);
			cr->rectangle (barWidth*i, height*(1-(eValue/scale)), barWidth, height*(eValue/scale));
		}
  }
	cr->stroke();
}

