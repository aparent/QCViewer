#include "gate.h"
#include <complex>
#include <string>

complex<float> *getGateMatrix(string gateName){
	complex<float> * ret;
	if (gateName.compare("H")     == 0){
		ret = new complex<float>[4]
		ret[0]=  1 ; ret[1]=  1;
		ret[2]=  1 ; ret[3]= -1;
	}
	if (gateName.compare("T")     == 0){
		ret = new complex<float>[4]
		ret[0]=  0 ; ret[1]=  1;
		ret[2]=  1 ; ret[3]=  0;
	}
	if (gateName.compare("F")     == 0){
		ret = new complex<float>[16]
		ret[0 ]=  1 ; ret[1 ]=  0; ret[2 ]=  0 ; ret[3 ]=  0;
		ret[4 ]=  0 ; ret[5 ]=  0; ret[6 ]=  1 ; ret[7 ]=  0;
		ret[8 ]=  0 ; ret[9 ]=  1; ret[10]=  0 ; ret[11]=  0;
		ret[12]=  0 ; ret[13]=  0; ret[14]=  0 ; ret[15]=  1;
	}
} 
