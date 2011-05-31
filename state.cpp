#include "state.h"
#include "utility.h" // ipow
#include <complex>

using namespace std;

template <class IndexType, class NumType>
State<IndexType, NumType>::State () {}

template <class IndexType, class NumType>
State<IndexType, NumType>::State (complex<NumType> amp, IndexType bits) {
  data[bits] = amp;
}

template <class IndexType, class NumType>
State<IndexType, NumType>::State (stateVec &v) {
  //assert (v.dim <= sizeof(indexType)*8);
  for (IndexType i = 0; i < (IndexType)ipow(2,v.dim); i++) {
    if (v.data[i] != 0) {
      data[i] = v.data[i];
    }
  }
}

template <class IndexType, class NumType>
complex<NumType> State<IndexType, NumType>::getAmplitude (IndexType bits) {
  if (data.find(bits) == map<IndexType, complex<NumType> >::end)
     return 0;
  else
     return data[bits];
}

template <class IndexType, class NumType>
State<IndexType, NumType>& State<IndexType, NumType>::operator+= (const State<IndexType, NumType> &r) {
  typename map<IndexType, complex<NumType> >::iterator it;
  for (it = r.begin(); it != r.end(); it++) {
    data[it->first] += it->second;
  }
  return *this;
}
