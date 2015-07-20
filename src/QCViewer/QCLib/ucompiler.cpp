#include "ucompiler.h"
#include "circuit.h"
#include "gate.h"
#include "utility.h"
#include "types.h"
#include "subcircuit.h"
#include "circuitParser.h"
#include "gates/UGateLookup.h"
#include "mat.h"
#include "normalize.h"
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

using u32 = uint32_t;
using s64 = int64_t;

using comp_mat = Matrix<uc_hpr_complex>;
using sym_mat = Matrix<uc_ring>;

static const hprHelpers::hpr_real uc_hpr_epsilon = hprHelpers::hpr_real("1e-100");
static const float_type uc_lpr_epsilon = 1e-30;
static const uc_hpr_complex uc_hpr_zero = uc_hpr_complex(0,0);

// not currently used
//
/* static bool is_unitary(comp_mat & m)
{
    auto conj = [](const uc_hpr_complex & e) {
        return std::conj(e);
    };
    comp_mat I(m.numRows(),m.numCols(),uc_hpr_zero);
    I.identity(uc_hpr_complex(hprHelpers::one(),0));

    comp_mat mc = m.transpose().map_nonzero(conj);

    uc_hpr_complex a1n = (m * mc - I).frob_norm2(conj);
    uc_hpr_complex a2n = (mc * m - I).frob_norm2(conj);

    return (a1n.real() < uc_hpr_epsilon && a2n.real() < uc_hpr_epsilon);
} */

CompGateLibrary::CompGateLibrary() { }

CompGateLibrary::~CompGateLibrary() { }

Matrix<uc_hpr_complex> CompGateLibrary::getMatrixForGate(std::shared_ptr<Gate> g)
{
    u32 bu_dim = 1 << g->targets.size();

    comp_mat base_unitary(bu_dim,bu_dim,uc_hpr_zero);
    shared_ptr<UGate> ug = dynamic_pointer_cast<UGate>(g);
    shared_ptr<RGate> rg = dynamic_pointer_cast<RGate>(g);
    if(ug) {
        if(ug->getName() == "tof") { /* XXX special case - tof is not in the gate library */
            base_unitary.set(1,0,uc_hpr_complex(hprHelpers::one(),0));
            base_unitary.set(0,1,uc_hpr_complex(hprHelpers::one(),0));
        } else {
            gateMatrix m = UGateLookup(ug->getName());
            if(m.dim == 0) {
                throw std::runtime_error("Could not get unitary for UGate.");
            }
            assert(m.dim == bu_dim);
            for(u32 i = 0; i < m.dim; i++) {
                for(u32 j = 0; j < m.dim; j++) {
                    complex<float_type> ent = m.data[i*m.dim + j];
                    if(std::abs(ent) > uc_lpr_epsilon) {
                        float_type lpr_re = ent.real(), lpr_im = ent.imag();
                        base_unitary.set(i,j,uc_hpr_complex(lpr_re,lpr_im));
                    }
                }
            }
        }
    } else if(rg) {
        switch(rg->get_axis()) {
        case RGate::X: {
            float_type cosr = cos(rg->get_rotVal()/2.0), sinr = sin(rg->get_rotVal()/2.0);
            if(std::abs(cosr) > uc_lpr_epsilon) {
                base_unitary.set(0,0,uc_hpr_complex(cosr,0.0));
                base_unitary.set(1,1,uc_hpr_complex(cosr,0.0));
            }
            if(std::abs(sinr) > uc_lpr_epsilon) {
                base_unitary.set(1,0,uc_hpr_complex(0.0,-sinr));
                base_unitary.set(0,1,uc_hpr_complex(0.0,-sinr));
            }
            break;
        }
        case RGate::Y: {
            float_type cosr = cos(rg->get_rotVal()/2.0), sinr = sin(rg->get_rotVal()/2.0);
            if(std::abs(cosr) > uc_lpr_epsilon) {
                base_unitary.set(0,0,uc_hpr_complex(cosr,0.0));
                base_unitary.set(1,1,uc_hpr_complex(cosr,0.0));
            }
            if(std::abs(sinr) > uc_lpr_epsilon) {
                base_unitary.set(1,0,uc_hpr_complex(sinr,0.0));
                base_unitary.set(0,1,uc_hpr_complex(-sinr,0.0));
            }
            break;
        }
        case RGate::Z: {
            base_unitary.set(0,0,std::exp(uc_hpr_complex(0.0,-(rg->get_rotVal())/2.0)));
            base_unitary.set(1,1,std::exp(uc_hpr_complex(0.0,(rg->get_rotVal())/2.0)));
            break;
        }
        }
    } else {
        throw std::runtime_error("Couldn't get unitary for gate!");
    }
    return base_unitary;
}

template<typename T>
UnitaryCompiler<T>::UnitaryCompiler(std::shared_ptr<GateLibrary<T>> gl,
                                    const T & zero_, const T & one_)
    : lib(gl), zero(zero_), one(one_)
{ }

template<typename T>
UnitaryCompiler<T>::~UnitaryCompiler()
{ }

template<typename T>
Matrix<T>
UnitaryCompiler<T>::getMatrixForCircuit(shared_ptr<Circuit> c)
{
    shared_ptr<Circuit> c_n = CircuitNormalizer::normalize(c);
    if(c_n->numLines() >= 31) {
        throw std::runtime_error("UnitaryCompiler: Circuit has too many qubits.");
    }
    u32 dim = 1 << c_n->numLines();
    Matrix<T> accum(dim,dim,zero);
    accum.identity(one);

    for(s64 i = c_n->numGates()-1; i >= 0; i--) {
        shared_ptr<Gate> g = c_n->getGate(i);

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

        Matrix<T> top_i(top_dim,top_dim,zero);
        top_i.identity(one);
        Matrix<T> bot_i(bot_dim,bot_dim,zero);
        bot_i.identity(one);

        Matrix<T> base_unitary = lib->getMatrixForGate(g);

        Matrix<T> ctl_u(ctl_dim,ctl_dim,zero);
        ctl_u.identity(one);
        Matrix<T> ctl_pad(ctl_dim,bu_dim,zero);
        ctl_u.happend(ctl_pad).vappend(ctl_pad.transpose().happend(base_unitary));

        Matrix<T> final_u = ((top_i % ctl_u) % bot_i);

        accum *= final_u;
    }

    return accum;
}

template class UnitaryCompiler<uc_hpr_complex>;
template class UnitaryCompiler<uc_ring>;

