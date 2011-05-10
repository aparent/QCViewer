#include "gate.h"
#include "utility.h"

Control::Control(int setWire, bool setPol){
  wire = setWire;
  polarity = setPol;
}   

int Gate::numCont(){
  return controls.size();
}

int Gate::numNegCont(){
  int count = 0;
  for(int i=0; i<controls.size(); i++){
    if (controls.at(i).polarity) count++; //increment if true (negative control)
  }
  return count;
}

int CNOTGate::QCost(int numLines){ //TODO: Check for Pres Gates
  int c  = numCont(); //m
  int nc = numNegCont(); //v
  int l  = numLines; //n
  if ( c == 0 || (c == 1 && nc == 0)) return 1; //NOT & CNOT
	switch (c) {
    case 1:  
      if (nc == 1) return 3;
    case 2:  //Toffoli
			if(nc==2) return 6;
			else      return 5;
    case 3: //t4
      if (nc == 3) return 15;
			else         return 13;
    case 4: //t5
			if(l-c-1>=c-2){//atleast m-2 garbage lines exist Note -1 is because of the target
				if(nc==4) return 28;
				else      return 26;
			}
      else {
			  if(nc==4) return 31; //////////////////////????????????????????
				else      return 29;
			}
      case 5: //t6
			  if(l-c-1 >= c-2){//m-2garbage lines
				  if (nc == 5) return 40;
					else         return 38;
					}
        else if (l-c>=2) {// one garbage line
				  if(nc==5)return 54;
					else     return 52;
				}
        else { 
				  if(nc==5)return 63;
					else     return 61;
				}
      case 6: //t7
		    if(l-c-1>=c-2){//m-2garbage lines
			    if(nc==6) return 52;
			  	  else    return 50;
			 }
       else if (l-c >= 2) {// one garbage line
			   if (nc == 6) return 82;
				 else         return 80;
			 }
       else {//no garbage
			   if(nc==6) return 127;
				 else      return 125;
			 }
     case 7: //t8
		   if(l-c-1>=c-2){//m-2garbage lines
			   if(nc==7) return 64;
				 else      return 62;
			 }
       else if (l-c>=2) {// one garbage line
			   if ( nc== 7) return 102;
			   else         return 100;
			 }
       else {//no garbage
			   if(nc==7) return 255;
			   else      return 253;
			 }
     case 8: //t9
       if(l-c-1>=c-2){//m-2garbage lines
						if(nc==8) return 76;
						else      return 74;
					}
       else if (l-c>=2) {// one garbage line
						if(nc==8)return 130;
						else     return 128;
			 }
       else {//no garbage
						if(nc==8)return 511;
						else     return 509;
			 }
     case 9: //t10
		   if(l-c-1>=c-2){//m-2garbage lines
		     if(nc==9)return 88;
				 else     return 86;
			 }
       else if (l-c>=2) {// one garbage line
			   if(nc==9) return 154;
				 else      return 152;
			 }
       else {//no garbage
			   if(nc==9) return 1023;
			   else      return 1021;
			 }
     default:
       if (c>=10){//tn
	       if(l-c-1>=c-2) {//m-2 garbage lines
						if(nc==c) return 12*c-20;
						else      return 12*c-22;
					}
          else if (l-c>=2) {// one garbage line
				    if(nc==c)return 24*c-62;
				    else   return 24*c-64;
			    }
          else {//no garbage
			     if(nc==c) return (int)ipow(2,c+1)-1;		
           else      return (int)ipow(2,c+1)-3;                              
          }
       }
  }
  return 0;
}
