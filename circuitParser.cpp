#include "circuitParser.h"

int gateCount = 0;

using namespace std;

string removeQuotes(string str){
	if (str[0] == '"'){
		str.erase(str.begin());
		str.erase(str.end());
	}
	return str;
}

//NOTE: (*(++(*it))) means (*it): dereferance it, ++(*it): call the ++ operator on the iterator,
//*(++(*it)): call the * operator on the iterator, note this is overloaded and actually means the value of
//the vector location where it points now

void parseLineNames(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
    circ->addLine(removeQuotes((**it).value));
	}
}

void parseInputs(Circuit * circ, vector<TFCToken>::iterator * it){
	while((*(++(*it))).type == VAR_NAME){
		for(int j = 0; j < circ->numLines(); j++){
      if (removeQuotes((**it).value).compare(circ->getLine(j)->lineName)==0){
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
      if (removeQuotes((**it).value).compare(circ->getLine(j)->lineName)==0){
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
      	circ->getLine(j)->outLabel = removeQuotes((**it).value);
        break;
      }
    }
		//error
	}
}

void parseGateInputs(Gate *gate, Circuit *circ, vector<TFCToken>::iterator * it){
  bool target = true;
  while((*(++(*it))).type == VAR_VALUE){
    for(int j = 0; j < circ->numLines(); j++){
      if (removeQuotes(((**it).value)).compare(circ->getLine(j)->lineName)==0){
        gate->controls.push_back(Control(j,false));
        break;
      }
    }
  }
	gate->controls.pop_back();
	--(*it);
  for(int j = 0; j < circ->numLines(); j++){
    if (removeQuotes((**it).value).compare(circ->getLine(j)->lineName)==0){
      gate->targets.push_back(j);
      continue;
    }
  }
}

void parseGates(Circuit *circ, vector<TFCToken>::iterator * it){
  while((*(++(*it))).type != SEC_END){
  	Gate *newGate;
		if (((**it).value).compare("H") == 0 || ((**it).value).compare("h") == 0){
  		newGate = new HGate();
		}
		else{
  		newGate = new NOTGate();
		}
  	newGate->name = ((**it).value);
		cout << gateCount++ << endl;
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
	cout << "Lexing...";
	vector<TFCToken> *tokens = lexCircuit(file);
	cout << "done" << endl; 
	cout << "Parseing..." << endl;
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
		}
		//else ...  TODO: Error?
	}
	return circ;
}
