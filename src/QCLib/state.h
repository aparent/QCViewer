#ifndef STATE__INCLUDED
#define STATE__INCLUDED
#include <map>
#include <complex>
#include <stdint.h>
#include "types.h"

// If it is desired to up the accuracy later, or change the maximum
// qubit size, change this.
typedef std::map<index_t, std::complex<float_type> >  StateMap;

class State {
public:
  State ();
  State (index_t);
  State (std::complex<float_type> amp, index_t bits);

	void print();

  std::complex<float_type> getAmplitude (index_t bits);
  const State& operator+= (const State &r);
  const State& operator-= (const State &r);
  const State& operator*= (const std::complex<float_type>);
	void normalize();
	unsigned int numBits() const;

	index_t dim;
  StateMap data;
};

State kron (const State &l,const State &r);
#endif // STATE__INCLUDED
