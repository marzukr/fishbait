#include <iostream>

#include "clustering/cluster_table.h"
#include "mccfr/definitions.h"
#include "mccfr/hyperparameters.h"
#include "mccfr/strategy.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "relay/scribe.h"

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <average location> "
              << "<hdf save location>" << std::endl;
    return 1;
  }
  using StrategyT = fishbait::Strategy<fishbait::hparam::kPlayers,
                                       fishbait::hparam::kActions,
                                       fishbait::ClusterTable>;
  using AverageT = typename StrategyT::Average;
  AverageT avg = AverageT::LoadAverage(argv[1], true);
  fishbait::Scribe<fishbait::hparam::kPlayers, fishbait::hparam::kActions,
                   fishbait::ClusterTable> ahmes{avg, argv[2], true};
  return 0;
}
