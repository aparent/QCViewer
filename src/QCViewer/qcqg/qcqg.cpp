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

Authors: Alex Parent, Jacob Parker, Marc Burns
---------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <memory>
#include <set>
#include <deque>
#include <algorithm>
#include <numeric>
#include <exception>
#include "circuit.h"
#include "subcircuit.h"
#include "gate.h"
#include "circuitParser.h"

class linemapper {
public:
  linemapper(std::shared_ptr<Circuit> c) : circ(c) {
    std::set<std::string> names;
    for(unsigned int i = 0; i < circ->numLines(); ++i) {
      std::string line = circ->getLine(i).getInputLabel();
      if(!valid_varname(line) ||
         (names.find(line) != names.end()))
      {
        use_lines = false;
        return;
      }
      names.insert(line);
    }
    use_lines = true;
  }
  bool valid_varname(std::string n) {
    return ((n.length() > 0) &&
            (((n[0] >= 'a') && (n[0] <= 'z')) ||
             ((n[0] >= 'A') && (n[0] <= 'Z'))) &&
            std::accumulate(n.begin()+1, n.end(), true,
                            [](const bool & v, const char & c) {
                              return v &&
                                     (((c >= 'a') && (c <= 'z')) ||
                                      ((c >= 'A') && (c <= 'Z')) ||
                                      ((c >= '0') && (c <= '9')));
                            }));
  }
  std::string map(int index) {
    std::string res;
    if(use_lines) {
      res = circ->getLine(index).getInputLabel();
      if(!res.compare(0,4,"FREE")) {
        res += "_";
      }
    } else {
      std::stringstream ss;
      ss << "a" << index;
      res = ss.str();
    }
    return res;
  }
private:
  std::shared_ptr<Circuit> circ;
  bool use_lines;
};

class indenter {
public:
  indenter() : indent(0) {};
  indenter & operator()(int x) {
    indent += x;
    return *this;
  }
  int indent;
private:
  indenter(const indenter & other) { (void)other; }
};

struct writectx {
  writectx(std::ostream & os) : out(os) {};
  void pushmap(std::shared_ptr<Circuit> c) {
    maps.push_front(linemapper(c));
  }
  void popmap() {
    maps.pop_front();
  }
  std::string map(int index) {
    return maps.front().map(index);
  }

  std::ostream & out;
  indenter ind;
  std::deque<linemapper> maps;
};

