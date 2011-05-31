#ifndef STATE__INCLUDED
#define STATE__INCLUDED
#include <map>
#include <complex>
#include "dirac.h"

template <class IndexType, class NumType> 
class State {
public:
  State ();
  State (std::complex<NumType> amp, IndexType bits);
  State (stateVec &v);

  std::complex<NumType> getAmplitude (IndexType bits);
  State<IndexType, NumType>& operator+= (const State<IndexType, NumType> &r);

private:
  std::map<IndexType, std::complex<NumType> > data;
};

#endif // STATE__INCLUDED
