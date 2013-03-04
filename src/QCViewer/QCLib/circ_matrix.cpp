#include "circuit.h"
#include "gate.h"
#include "utility.h"
#include "types.h"
#include "subcircuit.h"
#include "circuitParser.h"
#include "gates/UGateLookup.h"
#include "mat.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <climits>
#include <cassert>
#include <iostream>
#include <complex>

using namespace std;

typedef uint32_t u32;
typedef int32_t s32;
typedef int64_t s64;

shared_ptr<Circuit> circ_unormalize(shared_ptr<Circuit> c);
shared_ptr<Circuit> circ_flatten(shared_ptr<Circuit> in);

using comp_mat = Matrix<complex<float_type>>;
#define CPX_EPSILON 1e-6

float_type trace_norm(const comp_mat & m) {
  comp_mat mm = m, mmc = m;
  float_type res = 0.0;
  mmc = mmc.transpose().map_nonzero([](const complex<float_type> & e) { return std::conj(e); });
  mm = mmc * mm;

  for(u32 i = 0; i < mm.numRows();i++) {
    res += mm.get(i,i).real();
  }
  return res;
}

bool is_unitary(comp_mat & m) {
  comp_mat I(m.numRows(),m.numCols(),complex<float_type>(0.0,0.0));
  I.identity(complex<float_type>(1.0,0.0));
  comp_mat mc = m;
  mc = mc.transpose().map_nonzero([](const complex<float_type> & e) { return std::conj(e); });

  float_type a1n = trace_norm(m * mc - I);
  float_type a2n = trace_norm(mc * m - I);

  return (a1n < CPX_EPSILON && a2n < CPX_EPSILON);
}

comp_mat comp_matrix_for_circuit(shared_ptr<Circuit> c)
{
  shared_ptr<Circuit> c_n = circ_unormalize(circ_flatten(c));
  if(c_n->numLines() >= 31) {
    throw std::runtime_error("comp_matrix_for_circuit: Circuit has too many qubits.");
  }
  u32 dim = 1 << c_n->numLines();
  comp_mat accum(dim,dim,0);
  accum.identity(1);

  for(s64 i = c_n->numGates()-1;i >= 0;i--)
  {
    shared_ptr<Gate> g = c_n->getGate(i);
    for(auto i : g->targets) {
      cout << " " << i;
    }
    for(auto & p : g->controls) {
      cout << " " << p.wire << "(" << p.polarity << ")";
    }
    cout << endl;

    u32 g_start = UINT_MAX, g_end, top_dim, bot_dim, bu_dim, ctl_dim;
    if(g->controls.size() > 0) {
      for(auto & con : g->controls) {
        if(con.wire < g_start) {
          g_start = con.wire;
        }
      }
    } else {
      for(auto targ : g->targets) {
        if(targ < g_start) {
          g_start = targ;
        }
      }
    }
    g_end = g_start + g->targets.size() + g->controls.size() - 1;
    top_dim = 1 << g_start;
    bot_dim = 1 << (c_n->numLines() - g_end - 1);
    bu_dim = 1 << g->targets.size();
    ctl_dim = (1 << (g->controls.size() + 1)) - 2;

    comp_mat top_i(top_dim,top_dim,0);
    top_i.identity(1);
    comp_mat bot_i(bot_dim,bot_dim,0);
    bot_i.identity(1);

    comp_mat base_unitary(bu_dim,bu_dim,0);
    shared_ptr<UGate> ug = dynamic_pointer_cast<UGate>(g);
    shared_ptr<RGate> rg = dynamic_pointer_cast<RGate>(g);
    if(ug) {
      if(ug->getName() == "tof") { /* XXX special case - tof is not in the gate library */
        base_unitary.set(1,0,complex<float_type>(1.0,0.0));
        base_unitary.set(0,1,complex<float_type>(1.0,0.0));
      } else {
        gateMatrix m = UGateLookup(ug->getName());
        if(m.dim == 0) {
          throw std::runtime_error("Could not get unitary for UGate.");
        }
        assert(m.dim == bu_dim);
        for(u32 i = 0;i < m.dim;i++) {
          for(u32 j = 0;j < m.dim;j++) {
            complex<float_type> ent = m.data[i*m.dim + j];
            if(std::abs(ent) > CPX_EPSILON) {
              base_unitary.set(i,j,ent);
            }
          }
        }
      }
    } else if(rg) {
      switch(rg->get_axis()) {
        case RGate::X:
        {
          float_type cosr = cos(rg->get_rotVal()/2.0), sinr = sin(rg->get_rotVal()/2.0);
          if(std::abs(cosr) > CPX_EPSILON) {
            base_unitary.set(0,0,complex<float_type>(cosr,0.0));
            base_unitary.set(1,1,complex<float_type>(cosr,0.0));
          }
          if(std::abs(sinr) > CPX_EPSILON) {
            base_unitary.set(1,0,complex<float_type>(0.0,-sinr));
            base_unitary.set(0,1,complex<float_type>(0.0,-sinr));
          }
          break;
        }
        case RGate::Y:
        {
          float_type cosr = cos(rg->get_rotVal()/2.0), sinr = sin(rg->get_rotVal()/2.0);
          if(std::abs(cosr) > CPX_EPSILON) {
            base_unitary.set(0,0,complex<float_type>(cosr,0.0));
            base_unitary.set(1,1,complex<float_type>(cosr,0.0));
          }
          if(std::abs(sinr) > CPX_EPSILON) {
            base_unitary.set(1,0,complex<float_type>(sinr,0.0));
            base_unitary.set(0,1,complex<float_type>(-sinr,0.0));
          }
          break;
        }
        case RGate::Z:
        {
          base_unitary.set(0,0,std::exp(complex<float_type>(0.0,-(rg->get_rotVal())/2.0)));
          base_unitary.set(1,1,std::exp(complex<float_type>(0.0,(rg->get_rotVal())/2.0)));
          break;
        }
      }
    } else {
      throw std::runtime_error("Couldn't get unitary for gate!");
    }

    comp_mat ctl_u(ctl_dim,ctl_dim,complex<float_type>(0.0,0.0));
    ctl_u.identity(complex<float_type>(1.0,0.0));
    comp_mat ctl_pad(ctl_dim,bu_dim,complex<float_type>(0.0,0.0));
    ctl_u.happend(ctl_pad).vappend(ctl_pad.transpose().happend(base_unitary));
    //assert(ctl_u.numRows() == ctl_u.numCols());
    //assert(is_unitary(ctl_u));

    comp_mat final_u = ((top_i % ctl_u) % bot_i);
    //assert(final_u.numRows() == final_u.numCols() && final_u.numCols() == dim);
    //assert(is_unitary(final_u));

    accum *= final_u;
  }

  assert(is_unitary(accum));
  return accum;
}

/*
int main(int argc, char ** argv) {
  UGateSetup();
  vector<string> error_log;
  shared_ptr<Circuit> c = parseCircuit(argv[1],error_log);
  if(!c) {
    cout << "Could not load circuit:\n";
    for(auto & s : error_log) {
      cout << s;
    }
  }

  comp_mat m = comp_matrix_for_circuit(c);
  cout << m.toString();
  return 0;
}
*/
