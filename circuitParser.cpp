#include "circuitParser.h"
#include <iostream>

using namespace std;

// NOTE: (*(++(*it))) means (*it): dereferance it, ++(*it): call the ++ operator on the iterator,
// *(++(*it)): call the * operator on the iterator, note this is overloaded and actually means the value of
// the vector location where it points now

//TODO: parseCircuit should return null in the event of a parsing error 

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

bool parseGateInputs(Gate *gate, Circuit *circ, vector<TFCToken>::iterator * it){
	bool found,error=false; 
  while((*(++(*it))).type == GATE_INPUT || (**it).type == GATE_INPUT_N){
		found = false; 
    for(int j = 0; j < circ->numLines(); j++){
      if (((**it).value).compare(circ->getLine(j)->lineName)==0){
				if ((**it).type == GATE_INPUT_N){
        	gate->controls.push_back(Control(j,true));
					found = true; 
        	break;
				}
				else{
        	gate->controls.push_back(Control(j,false));
					found = true; 
        	break;
				}
      }
    }
		if (!found){
			cout << "ERROR unknown wire: " << ((**it).value) << ". On:" << gate->getName() << "." << endl;
			error = true;
		}
  }
	if (error){
		return false;
	}
	unsigned int numTarg;
	if (gate->getName().compare("F")==0){
		numTarg = 2;
	}
	else numTarg = 1;
	if (numTarg > gate->controls.size()){
			cout << "ERROR Not enough targets." << endl;
			delete gate;
			gate = NULL;
			return false;
	}
	for (unsigned int i = 0; i < numTarg; i++){
  	gate->targets.push_back(gate->controls.back().wire);
		gate->controls.pop_back();
	}
	return true;
}

void parseGates(Circuit *circ, vector<TFCToken>::iterator * it){
	(*it)++;
  while((**it).type != SEC_END){
  	Gate *newGate;
		if(((**it).value).compare("R") == 0){
			(*it)++;
			if ((**it).type != GATE_SET){
				cout << "ERROR: No setting for R gate."<< endl;
			}
			istringstream ss((**it).value); 
			float_t rot;
			ss >>  rot;
  		newGate = new RGate(rot); //sets rotation amount 
		} else if (((**it).value[0]) == 'T'){
  		newGate = new UGate("X");
			newGate->drawType = NOT;
		} else if (((**it).value[0]) == 'F'){
  		newGate = new UGate("F");
			newGate->drawType = FRED;
		} else {
  		newGate = new UGate((**it).value);
		}
		if(parseGateInputs(newGate,circ,it)){ //Will return true if it succeeds 
  		circ->addGate(newGate);
		} else {
			cout << "Ommitting gate due to errors" << endl;
  	}
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
