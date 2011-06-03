#ifndef  DIRAC_INCLUDE
#define  DIRAC_INCLUDE

#include <string>
#include <complex>

struct stateVec {
	unsigned int dim;
  std::complex<float> *data;
};

stateVec *getStateVec (std::string,bool=false);
#endif
