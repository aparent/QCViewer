#ifndef STATE__INCLUDED
#define STATE__INCLUDED
#include <map>
#include <complex>
#include <stdint.h>

// If it is desired to up the accuracy later, or change the maximum
// qubit size, change this.
// Templates were removed because they made the code ugly.
typedef float float_t;
typedef uint32_t index_t;
typedef std::map<index_t, std::complex<float_t> >  StateMap;

class State {
public:
  State ();
  State (index_t);
  State (std::complex<float_t> amp, index_t bits);

	void print();

  std::complex<float_t> getAmplitude (index_t bits);
  const State& operator+= (const State &r);
  const State& operator-= (const State &r);
  const State& operator*= (const std::complex<float_t>);
	void normalize();

	index_t dim;
  StateMap data;
};

/* TODO: soon!!!!
class BitString {
public:
  BitString (uint32_t len);

  void get (uint32_t pos);
	void set (uint32_t pos, uint8_t val);
	void and (BitString &);
  void or (BitString &);
	void flip ();
	void clear ();
  void map (...);

	bool operator== (const BitString&) const;
private:

};
*/

State kron (State&,State&);
#endif // STATE__INCLUDED
