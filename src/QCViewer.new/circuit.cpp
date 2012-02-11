#include <algorithm>
#include "circuit.h"
#include "common.h"

/*
  Implementation for struct drawable_t
 */

drawable_t::drawable_t ()
{
    p_value_t p;
    p.b = false;
    set_param (PARALLEL_GUIDES, p);
    set_param (COMPRESSED, p);
    p.b = true;
    set_param (BREAKPOINTS, p);
    set_param (SELECTED, p);
    set_param (QUBIT_LABELS, p);
    set_param (COLUMN_CENTRED, p);
    p.f = 1.0;
    set_param (LR_PAD, p);
    p.f = 0.0;
    set_param (RIGHT_PAD, p);
}

drawable_t::~drawable_t ()
{
}

drawable_t::p_value_t drawable_t::get_param (param_t p) const
{
    std::map<param_t, p_value_t>::const_iterator it = params.find(p);
    assert (it != params.end ());
    return it->second;
}

void drawable_t::set_param (param_t p, p_value_t v)
{
    params[p] = v;
}

/*
 Implementation for struct sim_t
 */

sim_t::state_t sim_t::simulation_state () const
{
    return sim_state;
}

/*
  Implementation for struct circuit_t
 */

circuit_t::circuit_t ()
{
}

circuit_t::~circuit_t ()
{
    std::vector<circuit_column_t*>::iterator it;
    std::vector<qubit_t*>::iterator it2;

    for (it = columns.begin(); it != columns.end(); it++) delete *it;

    for (it2 = qubits.begin(); it2 != qubits.end(); it2++)
        if ( (*it2)->owner == this ) delete *it2;
}

void circuit_t::add_qubit (qubit_t* q, qubit_t* prev)
{
    if ( prev == NULL ) {
        qubits.push_back(q);
    }

    std::vector<qubit_t*>::iterator it =
        std::find(qubits.begin(), qubits.end(), prev);

    if ( it == qubits.end() )
        panic("Tried to add a qubit after a qubit which didn't exist.");

    qubits.insert(it + 1, q);
}

std::vector<const gate_t*> circuit_t::get_qubit_users (const qubit_t* q) const
{
    std::vector<const gate_t*> result;
    std::vector<circuit_column_t*>::const_iterator it;
    for (it = columns.begin(); it != columns.end(); it++) {
        std::vector<const gate_t*> subresult = (*it)->get_qubit_users(q);
        result.insert(result.end(), subresult.begin(), subresult.end());
    }
    return result;
}

void circuit_t::delete_qubit (qubit_t* q)
{
    assert (q->owner == this || q->owner == NULL);
    std::vector<qubit_t*>::iterator it =
        std::find(qubits.begin(), qubits.end(), q);

    if ( it == qubits.end() )
        panic("Tried to add a qubit after a qubit which didn't exist.");

    if ( q->owner == this ) delete q;
    qubits.erase (it);

    std::vector<circuit_column_t*>::iterator it2;
    for (it2 = columns.begin(); it2 != columns.end(); it2++) {
        (*it2)->delete_qubit (q);
        if ( (*it2)->empty() ) it2 = columns.erase (it2) - 1;
    }
}

void circuit_t::swap_qubits (const qubit_t* a, const qubit_t* b)
{
    assert (a->owner == this && b->owner == this);
    std::vector<qubit_t*>::iterator q1 =
        std::find(qubits.begin(), qubits.end(), a);

    std::vector<qubit_t*>::iterator q2 =
        std::find(qubits.begin(), qubits.end(), b);

    if ( q1 == qubits.end() || q2 == qubits.end() )
        panic("Tried to swap two qubits, one of which did not exist.");

    std::swap(*q1, *q2);

    std::vector<circuit_column_t*>::iterator it;
    for (it = columns.begin(); it != columns.end(); it++) {
        std::vector<circuit_column_t*> new_columns = (*it)->reorder_qubits(qubits);
        columns.insert (it, new_columns.begin(), new_columns.end());
        it = columns.begin(); // XXX: do this better?
    }
}

void circuit_t::append_gate (gate_t* g, bool force_column_break)
{
    if ( columns.size() == 0 ) {
        circuit_column_t* nc = new circuit_column_t();
        columns.push_back(nc);
    }
    if ( columns.back()->gate_fits(g, qubits)
            && !(force_column_break && !columns.back()->empty()) ) {
        columns.back()->add_gate(g);
    } else {
        circuit_column_t* nc = new circuit_column_t();
        columns.push_back(nc);
        columns.back()->add_gate(g);
    }
}

void circuit_t::draw (Cairo::Context cr) const
{

}

void circuit_t::set_param_recursive (param_t p, p_value_t v)
{
    set_param (p, v);
    std::vector<circuit_column_t*>::iterator it;
    for (it = columns.begin(); it != columns.end(); it++)
        (*it)->set_param_recursive (p, v);
}

