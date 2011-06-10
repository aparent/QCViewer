#ifndef STATE__INCLUDED
#define STATE__INCLUDED
#include <map>
#include <complex>
#include <stdint.h>

// If it is desired to up the accuracy later, or change the maximum
// qubit size, change this.
// Templates were removed because they made the code ugly.
typedef float float_type;
typedef uint32_t index_t;
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

	index_t dim;
  StateMap data;
};

State kron (State&,State&);
#endif // STATE__INCLUDED
