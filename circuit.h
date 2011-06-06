#ifndef CIRCUIT
#define CIRCUIT

#include "gate.h"
#include <vector>
#include <string>
using namespace std;

class Line {
  public:
    Line (string);
    string getInputLabel ();
    string getOutputLabel();
    string lineName;
    string outLabel;
    bool constant;
    bool garbage;
    int initValue;
};

class QArch { // Less space efficient than it could trivially be.
public:
  QArch (int nn) : n(nn) { graph = new char[n*n]; for (int i = 0; i < n*n; i++) graph[i] = 0; }

  bool query (int i, int j) { return (0 != graph[i+n*j]); }
  void set (int i, int j) { graph[i+n*j] = graph[j+n*i] = 1; }
private:
  int n;
  char *graph;
};

class Circuit {
  public:
    Circuit ();
    ~Circuit ();

    int QCost();

    void   addLine(string line);
    Line*  getLine(int pos);
    int    numLines();

    void addGate(Gate *newGate); //appends to end
    void addGate(Gate *newGate,int pos); //inserts at pos
    Gate* getGate(int pos);
    unsigned int numGates(); //Returns the number of gates

    void newArch ();
    void removeArch ();
    void parseArch (string);

    vector<int> getParallel();  //Returns a vector of ints specifying the last gate in each parallel block.
    vector<int> getGreedyParallel (); // used for drawing gates in the same column
    vector<int> getArchWarnings ();

    QArch *arch;
  private:
    vector <Gate*>           gates;
    vector <Line>            lines;
};


#endif
