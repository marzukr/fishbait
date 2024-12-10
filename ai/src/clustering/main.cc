#include <cstdint>
#include <memory>
#include <string>
#include <cstring>

#include "array/array.h"
#include "array/matrix.h"
#include "clustering/definitions.h"
#include "clustering/distance.h"
#include "clustering/k_means.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/lut_files.h"
#include "utils/cereal.h"
#include "utils/random.h"

int main(int argc, char *argv[]) {
  // Print usage on invalid input
  if (argc != 1 + 1 || !(!strcmp(argv[1], "flop") || !strcmp(argv[1], "turn")
        || !strcmp(argv[1], "river"))) {
    std::cout << "Usage: clustering.out (flop | turn | river)"
              << std::endl;
    return 1;
  }

  // cluster flop
  if (!strcmp(argv[1], "flop")) {
    nda::matrix<fishbait::HistCount> data_points;
    fishbait::FlopLUT_File(fishbait::FileAction::Load, &data_points, true);

    // run clustering 10 times
    fishbait::KMeans<fishbait::HistCount, fishbait::EarthMoverDistance> k(
        fishbait::NumClusters(fishbait::Round::kFlop));
    k.MultipleRestarts(data_points, 10, fishbait::kPlusPlus, true);

    // save best run
    fishbait::CerealSave(
        fishbait::ClusterAssignmentFile(fishbait::Round::kFlop),
        k.assignments(), true);

  // cluster turn
  } else if (!strcmp(argv[1], "turn")) {
    nda::matrix<fishbait::HistCount> data_points;
    fishbait::TurnLUT_File(fishbait::FileAction::Load, &data_points, true);

    // run clustering 10 times
    fishbait::KMeans<fishbait::HistCount, fishbait::EarthMoverDistance> k(
        fishbait::NumClusters(fishbait::Round::kTurn));
    k.MultipleRestarts(data_points, 10, fishbait::kPlusPlus, true);

    // save best run
    fishbait::CerealSave(
        fishbait::ClusterAssignmentFile(fishbait::Round::kTurn),
        k.assignments(), true);

  // cluster river
  } else if (!strcmp(argv[1], "river")) {
    nda::matrix<double> data_points;
    fishbait::RiverLUT_File(fishbait::FileAction::Load, &data_points, true);

    // run clustering 10 times
    fishbait::KMeans<double, fishbait::EuclideanDistance> k(
        fishbait::NumClusters(fishbait::Round::kRiver));
    k.MultipleRestarts(data_points, 10, fishbait::kPlusPlus, true);

    // save best run
    fishbait::CerealSave(
        fishbait::ClusterAssignmentFile(fishbait::Round::kRiver),
        k.assignments(), true);
  }

  return 0;
}
