// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <memory>
#include <string>
#include <cstring>

#include "array/array.h"
#include "array/matrix.h"
#include "clustering/distance.h"
#include "clustering/k_means.h"
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
    // load hand_strength LUT
    nda::matrix<uint32_t> data_points({1, 1}, 0);
    utils::CerealLoad("out/hand_strengths/flop_lut_nda.cereal",
                      &data_points, true);

    // run clustering 10 times
    clustering::KMeans<uint32_t, clustering::EarthMoverDistance> k(200);
    k.MultipleRestarts(data_points, 10, clustering::kPlusPlus, true);

    // save best run
    utils::CerealSave("out/clustering/flop_clusters.cereal",
                      k.assignments(), true);

  // cluster turn
  } else if (!strcmp(argv[1], "turn")) {
    // load hand_strength LUT
    nda::matrix<uint32_t> data_points({1, 1}, 0);
    utils::CerealLoad("out/hand_strengths/turn_lut_nda.cereal",
                      &data_points, true);

    // run clustering 10 times
    clustering::KMeans<uint32_t, clustering::EarthMoverDistance> k(200);
    k.MultipleRestarts(data_points, 10, clustering::kPlusPlus, true);

    // save best run
    utils::CerealSave("out/clustering/turn_clusters.cereal",
                      k.assignments(), true);

  // cluster river
  } else if (!strcmp(argv[1], "river")) {
    // load hand_strength LUT
    nda::matrix<double> data_points({1, 1}, 0);
    utils::CerealLoad("out/hand_strengths/river_lut_nda.cereal",
                      &data_points, true);

    // run clustering 10 times
    clustering::KMeans<double, clustering::EuclideanDistance> k(200);
    k.MultipleRestarts(data_points, 10, clustering::kPlusPlus, true);

    // save best run
    utils::CerealSave("out/clustering/river_clusters.cereal",
                      k.assignments(), true);
  }

  return 0;
}
