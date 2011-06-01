#include "drawState.h"
using namespace std;


void draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height){
	map<index_t, complex<float_t> >::const_iterator it;
	float barWidth = width/((float)state->dim);
	float realScale = 0;
	float imagScale = 0;
  for (it = state->data.begin(); it != state->data.end(); it++) {
    if (it->second.real() > realScale){
		realScale = it->second.real();
		}
    if (it->second.imag() > imagScale){
		imagScale = it->second.imag();
		}
  }
	float rValue;
  for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			cr->set_source_rgb (0,0,0);
			cr->rectangle (barWidth*i, height-height*(rValue/realScale), barWidth, height*(rValue/realScale));
		}
  }
	cr->stroke();
}

