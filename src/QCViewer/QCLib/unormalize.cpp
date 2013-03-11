#include "circuit.h"
#include "gate.h"
#include "utility.h"
#include "types.h"
#include "subcircuit.h"
#include "circuitParser.h"
#include "gates/UGateLookup.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <climits>
#include <cassert>
#include <iostream>

using namespace std;

typedef uint32_t u32;
typedef int32_t s32;
typedef int64_t s64;
using swaplist = vector<pair<u32, u32>>;

static void subcirc_flatten(shared_ptr<Subcircuit> sub, shared_ptr<Circuit> accum)
{
  for(u32 t=0;t<sub->getLoopCount();t++)
  {
    for(u32 i=0;i<sub->numGates();i++)
    {
      shared_ptr<Gate> new_gate = sub->getGate(i)->clone();
      shared_ptr<Subcircuit> new_sub = dynamic_pointer_cast<Subcircuit>(new_gate);
      if(new_sub) {
        subcirc_flatten(new_sub, accum);
      } else {
        accum->addGate(new_gate);
      }
    }
  }
}

shared_ptr<Circuit> circ_flatten(shared_ptr<Circuit> in)
{
  shared_ptr<Circuit> accum = shared_ptr<Circuit>(new Circuit());
  accum->setName(in->getName());
  for(u32 i=0;i<in->numLines();i++) {
    accum->addLine("");
    accum->getLineModify(i) = in->getLine(i);
  }
  for(u32 i=0;i<in->numGates();i++)
  {
    shared_ptr<Gate> new_gate = in->getGate(i)->clone();
    shared_ptr<Subcircuit> sub = dynamic_pointer_cast<Subcircuit>(new_gate);
    if(sub) {
      subcirc_flatten(sub, accum);
    } else {
      accum->addGate(new_gate);
    }
  }
  return accum;
}

static swaplist remove_id_swaps(const swaplist & swaps)
{
  swaplist out;
  pair<u32, u32> prev (0,0);
  for(auto & p : swaps)
  {
    if(p.first != p.second &&
        ((p.first != prev.first || p.second != prev.second) &&
         (p.second != prev.first || p.first != prev.second)))
    {
      out.push_back(p);
      prev = p;
    }
  }
  return out;
}

static swaplist unit_swaps(const swaplist & swaps)
{
  swaplist out;
  for(auto & p : swaps)
  {
    u32 start, end;
    if(p.first < p.second) {
      start = p.first;
      end = p.second;
    } else {
      start = p.second;
      end = p.first;
    }
    for(u32 j=start;j<end;j++)
    {
      out.push_back(pair<u32,u32>(j,j+1));
    }
    for(s64 j=end-1;j>start;j--)
    {
      out.push_back(pair<u32,u32>((u32)j,(u32)(j-1)));
    }
  }
  return out;
}

static void swaps_to_circ(shared_ptr<Circuit> c, swaplist & swaps)
{
  for(auto & p : swaps)
  {
    shared_ptr<Gate> g = shared_ptr<Gate>(new UGate("F"));
    g->drawType = Gate::FRED;
    g->targets.push_back(p.first);
    g->targets.push_back(p.second);
    c->addGate(g);
  }
}

static void nots_to_circ(shared_ptr<Circuit> c, vector<u32> nots)
{
  for(auto & p : nots)
  {
    shared_ptr<Gate> g = shared_ptr<Gate>(new UGate("NOT"));
    g->drawType = Gate::NOT;
    g->targets.push_back(p);
    c->addGate(g);
  }
}

shared_ptr<Circuit> circ_unormalize(shared_ptr<Circuit> c)
{
  shared_ptr<Circuit> out = shared_ptr<Circuit>(new Circuit());
  out->setName(c->getName());
  for(u32 i=0;i<c->numLines();i++) {
    out->addLine("");
    out->getLineModify(i) = c->getLine(i);
  }

  for(u32 i=0;i<c->numGates();i++)
  {
    shared_ptr<Gate> new_gate = c->getGate(i)->clone();
    if(new_gate->targets.empty()) {
      throw std::runtime_error("unormalize: Gate has no targets.");
    }

    swaplist swaps;
    u32 min_target = UINT_MAX;
    for(auto & p : new_gate->targets)
    {
      if(p < min_target) {
        min_target = p;
      }
    }
    for(u32 j = 0; j < new_gate->targets.size(); j++)
    {
      swaps.push_back(pair<u32,u32>(new_gate->targets.at(j), min_target+j));
      new_gate->targets.at(j) = min_target + j;
    }

    s64 del_demote = new_gate->controls.size() - min_target;
    vector<u32> linemap;
    linemap.reserve(c->numLines());
    for(u32 j=0; j < c->numLines(); j++)
    {
      linemap.push_back(j);
    }
    if(del_demote > 0) {
      assert(min_target + del_demote + new_gate->targets.size() <= c->numLines());
      for(s64 j=new_gate->targets.size() - 1;j >= 0;j--)
      {
        swaps.push_back(pair<u32,u32>((u32)(min_target+j), (u32)(min_target+j+del_demote)));
        u32 tmp = linemap[min_target+j];
        linemap[min_target+j] = linemap[min_target+j+del_demote];
        linemap[min_target+j+del_demote] = tmp;
        new_gate->targets.at((u32)j) += del_demote;
      }
      min_target += del_demote;
    }
    u32 max_control = min_target - 1;
    vector<u32> nots;
    for(u32 j = 0; j < new_gate->controls.size(); j++)
    {
      u32 d = max_control - new_gate->controls.size() + j + 1;
      swaps.push_back(pair<u32, u32>(new_gate->controls.at(j).wire,d));
      new_gate->controls.at(j).wire = d;
      if(new_gate->controls.at(j).polarity == true) {
        nots.push_back(d);
        new_gate->controls.at(j).polarity = false;
      }
    }

    swaps = unit_swaps(remove_id_swaps(swaps));
    swaps_to_circ(out, swaps);
    nots_to_circ(out, nots);
    for(u32 t=0;t<new_gate->getLoopCount();t++)
    {
      out->addGate(new_gate);
    }
    nots_to_circ(out, nots);
    reverse(swaps.begin(),swaps.end());
    swaps_to_circ(out, swaps);
  }
  return out;
}

/*
int main(int argc, char ** argv) {
  UGateSetup();
  string basename = argv[1];
  vector<string> error_log;
  shared_ptr<Circuit> c = parseCircuit(basename,error_log);
  if(!c) {
    cout << "Could not load circuit:\n";
    for(auto & s : error_log) {
      cout << s;
    }
  }

  shared_ptr<Circuit> flat = circ_flatten(c);
  saveCircuit(flat,basename + "_flattened");
  shared_ptr<Circuit> normal = circ_unormalize(flat);
  saveCircuit(normal,basename + "_normalized");

  return 0;
}
*/

