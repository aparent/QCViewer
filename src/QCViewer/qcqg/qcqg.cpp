/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

See also 'ADDITIONAL TERMS' at the end of the included LICENSE file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QCViewer is a trademark of the of the The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

Authors: Alex Parent, Jacob Parker
---------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <memory>
#include <set>
#include <regex>
#include <algorithm>
#include "circuit.h"
#include "subcircuit.h"
#include "gate.h"
#include "circuitParser.h"

class indenter {
public:
  indenter & operator()(int x) {
    indent += x;
    return *this;
  }
  static int indent;
};

std::ostream & operator<<(std::ostream & os, indenter & ind) {
  for(int i=0;i<ind.indent;i++) {
    os << "\t";
  }
  return os;
}

indenter ind;
int indenter::indent = 0;

std::string varname(std::shared_ptr<Circuit> circ, int index) {
/*  std::set<std::string> names;
  std::regex valid_check("[a-z]*");
  for(unsigned int i = 0; i < circ->numLines(); ++i) {
    std::string line = circ->getLine(i).getInputLabel();
    if(!std::regex_match(line, valid_check) ||
        (names.find(line) != names.end()))
    {*/
      std::stringstream ss;
      ss << "a" << index;
      return ss.str();
    /*}
  }
  return circ->getLine(index).getInputLabel();*/
}

std::string gatename(std::shared_ptr<Gate> g) {
  std::shared_ptr<Subcircuit> subcirc = std::dynamic_pointer_cast<Subcircuit>(g);
  if(subcirc) {
    return subcirc->getName();
  } else {
    std::string res = g->getName();
    for(std::string::iterator p = res.begin(); p != res.end(); ++p) {
      *p = toupper(*p);
    }
    if(res == "X") {
      res = "pauliX";
    } else if(res == "Y") {
      res = "pauliY";
    } else if(res == "Z") {
      res = "pauliZ";
    }
    return res;
  }
}

void writeLooped(std::function<void()> gen, unsigned int times, std::ostream & out) {
  if(times > 1) {
    out << ind(1) << "<statement>\n";
    out << ind(1) << "<repeat>\n";
    out << ind(1) << "<variable><name>FREE</name></variable>\n";
    out << ind(0) << "<times><integer>" << times << "</integer></times>\n";
    out << ind(0) << "<block>\n";
  }
  gen();
  if(times > 1) {
    out << ind(0) << "</block>\n";
    out << ind(-1) << "</repeat>\n";
    out << ind(-1) << "</statement>\n";
    ind(-1);
  }
}

void writeControlled(std::shared_ptr<Circuit> circ, std::function<void()> gen,
                     std::vector<Control> & c, std::ostream & out) {
  std::vector<Control> pos, neg;
  std::copy_if(c.begin(), c.end(), std::back_inserter(pos), [](Control & c){ return !c.polarity; });
  std::copy_if(c.begin(), c.end(), std::back_inserter(neg), [](Control & c){ return c.polarity; });
  if(!pos.empty()) {
    out << ind(1) << "<statement>\n";
    out << ind(1) << "<control>\n";
    out << ind(1) << "<quantum>\n";
    ind(1);
    for(auto & ctl : pos) {
      out << ind(0) << "<qurange><variable><name>" << varname(circ, ctl.wire)
          << "</name></variable></qurange>\n";
    }
    out << ind(-1) << "</quantum>\n";
    out << ind(0) << "<block>\n";
  }
  if(!neg.empty()) {
    out << ind(1) << "<statement>\n";
    out << ind(1) << "<controlnot>\n";
    out << ind(1) << "<quantum>\n";
    ind(1);
    for(auto & ctl : neg) {
      out << ind(0) << "<qurange><variable><name>" << varname(circ, ctl.wire)
          << "</name></variable></qurange>\n";
    }
    out << ind(-1) << "</quantum>\n";
    out << ind(0) << "<block>\n";
  }
  gen();
  if(!neg.empty()) {
    out << ind(0) << "</block>\n";
    out << ind(-1) << "</controlnot>\n";
    out << ind(-1) << "</statement>\n";
    ind(-1);
  }
  if(!pos.empty()) {
    out << ind(0) << "</block>\n";
    out << ind(-1) << "</control>\n";
    out << ind(-1) << "</statement>\n";
    ind(-1);
  }
}

