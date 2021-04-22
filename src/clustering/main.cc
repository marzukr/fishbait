// Copyright 2021 Marzuk Rashid

#include <stdint.h>

// #include <iostream>
#include <memory>
#include <string>

#include "clustering/distance.h"
#include "clustering/k_means.h"
#include "hand_strengths/lut_generators.h"
#include "utils/matrix.h"

int main() {
  // std::string path = "luts/flop_lut.cereal";
  std::string path = "luts/river_lut.cereal";
  std::cout << path << std::endl;
  return 0;
  // typedef uint32_t data_type;
  typedef double data_type;

  utils::Matrix<data_type> data_points(1, 1, 0);
  hand_strengths::LoadLUT(path, &data_points, true);

  // clustering::KMeans<data_type, clustering::EarthMoverDistance> k(200);
  clustering::KMeans<data_type, clustering::EuclideanDistance> k(200);
  // k.RandomSumInit(data_points, 6789);
  k.InitPlusPlus(data_points, true, 6789);
  k.Elkan(data_points, true, 43555);

  // std::cout << "Done." << std::endl;
  // // should be 36
  // std::cout << histograms[276*BIN_SIZE + 29] << std::endl;
  // // should be 0
  // std::cout << e->Distance(302847, 91636) << std::endl;
  // // should be 37339
  // std::cout << e->Distance(315, 302769) << std::endl;
  return 0;
}