std::ostream & operator<<(std::ostream & os, indenter & ind) {
  for(int i=0;i<ind.indent;i++) {
    os << "\t";
  }
  return os;
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

void writeLooped(std::function<void()> gen, unsigned int times, writectx & w) {
  if(times > 1) {
    w.out << w.ind(1) << "<statement>\n";
    w.out << w.ind(1) << "<repeat>\n";
    w.out << w.ind(1) << "<variable><name>FREE</name></variable>\n";
    w.out << w.ind(0) << "<times><integer>" << times << "</integer></times>\n";
    w.out << w.ind(0) << "<block>\n";
  }
  gen();
  if(times > 1) {
    w.out << w.ind(0) << "</block>\n";
    w.out << w.ind(-1) << "</repeat>\n";
    w.out << w.ind(-1) << "</statement>\n";
    w.ind(-1);
  }
}

void writeControlled(std::function<void()> gen, std::vector<Control> & c, writectx & w) {
  std::vector<Control> pos, neg;
  std::copy_if(c.begin(), c.end(), std::back_inserter(pos), [](Control & c){ return !c.polarity; });
  std::copy_if(c.begin(), c.end(), std::back_inserter(neg), [](Control & c){ return c.polarity; });
  if(!pos.empty()) {
    w.out << w.ind(1) << "<statement>\n";
    w.out << w.ind(1) << "<control>\n";
    w.out << w.ind(1) << "<quantum>\n";
    w.ind(1);
    for(auto & ctl : pos) {
      w.out << w.ind(0) << "<qurange><variable><name>" << w.map(ctl.wire)
            << "</name></variable></qurange>\n";
    }
    w.out << w.ind(-1) << "</quantum>\n";
    w.out << w.ind(0) << "<block>\n";
  }
  if(!neg.empty()) {
    w.out << w.ind(1) << "<statement>\n";
    w.out << w.ind(1) << "<controlnot>\n";
    w.out << w.ind(1) << "<quantum>\n";
    w.ind(1);
    for(auto & ctl : neg) {
      w.out << w.ind(0) << "<qurange><variable><name>" << w.map(ctl.wire)
            << "</name></variable></qurange>\n";
    }
    w.out << w.ind(-1) << "</quantum>\n";
    w.out << w.ind(0) << "<block>\n";
  }
  gen();
  if(!neg.empty()) {
    w.out << w.ind(0) << "</block>\n";
    w.out << w.ind(-1) << "</controlnot>\n";
    w.out << w.ind(-1) << "</statement>\n";
    w.ind(-1);
  }
  if(!pos.empty()) {
    w.out << w.ind(0) << "</block>\n";
    w.out << w.ind(-1) << "</control>\n";
    w.out << w.ind(-1) << "</statement>\n";
    w.ind(-1);
  }
}

void writeGate(std::shared_ptr<Gate> g, writectx & w) {
  auto gen = [&]() {
    auto genloop = [&] {
      std::shared_ptr<Subcircuit> subcirc = std::dynamic_pointer_cast<Subcircuit>(g);
      w.out << w.ind(1) << "<statement>\n";
      if(subcirc) {
        w.out << w.ind(1) << "<call>\n";
      } else {
        w.out << w.ind(1) << "<gate>\n";
      }
      w.out << w.ind(1) << "<name>" << gatename(g) << "</name>\n";
      w.out << w.ind(0) << "<quantum>\n";
      w.ind(1);
      for(auto t : g->targets) {
          w.out << w.ind(0) << "<qurange><variable><name>" << w.map(t)
                << "</name></variable></qurange>\n";
      }
      w.out << w.ind(-1) << "</quantum>\n";
      if(subcirc) {
        w.out << w.ind(-1) << "</call>\n";
      } else {
        w.out << w.ind(-1) << "</gate>\n";
      }
      w.out << w.ind(-1) << "</statement>\n";
      w.ind(-1);
    };
    writeLooped(genloop, g->getLoopCount(), w);
  };
  writeControlled(gen, g->controls, w);
}

void writeGateSeq(std::shared_ptr<Circuit> circ, writectx & w) {
  for(unsigned int i = 0; i < circ->numGates(); ++i) {
    writeGate(circ->getGate(i), w);
  }
}

void writeSubcircuit(std::string name, std::shared_ptr<Circuit> circ, writectx & w) {
  w.pushmap(circ);
  w.out << w.ind(1) << "<statement>\n";
  w.out << w.ind(1) << "<procedure>\n";
  w.out << w.ind(1) << "<name>" << name << "</name>\n";
  w.out << w.ind(0) << "<quantum>\n";
  w.ind(1);
  for(unsigned int i = 0; i < circ->numLines(); ++i) {
    w.out << w.ind(0) << "<variable><name>" << w.map(i)
          << "</name></variable>\n";
  }
  w.out << w.ind(-1) << "</quantum>\n";
  w.out << w.ind(0) << "<block>\n";
  writeGateSeq(circ, w);
  w.out << w.ind(0) << "</block>\n";
  w.out << w.ind(-1) << "</procedure>\n";
  w.out << w.ind(-1) << "</statement>\n";
  w.ind(-1);
  w.popmap();
}

void writeLineAlloc(std::shared_ptr<Circuit> circ, writectx & w) {
  for(unsigned int i = 0; i < circ->numLines(); ++i) {
    w.out << w.ind(1) << "<statement>\n";
    w.out << w.ind(1) << "<assignment>\n";
    w.out << w.ind(1) << "<variable><name>" << w.map(i)
          << "</name></variable>\n";
    w.out << w.ind(0) << "<value>\n";
    w.out << w.ind(1) << "<qurange>\n";
    w.out << w.ind(1) << "<wire>\n";
    w.out << w.ind(1) << "<quantity>\n";
    w.out << w.ind(1) << "<integer>1</integer>\n";
    w.out << w.ind(-1) << "</quantity>\n";
    w.out << w.ind(-1) << "</wire>\n";
    w.out << w.ind(-1) << "</qurange>\n";
    w.out << w.ind(-1) << "</value>\n";
    w.out << w.ind(-1) << "</assignment>\n";
    w.out << w.ind(-1) << "</statement>\n";
    w.ind(-1);
  }
}

void convert(std::shared_ptr<Circuit> circ, std::ostream & out) {
  /* Write header */
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<circuit xmlns=\"http://torque.bbn.com/ns/QuIGL\"\n"
      << "         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
      << "         xsi:schemaLocation=\"http://torque.bbn.com/ns/QuIGL ../xsd/QuIGL.xsd\">\n";

  writectx w(out);
  w.pushmap(circ);

  /* Write all subcircuits */
  for(auto sub : circ->subcircuits) {
    writeSubcircuit(sub.first, sub.second, w);
  }

  /* Write line allocation */
  writeLineAlloc(circ, w);

  /* Write gates */
  writeGateSeq(circ, w);

  /* Write footer */
  out << "</circuit>\n";
}

int main(int argc, char * argv[]) {
  std::shared_ptr<Circuit> circ;

  std::string usage = "Usage: qc2quigl [circuit.qc] [circuit.xml]\n";
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
    } catch(std::exception & e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return 1;
    }
  }

  return 0;
}
