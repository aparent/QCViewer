#include "circuit.h"
#include "utility.h"

void Circuit::addGate(Gate *newGate){
  gates.push_back(newGate);
}

void Circuit::addGate(Gate *newGate, int pos){
  gates.insert(gates.begin()+pos,newGate);
}

Gate* Circuit::getGate(int pos){
  return gates.at(pos);
}

int Circuit::numGates(){
  return gates.size();
}

int Circuit::QCost(){
  int totalCost =0;
  for(int i = 0; i < numGates(); i++){
    totalCost = totalCost + getGate(i)->QCost(numLines());
  } 
  return totalCost;
}

string Line::getInputLabel(){
  if (constant){
    return intToString(initValue);
  }
  return lineName;
}
    
string  Line::getOutputLabel(){
  if (garbage){ 
    return "Garbage";
  }
  if (outLabel.compare("")==0){ 
    return lineName;
  }
  return outLabel;
}

int Circuit::numLines(){
  return lines.size();
}

Line* Circuit::getLine(int pos){
  return &lines.at(pos);
}

Line::Line(string name){
  lineName  = name;
  garbage   = true;
  constant  = true;
  initValue = 0;
}

void Circuit::addLine(string lineName){
  lines.push_back(Line(lineName));
}