void writeGate(std::shared_ptr<Circuit> circ, std::shared_ptr<Gate> g, std::ostream & out) {
  auto gen = [&]() {
    auto genloop = [&] {
      std::shared_ptr<Subcircuit> subcirc = std::dynamic_pointer_cast<Subcircuit>(g);
      out << ind(1) << "<statement>\n";
      if(subcirc) {
        out << ind(1) << "<call>\n";
      } else {
        out << ind(1) << "<gate>\n";
      }
      out << ind(1) << "<name>" << gatename(g) << "</name>\n";
      out << ind(0) << "<quantum>\n";
      ind(1);
      for(auto t : g->targets) {
          out << ind(0) << "<qurange><variable><name>" << varname(circ, t)
              << "</name></variable></qurange>\n";
      }
      out << ind(-1) << "</quantum>\n";
      if(subcirc) {
        out << ind(-1) << "</call>\n";
      } else {
        out << ind(-1) << "</gate>\n";
      }
      out << ind(-1) << "</statement>\n";
      ind(-1);
    };
    writeLooped(genloop, g->getLoopCount(), out);
  };
  writeControlled(circ, gen, g->controls, out);
}

void writeGateSeq(std::shared_ptr<Circuit> circ, std::ostream & out) {
  for(unsigned int i = 0; i < circ->numGates(); ++i) {
    writeGate(circ, circ->getGate(i), out);
  }
}

void writeSubcircuit(std::string name, std::shared_ptr<Circuit> circ, std::ostream & out) {
  out << ind(1) << "<statement>\n";
  out << ind(1) << "<procedure>\n";
  out << ind(1) << "<name>" << name << "</name>\n";
  out << ind(0) << "<quantum>\n";
  ind(1);
  for(unsigned int i = 0; i < circ->numLines(); ++i) {
    out << ind(0) << "<variable><name>" << varname(circ, i)
        << "</name></variable>\n";
  }
  out << ind(-1) << "</quantum>\n";
  out << ind(0) << "<block>\n";
  writeGateSeq(circ, out);
  out << ind(0) << "</block>\n";
  out << ind(-1) << "</procedure>\n";
  out << ind(-1) << "</statement>\n";
  ind(-1);
}

void writeLineAlloc(std::shared_ptr<Circuit> circ, std::ostream & out) {
  for(unsigned int i = 0; i < circ->numLines(); ++i) {
    out << ind(1) << "<statement>\n";
    out << ind(1) << "<assignment>\n";
    out << ind(1) << "<variable><name>" << varname(circ, i)
        << "</name></variable>\n";
    out << ind(0) << "<value>\n";
    out << ind(1) << "<qurange>\n";
    out << ind(1) << "<wire>\n";
    out << ind(1) << "<quantity>\n";
    out << ind(1) << "<integer>1</integer>\n";
    out << ind(-1) << "</quantity>\n";
    out << ind(-1) << "</wire>\n";
    out << ind(-1) << "</qurange>\n";
    out << ind(-1) << "</value>\n";
    out << ind(-1) << "</assignment>\n";
    out << ind(-1) << "</statement>\n";
    ind(-1);
  }
}

void convert(std::shared_ptr<Circuit> circ, std::ostream & out) {
  /* Write header */
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<circuit xmlns=\"http://torque.bbn.com/ns/QuIGL\"\n"
      << "         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
      << "         xsi:schemaLocation=\"http://torque.bbn.com/ns/QuIGL ../xsd/QuIGL.xsd\">\n";

  /* Write all subcircuits */
  for(auto sub : circ->subcircuits) {
    writeSubcircuit(sub.first, sub.second, out);
  }

  /* Write line allocation */
  writeLineAlloc(circ, out);

  /* Write gates */
  writeGateSeq(circ, out);

  /* Write footer */
  out << "</circuit>\n";
}

int main(int argc, char * argv[]) {
  std::shared_ptr<Circuit> circ;

  std::string usage = "Usage: qcqg [circuit.qc] [circuit.xml]\n";
  if(argc != 3) {
    std::cerr << usage;
    return 1;
  }

  {
    std::string qctext;
    std::vector<std::string> error_log;

    if(!strcmp(argv[1], "-")) {
      qctext.assign((std::istreambuf_iterator<char>(std::cin)),
                     std::istreambuf_iterator<char>());
    } else {
      std::ifstream in(argv[1]);
      if(!in.good()) {
        std::cerr << "Could not open file '" << argv[1] << "'.\n";
        return 1;
      }
      qctext.assign((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
    }

    circ = parseCircuit(qctext, error_log);
    if(!circ || !error_log.empty()) {
      std::cout << "Errors were encountered reading '" << argv[1] << "':\n";
      for(std::vector<std::string>::iterator p = error_log.begin(); p != error_log.end(); ++p) {
        std::cerr << "\t" << *p << "\n";
      }
      return 1;
    }
  }

  {
    try {
      if(!strcmp(argv[2], "-")) {
        convert(circ, std::cout);
      } else {
        std::ofstream out(argv[2]);
        if(!out.good()) {
          std::cerr << "Could not open file '" << argv[2] << "'.\n";
          return 1;
        }
        convert(circ, out);
      }
    } catch(std::regex_error & e) {
      std::cerr << "Regex error: " << e.what() << "\n";
    } catch(std::exception & e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return 1;
    }
  }

  return 0;
}
