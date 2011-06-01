#include "circuitParser.h"

using namespace std;

//NOTE: (*(++(*it))) means (*it): dereferance it, ++(*it): call the ++ operator on the iterator,
//*(++(*it)): call the * operator on the iterator, note this is overloaded and actually means the value of
//the vector location where it points now

void parseLineNames(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
    circ->addLine((**it).value);
	}
}

void parseInputs(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
		for(int j = 0; j < circ->numLines(); j++){
      if ((**it).value.compare(circ->getLine(j)->lineName)==0){
        circ->getLine(j)->constant=false;
        break;
      }
		//error
    }
	}
}

void parseOutputs(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
		for(int j = 0; j < circ->numLines(); j++){
      if ((**it).value.compare(circ->getLine(j)->lineName)==0){
        circ->getLine(j)->garbage=false;
        break;
      }
    }
		//error
	}
}

void parseOutputLabels(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
		for(int j = 0; j < circ->numLines(); j++){
      if (!circ->getLine(j)->garbage){
      	circ->getLine(j)->outLabel = (**it).value;
        break;
      }
    }
		//error
	}
}

void parseGateInputs(Gate *gate, Circuit *circ, vector<TFCToken>::iterator * it){
  while((*(++(*it))).type == GATE_INPUT || (**it).type == GATE_INPUT_N){
    for(int j = 0; j < circ->numLines(); j++){
      if (((**it).value).compare(circ->getLine(j)->lineName)==0){
				if ((**it).type == GATE_INPUT_N){
        	gate->controls.push_back(Control(j,true));
        	break;
				}
				else{
        	gate->controls.push_back(Control(j,false));
        	break;
				}
      }
    }
  }
	int numTarg;
	if (gate->gateType==FRED){ 
		numTarg = 2;
	} 
	else numTarg = 1;
	for (int i = 0; i < numTarg; i++){
  	gate->targets.push_back(gate->controls.back().wire);
		gate->controls.pop_back();
	}
}

void parseGates(Circuit *circ, vector<TFCToken>::iterator * it){
	(*it)++;
  while((**it).type != SEC_END){
  	Gate *newGate;
		if (((**it).value).compare("H") == 0 || ((**it).value).compare("h") == 0){
  		newGate = new HGate();
  		newGate->name = "H";
		}
		else if(((**it).value[0]) == 't' ||((**it).value[0]) == 'T'){
  		newGate = new NOTGate();
  		newGate->name = "T";
		}
		else if(((**it).value[0]) == 'f' ||((**it).value[0]) == 'F'){
  		newGate = new FredGate();
  		newGate->name = "F";
		}
		else{
  		newGate = new UGate();
  		newGate->name = ((**it).value);	
		} 
  	parseGateInputs(newGate,circ,it);
  	circ->addGate(newGate);
  }
}

void parseConstants(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
		for(int j = 0; j < circ->numLines(); j++){
      if (circ->getLine(j)->constant){
      	circ->getLine(j)->initValue = atoi(((**it).value).c_str());
        break;
      }
    }
		//error
	}
}

Circuit *parseCircuit (string file){
	vector<TFCToken> *tokens = lexCircuit(file);
	if (tokens == NULL) return NULL;
	vector<TFCToken>::iterator tempIt = tokens->begin();
	vector<TFCToken>::iterator * it = &tempIt;
  Circuit *circ = new Circuit;
	for(; ;){
		if ((**it).type == SEC_START){
			if (((**it).value).compare("V")     == 0){
				parseLineNames(circ,it);
			}
			if (((**it).value).compare("I")     == 0){
				parseInputs(circ,it);
			}
			if (((**it).value).compare("O")     == 0){
				parseOutputs(circ,it);
			}
			if (((**it).value).compare("OL")    == 0){
				parseOutputLabels(circ,it);
			}
			if (((**it).value).compare("C")     == 0){
				parseConstants(circ,it);
			}
			if (((**it).value).compare("GATES") == 0){
				parseGates(circ,it);
			}
		}
		else if((**it).type == SEC_END){
			break;
		} else {
			delete circ;
			return NULL; // TODO: do it better
		}
	}
	return circ;
}
