#ifndef _UCOMPILER_H_
#define _UCOMPILER_H_
#include "circuit.h"
#include "gate.h"
#include "types.h"
#include "subcircuit.h"
#include "mat.h"
#include <memory>
#include <vector>
#include <complex>
#include "hprhelpers.h"
#include "rint.h"
#include <gmpxx.h>

/* High precision floating-point type. */
using uc_hpr_complex = hprHelpers::hpr_complex;

/* Symbolic comp. type. */
using uc_ring = ring_int<mpz_class>;

/* Gate library interface, templated over field of matrix for gate representation. */
template <typename T>
class GateLibrary
{
public:
    GateLibrary() {};
    virtual ~GateLibrary() {};
    virtual Matrix<T> getMatrixForGate(std::shared_ptr<Gate> g) = 0;
};

/* Floating point gate library. */
class CompGateLibrary : public GateLibrary<uc_hpr_complex>
{
public:
    CompGateLibrary();
    virtual ~CompGateLibrary();
    virtual Matrix<uc_hpr_complex> getMatrixForGate(std::shared_ptr<Gate> g);
};

/* Symbolic gate library. */
class SymGateLibrary : public GateLibrary<uc_ring>
{
public:
    SymGateLibrary();
    virtual ~SymGateLibrary();
    virtual Matrix<uc_ring> getMatrixForGate(std::shared_ptr<Gate> g);
};

/* Unitary compiler, templated over field of matrix for gate representation. */
template <typename T>
class UnitaryCompiler
{
public:
    UnitaryCompiler(std::shared_ptr<GateLibrary<T>> gl, const T & zero_, const T & one_);
    ~UnitaryCompiler();
    Matrix<T> getMatrixForCircuit(std::shared_ptr<Circuit> c);

private:
    std::shared_ptr<GateLibrary<T>> lib;
    T zero, one;
};

#endif
