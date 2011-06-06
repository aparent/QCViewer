//Implimentation code for the UGate
#include "../gate.h"

UGate::UGate
UGate::UGate(string n_name,) : name(n_name) {}


string UGate::getName(){
	return name; 
}
void UGate::setName(string n_name){
	name = n_name;
}

State UGate::applyGate(State){
}
