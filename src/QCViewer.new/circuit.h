#ifndef CIRCUIT__INCLUDED
#define CIRCUIT__INCLUDED

#include <string>
#include <vector>
#include <map>
#include <cairomm/context.h>
#include "common.h"

struct qubit_t;
struct gate_t;
struct circuit_column_t;
struct circuit_t;
struct drawable_t;
struct sim_t;

// XXX: use std version
struct noncopyable {
  noncopyable() = default;
  noncopyable(noncopyable const &) = delete;
  noncopyable & operator=(noncopyable const &) = delete;
};

struct drawable_t : private noncopyable {
    drawable_t ();
    ~drawable_t ();

    union p_value_t {
        bool b;
        int32_t i;
        uint32_t ui;
        float f;
    };

    enum param_t { // Boolean drawing settings
                   PARALLEL_GUIDES,
                   BREAKPOINTS,
                   SELECTED,
                   QUBIT_LABELS,
                   COMPRESSED,

                   // Spacing
                   LR_PAD,  // balanced left/right padding
                   RIGHT_PAD, // extra right padding
                   COLUMN_CENTRED
                 };

    virtual void draw (Cairo::Context) const = 0;

    p_value_t get_param (param_t) const;
    void set_param (param_t, p_value_t);
    virtual void set_param_recursive (param_t, p_value_t) = 0;

  private:
    std::map<param_t, p_value_t> params;
};

struct sim_t {
    enum state_t {
      READY,  // No simulation has taken place
      ACTIVE, // Some simulation has taken place
      DONE    // Simulation is finished
    };

    sim_t () : sim_state (READY) {}

    virtual void reset () = 0;
    virtual bool step () = 0;
    virtual bool run () = 0;
    state_t simulation_state () const;

  protected:
    state_t sim_state;
};


struct qubit_t : private noncopyable {
    qubit_t (std::string l, circuit_t* c) : label(l), owner(c) {}

    std::string label;
    circuit_t* const owner; // Note: will assume that we can't change this.
};

struct circuit_t : public drawable_t, public sim_t {
    circuit_t ();
    ~circuit_t ();

    /* Qubits */
    void add_qubit (qubit_t*, qubit_t*);
    // Note: assuming this will be used by the UI to make gates redish
    //       when the user wants to delete a qubit they touch. In this
    //       case, the gate_t's should not be modified. Change later
    //       if appropriate.
    std::vector<const gate_t*> get_qubit_users (const qubit_t*) const;
    void delete_qubit (qubit_t*);
    void swap_qubits (const qubit_t*, const qubit_t*);

    /* Gates */
    void append_gate (gate_t*, bool);

    /* Drawing */
    void draw (Cairo::Context) const;
    void set_param_recursive (param_t, p_value_t);

    /* Simulation */
    void reset ();
    bool step ();
    bool run ();

 private:
   std::vector<qubit_t*> qubits;
   std::vector<circuit_column_t*> columns;
};

struct circuit_column_t : public drawable_t, public sim_t {
  public:
    circuit_column_t ();
    ~circuit_column_t ();

    /* Qubits */
    std::vector<const gate_t*> get_qubit_users (const qubit_t*) const;
    void delete_qubit (qubit_t*);
    // NOTE: would be nice if we could do const qubit_t* in the vector
    //       ----> figure this out.
    std::vector<circuit_column_t*> reorder_qubits
      (const std::vector<qubit_t*>&);

    /* Gates */
    bool gate_fits (const gate_t*, const std::vector<qubit_t*>&) const;
    void add_gate (gate_t*);
    bool empty () const;

    /* Simulation */
    void reset ();
    bool step ();
    bool run ();

    /* Drawing */
    void draw (Cairo::Context) const;
    void set_param_recursive (param_t, p_value_t);

  private:
    std::vector<gate_t*> gates;
};

struct gate_t : public drawable_t, public sim_t {
    enum gate_type_t { UNITARY, SUBCIRCUIT, DUMMY };
    gate_t (gate_type_t g) : breakpoint_start (false),
                             breakpoint_end (false),
                             type(g) {}

    gate_type_t gate_type () const { return type; }

    /* Qubits */
    struct qubit_usage_t {
      enum { POSITIVE_CTRL, NEGATIVE_CTRL, TARGET } type;
      qubit_t* q;
    };

    virtual void delete_qubit (qubit_t*) = 0;
    virtual bool touches_qubit (const qubit_t*) const = 0;
    virtual void reorder_qubits (const std::vector<qubit_t*>&) = 0;
    virtual std::vector<qubit_t*>& get_overlapped_qubits
      (const std::vector<qubit_t*>&) const = 0;

    /* Gates */
    virtual void add_gate (gate_t*) = 0;

    /* Drawing*/
    virtual void set_param_recursive (param_t, p_value_t) = 0;

    /* Public Data */
    bool breakpoint_start;
    bool breakpoint_end;
    uint32_t loop_count;

  private:
    const gate_type_t type;
    std::vector<qubit_usage_t> qubits;
};

#endif
