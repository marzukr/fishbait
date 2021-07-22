// Copyright 2021 Marzuk Rashid

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
    nda::matrix<hand_strengths::HistCount> data_points;
    hand_strengths::FlopLUT_File(utils::FileAction::Load, &data_points, true);

    // run clustering 10 times
    clustering::KMeans<hand_strengths::HistCount,
                       clustering::EarthMoverDistance> k(200);
    k.MultipleRestarts(data_points, 10, clustering::kPlusPlus, true);

    // save best run
    utils::CerealSave(
        clustering::ClusterAssignmentFile(engine::Round::kFlop),
        k.assignments(), true);

  // cluster turn
  } else if (!strcmp(argv[1], "turn")) {
    nda::matrix<hand_strengths::HistCount> data_points;
    hand_strengths::TurnLUT_File(utils::FileAction::Load, &data_points, true);

    // run clustering 10 times
    clustering::KMeans<hand_strengths::HistCount,
                       clustering::EarthMoverDistance> k(200);
    k.MultipleRestarts(data_points, 10, clustering::kPlusPlus, true);

    // save best run
    utils::CerealSave(
        clustering::ClusterAssignmentFile(engine::Round::kTurn),
        k.assignments(), true);

  // cluster river
  } else if (!strcmp(argv[1], "river")) {
    nda::matrix<double> data_points;
    hand_strengths::RiverLUT_File(utils::FileAction::Load, &data_points, true);

    // run clustering 10 times
    clustering::KMeans<double, clustering::EuclideanDistance> k(200);
    k.MultipleRestarts(data_points, 10, clustering::kPlusPlus, true);

    // save best run
    utils::CerealSave(
        clustering::ClusterAssignmentFile(engine::Round::kRiver),
        k.assignments(), true);
  }

  return 0;
}
