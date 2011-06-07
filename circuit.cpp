#include "circuit.h"
#include "utility.h"
#include <map>
#include <algorithm> // for sort, which we should probably cut out
#include <fstream>

Circuit::Circuit() : arch(0) {}

Circuit::~Circuit () {
  removeArch ();
}

void Circuit::newArch () {
  arch = new QArch (numLines());
}

void Circuit::removeArch () {
  if (arch != 0) {
    delete arch;
    arch = 0;
  }
}

void Circuit::addGate(Gate *newGate){
  gates.push_back(newGate);
}

void Circuit::addGate(Gate *newGate, int pos){
  gates.insert(gates.begin()+pos, newGate);
}

Gate* Circuit::getGate(int pos){
  return gates.at(pos);
}

unsigned int Circuit::numGates(){
  return gates.size();
}

int Circuit::QCost(){
  return 0;
}

string Line::getInputLabel(){
  if (constant){
    return intToString(initValue);
  }
  return lineName;
}

string Line::getOutputLabel(){
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

vector<int> Circuit::getParallel(){
  vector<int>  returnValue;
  map<int,int> linesUsed;
  for(unsigned int i = 0; i<numGates(); i++){
    Gate *g = getGate(i);
    start:
    for(unsigned int j = 0; j < g->controls.size(); j++){
      if (linesUsed.find(g->controls[j].wire) != linesUsed.end()){
        returnValue.push_back(i - 1); //Push back the gate number before this one
        linesUsed.clear();
        goto start; //Go back to begining of main loop (redo this iteration because this gate is in the next block)
      }
      linesUsed[g->controls[j].wire];
    }
    for(unsigned int j = 0; j < g->targets.size(); j++) {
      if (linesUsed.find(g->targets[j]) != linesUsed.end()) {
        returnValue.push_back(i - 1);
        linesUsed.clear();
        goto start;
      }
      linesUsed[g->targets[j]];
    }
  }
  returnValue.push_back (numGates()-1); // for convenience.
  return returnValue;
}

// TODO: this is pretty akward to have outside the drawing code. Reorganize?
vector<int> Circuit::getGreedyParallel(){
  vector<int> parallel = getParallel (); // doing greedy sometimes "tries too hard"; we need to do greedy within the regions defined here (XXX: explain this better)
  sort (parallel.begin (), parallel.end ());
  vector<int>  returnValue;
  map<int,int> linesUsed;
  unsigned int maxw, minw;
  int k = 0;
  for(unsigned int i = 0; i < numGates(); i++){
    start:
    Gate *g = getGate(i);
    minmaxWire (&g->controls, &g->targets, &minw, &maxw);
    for (unsigned int j = minw; j <= maxw; j++) {
      if (linesUsed.find(j) != linesUsed.end()) {
        returnValue.push_back(i - 1);
        linesUsed.clear ();
        goto start;
      }
      linesUsed[j];
    }
    if (i == (unsigned int)parallel[k]) { // into next parallel group, so force a column move
      returnValue.push_back (i);
      k++;
      linesUsed.clear ();
    }
  }
  for (; k < (int)parallel.size(); k++) {
    returnValue.push_back (k);
  }
  sort (returnValue.begin (), returnValue.end ()); // TODO: needed?
//  returnValue.push_back (numGates()-1); // for convenience.
  return returnValue;
}

vector<int> Circuit::getArchWarnings () {
  vector<int> warnings;
  vector<unsigned int> wires;
  if (arch == 0) return warnings; // Assume "no" architecture by default.
  for (unsigned int g = 0; g < gates.size(); g++) {
    wires = getGate(g)->targets;
    Gate* gg = getGate (g);
    for (unsigned int i = 0; i < gg->controls.size(); i++) {
      wires.push_back (gg->controls[i].wire);
    }
    bool badgate = false;
    for (unsigned int i = 0; i < wires.size () && !badgate; i++) {
      for (unsigned int j = i + 1; j < wires.size () && !badgate; j++) {
        if (!arch->query (wires[i],wires[j])) badgate = true;
      }
    }
    if (badgate) warnings.push_back(g);
  }
  return warnings;
}

// TODO: check for errors
void Circuit::parseArch (const string filename) {
  ifstream file;
  file.open (filename.c_str(), ios::in);
  if (!file.is_open()) return;
  unsigned int n;
  file >> n;
  arch = new QArch(n);
  for (unsigned int i = 0; i < n; i++) {
    int m;
    file >> m;
    for (int j = 0; j < m; j++) {
      int q;
      file >> q;
      arch->set (i, q);
    }
  }
  file.close ();
}
