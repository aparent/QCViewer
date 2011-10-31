//Might want to jsut replace this with a bison parser since it would be easier to maintain
#include "circuitParser.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

void parseLineNames(Circuit * circ, vector<QCToken>::iterator &it){
  while((*(++it)).type == VAR_NAME){
    circ->addLine((*it).value);
  }
}

void parseInputs(Circuit * circ, vector<QCToken>::iterator &it){
  while((*(++it)).type == VAR_NAME){
    for(unsigned int j = 0; j < circ->numLines(); j++){
      if ((*it).value.compare(circ->getLine(j)->lineName)==0){
        circ->getLine(j)->constant=false;
        break;
      }
    //error
    }
  }
}

void parseOutputs(Circuit * circ, vector<QCToken>::iterator &it){
  while((*(++it)).type == VAR_NAME){
    for(unsigned int j = 0; j < circ->numLines(); j++){
      if ((*it).value.compare(circ->getLine(j)->lineName)==0){
        circ->getLine(j)->garbage=false;
        break;
      }
    }
    //error
  }
}

void parseOutputLabels(Circuit * circ, vector<QCToken>::iterator &it){
  while((*(++it)).type == VAR_NAME){
    for(unsigned int j = 0; j < circ->numLines(); j++){
      if (!circ->getLine(j)->garbage){
        circ->getLine(j)->outLabel = (*it).value;
        break;
      }
    }
    //error
  }
}

bool inControls(vector<Control> &ctr ,unsigned int n){
	for (unsigned int i = 0; i < ctr.size(); i++){
		if (ctr[i].wire == n) return true;
	}
	return false;
}