void circuit_t::reset ()
{
    assert (sim_state != READY);
    std::vector<circuit_column_t*>::iterator it = columns.begin();
    for (; (*it)->simulation_state() != READY; it++)
        (*it)->reset();
}

bool circuit_t::step ()
{
    assert (sim_state != DONE);
    std::vector<circuit_column_t*>::iterator it = columns.begin();
    for (; (*it)->simulation_state() != DONE; it++);
    bool done = (*it)->step();
    if ( done && it + 1 == columns.end() ) sim_state = DONE;
    else                                   sim_state = ACTIVE;
    return (sim_state == DONE);
}

bool circuit_t::run ()
{
    assert (sim_state != DONE);
    std::vector<circuit_column_t*>::iterator it;
    sim_state = ACTIVE;
    for (it = columns.begin(); it != columns.end(); it++) {
        if ((*it)->simulation_state() == DONE) continue;
        bool done = (*it)->run();
        if (!done) return false;
    }
    sim_state = DONE;
    return true;
}

/*
 Implementation for struct circuit_column_t
 */

circuit_column_t::circuit_column_t ()
{
    sim_state = circuit_t::READY;
}

circuit_column_t::~circuit_column_t ()
{
    for (uint32_t i = 0; i < gates.size(); i++) delete gates[i];
}

std::vector<const gate_t*> circuit_column_t::get_qubit_users
(const qubit_t* q) const
{
    std::vector<const gate_t*> result;
    std::vector<gate_t*>::const_iterator it;
    for (it = gates.begin(); it != gates.end(); it++) {
        if ((*it)->touches_qubit(q)) result.push_back(*it);
    }
    return result;
}

void circuit_column_t::delete_qubit (qubit_t* pos)
{
    std::vector<gate_t*>::iterator it;
    for (it = gates.begin(); it != gates.end(); it++) {
        if ((*it)->touches_qubit(pos)) {
            delete *it;
            it = gates.erase(it) - 1;
        }
    }
}

std::vector<circuit_column_t*> circuit_column_t::reorder_qubits
(const std::vector<qubit_t*>& order)
{
    std::vector<circuit_column_t*> nc;
    // XXX
    // also, lol set packing
    return nc;
}

bool circuit_column_t::gate_fits
(const gate_t* g, const std::vector<qubit_t*>& order) const
{
    std::vector<qubit_t*> used;
    std::vector<gate_t*>::const_iterator gi;
    for (gi = gates.begin(); gi != gates.end(); gi++) {
        std::vector<qubit_t*> used_g = (*gi)->get_overlapped_qubits(order);
        std::vector<qubit_t*>::iterator it;
        for (it = used_g.begin(); it != used_g.end(); it++) {
            std::vector<qubit_t*>::iterator it = std::find(used.begin(), used.end(), *it);
            assert(it == used.end());
            used.push_back(*it);
        }
    }

    std::vector<qubit_t*> used_g = g->get_overlapped_qubits(order);
    std::vector<qubit_t*>::iterator it;
    for (it = used_g.begin(); it != used_g.end(); it++) {
        if ( std::find(used.begin(), used.end(), *it) != used.end() )
            return false;
    }
    return true;
}

void circuit_column_t::add_gate (gate_t* g)
{
    // sadly, can't do this as we don't know the
    // ordering... assert(gate_fits(g));

    gates.push_back(g);
}

bool circuit_column_t::empty () const
{
    return (0 == gates.size());
}

void circuit_column_t::reset ()
{
    assert (sim_state != circuit_t::READY);
    std::vector<gate_t*>::iterator it;
    for (it = gates.begin(); it != gates.end(); it++) {
        gate_t *g = *it;
        if ( g->simulation_state() == circuit_t::READY ) break;
        g->reset();
    }
    sim_state = circuit_t::READY;
}

bool circuit_column_t::step ()
{
    assert (sim_state != circuit_t::DONE);
    std::vector<gate_t*>::iterator it = gates.begin();
    for (; it != gates.end() && (*it)->simulation_state() == circuit_t::DONE; it++);
    assert (it != gates.end());
    bool done = (*it)->step();
    if ( done && it + 1 == gates.end() ) {
        sim_state = circuit_t::DONE;
        return true;
    }
    sim_state = circuit_t::ACTIVE;
    return false;
}

bool circuit_column_t::run ()
{
    assert(sim_state != circuit_t::DONE);
    std::vector<gate_t*>::iterator it;
    sim_state = circuit_t::ACTIVE;
    for (it = gates.begin(); it != gates.end(); it++) {
        gate_t* g;
        if ( g->simulation_state() == circuit_t::DONE ) continue;
        bool done = g->run();
        if ( !done ) return false;
    }
    sim_state = circuit_t::DONE;
    return true;
}

void circuit_column_t::draw (Cairo::Context cr) const
{

}

void circuit_column_t::set_param_recursive (param_t p, p_value_t v)
{
    set_param (p, v);
    std::vector<gate_t*>::iterator it;
    for (it = gates.begin(); it != gates.end(); it++)
        (*it)->set_param_recursive (p, v);
}
