#include "circuitParser.h"
#include "utility.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>

string getGateInfo(Circuit *circ)
{
  Gate *gate; //for current gate
  stringstream ret;
  ret << "BEGIN\n";
  for (unsigned int i = 0; i< circ->numGates(); i++)
    {
      gate = circ->getGate(i);
      if(gate->type == Gate::RGATE)
        {
          string symbol;
          if (((RGate*)gate)->get_axis()==RGate::X)
            {
              symbol = "X";
            }
          else if (((RGate*)gate)->get_axis()==RGate::Y)
            {
              symbol = "Y";
            }
          if (((RGate*)gate)->get_axis()==RGate::Z)
            {
              symbol = "Z";
            }
          ret << "R(" << symbol << ((RGate*)gate)->get_rotVal() << ")";
        }
      else
        {
          ret << gate->getName();
        }
      for (unsigned int j = 0; j < gate->controls.size() ; j++)
        {
          ret << " \"" << circ->getLine(gate->controls[j].wire)->lineName << "\"";
        }
      for (unsigned int j = 0; j < gate->targets.size() ; j++)
        {
          ret << " \"" << circ->getLine(gate->targets[j])->lineName << "\"";
        }
      ret << "\n";
    }
  ret << "END";
  return ret.str();
}


string getCircuitInfo(Circuit *circ)
{
  stringstream v,in,o,ol,c,ret;  //correspond to simlarly named sections in the file
  v << ".v" ;
  in << ".i";
  o << ".o";
  ol << ".ol";
  c << ".c";
  Line *line;  //for current line
  for (unsigned int i = 0; i< circ->numLines(); i++)
    {
      line = circ->getLine(i);
      v << " \"" << line->lineName << "\"";
      if (line->constant)
        {
          c << " " << line->initValue;
        }
      else
        {
          in << " \"" << line->lineName << "\"";
        }
      if (!line->garbage)
        {
          o << " \""<< line->lineName << "\"";
        }
      if (line->outLabel.compare("")!=0)
        {
          ol << " \""<< line->lineName << "\"";
        }
    }
  v << "\n";
  in << "\n";
  o << "\n";
  ol << "\n";
  c << "\n";
  ret << v.str() << in.str() << o.str() << ol.str() << c.str();
  return ret.str();
}

void saveCircuit(Circuit *circ, string filename)
{
  ofstream f;
  f.open (filename.c_str());
  string circInfo = getCircuitInfo(circ);
  string gateInfo = getGateInfo(circ);
  f << circInfo << gateInfo;
  f.close();
}