bool parseGateInputs(Gate *gate, Circuit *circ, vector<QCToken>::iterator &it){
  bool found;
  while((*(++it)).type == GATE_INPUT || (*it).type == GATE_INPUT_N){
    found = false;
    for (unsigned int j = 0; j < circ->numLines(); j++){	
      if (((*it).value).compare(circ->getLine(j)->lineName)==0){
				if (inControls(gate->controls,j)){ 
					cerr << "ERROR: repeated control on " << gate->getName() << "." << endl;
					return false;
				}
        if ((*it).type == GATE_INPUT_N){
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
      cerr << "ERROR unknown wire: " << ((*it).value) << ". On:" << gate->getName() << "." << endl;
    	return false;
    }
  }
  unsigned int numTarg;
  if (gate->getName().compare("F")==0){
    numTarg = 2;
  }
  else numTarg = 1;
  if (numTarg > gate->controls.size()){
      cerr << "ERROR Not enough targets." << endl;
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

void parseGates(Circuit *circ, vector<QCToken>::iterator &it){
  it++;
  while((*it).type != SEC_END){
    Gate *newGate;
    if(((*it).value).compare("R") == 0){
      it++;
      if ((*it).type != GATE_SET){
        cout << "ERROR: No setting for R gate."<< endl;
      }
			char t = (*it).value[0];//for rot type
			RGate::Axis rot_type;
      stringstream ss((*it).value);
			if (t=='x'||t=='X'){
				rot_type = RGate::X;
				ss.ignore(1);
			} else if (t=='y'||t=='Y'){
				rot_type = RGate::Y;
				ss.ignore(1);
			} else if (t=='Z'||t=='z'){
				rot_type = RGate::Z;
				ss.ignore(1);
			} else {
				rot_type = RGate::Z;
			}
      float_type rot;
      ss >>  rot;
      newGate = new RGate(rot, rot_type); //sets rotation amount
    } else if (((*it).value[0]) == 'T'){
      newGate = new UGate("X");
      newGate->drawType = Gate::NOT;
    } else if (((*it).value[0]) == 'F'){
      newGate = new UGate("F");
      newGate->drawType = Gate::FRED;
    } else {
      newGate = new UGate((*it).value);
    }
    if(parseGateInputs(newGate,circ,it)){ //Will retun true if it succeeds
      circ->addGate(newGate);
    } else {
      cout << "Ommitting gate due to errors" << endl;
    }
  }
}

void parseConstants(Circuit * circ, vector<QCToken>::iterator &it){
  while((*(++it)).type == VAR_NAME){
    for(unsigned int j = 0; j < circ->numLines(); j++){
      if (circ->getLine(j)->constant){
        circ->getLine(j)->initValue = atoi(((*it).value).c_str());
        break;
      }
    }
    //error
  }
}

Circuit *parseCircuit (string file){
  Circuit *circ = new Circuit;

	//removes the file type from the circuit name
	bool name_set = false;
	unsigned int slash=0;
	for (unsigned int i = 0;i<file.size();i++){
		if (file[i]=='/' || file[i]=='\\'){
			slash = i+1;
		}
		if (file[i]=='.'){
			circ->name = file.substr(slash,i-slash);
			name_set = true;
			break;
		}
	}
	if (!name_set){
		circ->name = file;
	}

  vector<QCToken> *tokens = lexCircuit(file);
  if (tokens == NULL) return NULL;
  vector<QCToken>::iterator it = tokens->begin();
  for(; ;){
    if ((*it).type == SEC_START){
      if (((*it).value).compare("V")     == 0){
        parseLineNames(circ,it);
      }
      if (((*it).value).compare("I")     == 0){
        parseInputs(circ,it);
      }
      if (((*it).value).compare("O")     == 0){
        parseOutputs(circ,it);
      }
      if (((*it).value).compare("OL")    == 0){
        parseOutputLabels(circ,it);
      }
      if (((*it).value).compare("C")     == 0){
        parseConstants(circ,it);
      }
      if (((*it).value).compare("GATES") == 0){
        parseGates(circ,it);
      }
    }
    else if((*it).type == SEC_END){
      break;
    } else {
      delete circ;
      return NULL; // TODO: do it better
    }
  }
  return circ;
}



string getGateInfo(Circuit *circ){
	Gate *gate; //for current gate
	stringstream ret;
	ret << "BEGIN\n";
	for (unsigned int i = 0; i< circ->numGates(); i++){
		gate = circ->getGate(i);
		if(gate->type == Gate::RGATE){
			string symbol;
			if (((RGate*)gate)->get_axis()==RGate::X){
				symbol = "X";
			}else if (((RGate*)gate)->get_axis()==RGate::Y){
				symbol = "Y";
			}if (((RGate*)gate)->get_axis()==RGate::Z){
				symbol = "Z";
			}
			ret << "R(" << symbol << ((RGate*)gate)->get_rotVal() << ")";
		} else {
			ret << gate->getName();
		}
		for (unsigned int j = 0; j < gate->controls.size() ; j++){
			ret << " \"" << circ->getLine(gate->controls[j].wire)->lineName << "\"";
		}
		for (unsigned int j = 0; j < gate->targets.size() ; j++){
			ret << " \"" << circ->getLine(gate->targets[j])->lineName << "\"";
		}
		ret << "\n";
	}
	ret << "END";
	return ret.str();
}

string getCircuitInfo(Circuit *circ){
	stringstream v,in,o,ol,c,ret;  //correspond to simlarly named sections in the file
	v << ".v" ; in << ".i"; o << ".o"; ol << ".ol"; c << ".c";
	Line *line;  //for current line
	for (unsigned int i = 0; i< circ->numLines(); i++){
		line = circ->getLine(i);
		v << " \"" << line->lineName << "\"";
		if (line->constant){
			c << " " << line->initValue;
		} else {
		 in << " \"" << line->lineName << "\"";
		}
		if (!line->garbage){
			o << " \""<< line->lineName << "\"";
		}
		if (line->outLabel.compare("")!=0){
			ol << " \""<< line->lineName << "\"";
		}
	}
	v << "\n"; in << "\n"; o << "\n"; ol << "\n"; c << "\n";
	ret << v.str() << in.str() << o.str() << ol.str() << c.str();
	return ret.str();
}

void saveCircuit(Circuit *circ, string filename){
	ofstream f;
  f.open (filename.c_str());
	string circInfo = getCircuitInfo(circ);
	string gateInfo = getGateInfo(circ);
	f << circInfo << gateInfo;
	f.close();
}
