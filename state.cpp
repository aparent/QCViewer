#include "state.h"
#include "utility.h" // ipow
#include <complex>
#include <iostream>
#include <limits>

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
			float e = numeric_limits<float_t>::epsilon();
			if (data[it->first].real() < e && data[it->first].imag() < e){//TODO: Probably want some multiple of e
				data.erase(it->first);
			}
		}
  }
  return *this;
}

const State& State::operator-= (const State& r) {
  map<index_t, complex<float_t> >::const_iterator it;
  for (it = r.data.begin(); it != r.data.end(); it++) {
		if (it->second != complex<float_t>(0)){
    	data[it->first] -= it->second;
			float e = numeric_limits<float_t>::epsilon();
			if (data[it->first].real() < e && data[it->first].imag() < e){//TODO: Probably want some multiple of e
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

void kron (state& r){
  for (it = data.begin(); it != r.data.end(); it++) {
  for (it = r.data.begin(); it != r.data.end(); it++) {
		}
	}
}

void State::normalize(){
	map<index_t, complex<float_t> >::const_iterator it;
	float_t normFact = 0;
  for (it = data.begin(); it != data.end(); it++) {
		normFact+= it->second.real()*it->second.real()+it->second.imag()*it->second.imag();
  }
	normFact = sqrt(normFact);
  for (it = data.begin(); it != data.end(); it++) {
		it->second = it->second/normFact;
	}
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
