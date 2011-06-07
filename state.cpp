#include "state.h"
#include "utility.h" // ipow
#include <complex>
#include <iostream>

using namespace std;

State::State () {
	dim = 0;
}

State::State (complex<float_t> amp, index_t bits) {
	if(amp!=complex<float_t>(0)){
  	data[bits] = amp;
	}
}

State::State (stateVec *v) {
  //assert (v.dim <= sizeof(index_t)*8)i;
	if ( v != NULL ){
		dim = v->dim;
  	for (index_t i = 0; i < (index_t)v->dim; i++) {
    	if (v->data[i] != complex<float_t>(0)) {
    	  data[i] = v->data[i];
    	}
  	}
	}
}

complex<float_t> State::getAmplitude (index_t bits) {
  if (data.find(bits) == data.end())
     return 0;
  else
     return data[bits];
}

const State& State::operator+= (const State& r) {
  map<index_t, complex<float_t> >::const_iterator it;
  for (it = r.data.begin(); it != r.data.end(); it++) {
		if (it->second != complex<float_t>(0)){
    	data[it->first] += it->second;
			if (data[it->first] == complex<float_t>(0)){
				data.erase(it->first);
			}
		}
  }
  return *this;
}

const State& State::operator*= (const complex<float_t> x) {
  map<index_t, complex<float_t> >::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    data[it->first] *= x;
  }
  return *this;
}

void State::print(){
	map<index_t, complex<float_t> >::const_iterator it;
	cout << "Printing state:" << endl;
  for (it = data.begin(); it != data.end(); it++) {
		printIntBin(it->first);
    cout << ":" << it->second << endl;
  }
	cout << "-----------------------------------------------" << endl;
}
