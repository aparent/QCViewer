#ifndef _NORMALIZE_H_
#define _NORMALIZE_H_
#include "circuit.h"
#include "gate.h"
#include "types.h"
#include "subcircuit.h"
#include <vector>
#include <memory>
#include <stdint.h>

class CircuitNormalizer {
public:
  static std::shared_ptr<Circuit> normalize(std::shared_ptr<Circuit> c, bool useCNOT = false);

private:
  using u32 = uint32_t;
  using swaplist = std::vector<std::pair<u32, u32>>;
  static void subcirc_flatten(std::shared_ptr<Subcircuit> sub, std::shared_ptr<Circuit> accum);
  static std::shared_ptr<Circuit> circ_flatten(std::shared_ptr<Circuit> in);
  static swaplist remove_id_swaps(const swaplist & swaps);
  static swaplist unit_swaps(const swaplist & swaps);
  static void swaps_to_circ(std::shared_ptr<Circuit> c, swaplist & swaps, bool useCNOT);
  static void nots_to_circ(std::shared_ptr<Circuit> c, std::vector<u32> nots);
};

#endif
