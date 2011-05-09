#include "circuit.h"

void Circuit::addGate(Gate newGate){
  gates.push_back(newGate);
}

void Circuit::addGate(Gate newGate, int pos){
  gates.insert(gates.begin()+pos,newGate);
}

Gate Circuit::getGate(int pos){
  return gates.at(pos);
}


int Circuit::numGates(){
  return gates.size();
}

int Circuit::QCost(){
  int totalCost =0;
  for(int i = 0; i < numGates(); i++){
    totalCost = totalCost + getGate(i).QCost();
  } 
  return totalCost;
}
