#ifndef STATE__INCLUDED
#define STATE__INCLUDED
#include <map>
#include <complex>
#include "dirac.h"
#include <stdint.h>

// If it is desired to up the accuracy later, or change the maximum
// qubit size, change this.
// Templates were removed because they made the code ugly.
#define float_t float
#define index_t uint32_t

class State {
public:
  State ();
  State (std::complex<float_t> amp, index_t bits);
  State (stateVec &v);

  std::complex<float_t> getAmplitude (index_t bits);
  const State& operator+= (const State &r);

  std::map<index_t, std::complex<float_t> > data;
};

#endif // STATE__INCLUDED
