//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/simulation/StateGenerator.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <random>

namespace ec {

void StateGenerator::seedGenerator(const std::size_t s) {
  seed = s;
  if (seed == 0U) {
    std::array<std::mt19937_64::result_type, std::mt19937_64::state_size>
        randomData{};
    std::random_device rd;
    std::generate(std::begin(randomData), std::end(randomData), std::ref(rd));
    std::seed_seq seeds(std::begin(randomData), std::end(randomData));
    mt.seed(seeds);
  } else {
    mt.seed(seed);
  }
}
} // namespace ec
