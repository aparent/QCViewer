
#include <string>
#include <memory>
#include <vector>

class GateApp;
class Gate;
class Circuit;

class Gate
{
public:
enum GateType {GATE, CIRC, BREAK};
//! Creates a gate with a gate name and type
Gate(std::string, Gatetype)
GateType gateType;
std::string name;

private:
}

class Circuit : public Gate
{
public:
Circuit(std::string);
//! Adds a gate at the end of the circuit
void addGate(GateApp);
//! inserts a gate at some index
void insertGate(GateApp,int);
void setInputs(std::vector<std::string>);

std::vector<std::string> inputs;
std::vector<GateApp> gates;

private:
}

class GateApp
{
public:
  //! contains the input ordering as indices into the inputs of the parent circuit
  std::vector<int> inputMap;
  std::shared_ptr<Gate> gate;

private:
} 
