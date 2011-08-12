#include "state.h"
#include "utility.h" // ipow floorLog2
#include <iostream>
#include <limits>
using namespace std;

State::State () {
	dim = 0;
}

State::State (index_t n_dim) {
	dim = n_dim;
}

State::State (complex<float_type> amp, index_t bits) {
	if(amp!=complex<float_type>(0)){
  	data[bits] = amp;
	}
}

complex<float_type> State::getAmplitude (index_t bits) {
  if (data.find(bits) == data.end())
     return 0;
  else
     return data[bits];
}

const State& State::operator+= (const State& r) {
  map<index_t, complex<float_type> >::const_iterator it;
  for (it = r.data.begin(); it != r.data.end(); ++it) {
		if (it->second != complex<float_type>(0)){
    	data[it->first] += it->second;
			float_type e = 10*numeric_limits<float_type>::epsilon();
			if (abs(data[it->first]) < e ){
				data.erase(it->first);
			}
		}
  }
  return *this;
}

const State& State::operator-= (const State& r) {
  map<index_t, complex<float_type> >::const_iterator it;
  for (it = r.data.begin(); it != r.data.end(); ++it) {
		if (it->second != complex<float_type>(0)){
    	data[it->first] -= it->second;
			float_type e = 10*numeric_limits<float_type>::epsilon();
			if (abs(data[it->first].real()) < e){
				data.erase(it->first);
			}
		}
  }
  return *this;
}

const State& State::operator*= (const complex<float_type> x) {
  map<index_t, complex<float_type> >::const_iterator it;
  for (it = data.begin(); it != data.end(); ++it) {
    data[it->first] *= x;
  }
  return *this;
}

unsigned int State::numBits(){
	return floorLog2(dim); 
}

State kron (State& l, State& r){
  StateMap::iterator it_r;
  StateMap::iterator it_l;
	State ret;
  for (it_l = l.data.begin(); it_l != l.data.end(); ++it_l) {
  	for (it_r = r.data.begin(); it_r != r.data.end(); ++it_r) {
			ret.data[(it_l->first < r.numBits())|(it_r->first)] = it_l->second*it_r->second;
		}
	}
	ret.dim = l.dim*r.dim;
	return ret;
}

void State::normalize(){
	StateMap::const_iterator it;
  float_t norm = 0;
	float_t real;
	float_t imag;
	for (it = data.begin(); it != data.end(); ++it) {
    real = it->second.real ();
		imag = it->second.imag ();
		norm += real*real + imag*imag;
  }
	norm = sqrt(norm);
  for (it = data.begin(); it != data.end(); ++it) {
		data[it->first] = (it->second) / complex<float_type>(norm,0);
	}
}

void State::print(){
	map<index_t, complex<float_type> >::const_iterator it;
  for (it = data.begin(); it != data.end(); ++it) {
		printIntBin(it->first);
    cout << ":" << it->second << endl;
  }
}
