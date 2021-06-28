// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <memory>
#include <string>

#include "array/array.h"
#include "array/matrix.h"
#include "clustering/distance.h"
#include "clustering/k_means.h"
#include "utils/cereal.h"
#include "utils/random.h"

int main() {
  // std::string path = "luts/flop_lut_64.cereal";
  std::string path = "luts/river_lut_64.cereal";
  // using DataT = uint32_t;
  using DataT = double;
  using DataTableT = nda::array<DataT, nda::matrix_shape<>>;

  DataTableT data_points({1, 1}, 0);
  utils::CerealLoad(path, &data_points, true);

  // clustering::KMeans<DataT, clustering::EarthMoverDistance> k(200);
  clustering::KMeans<DataT, clustering::EuclideanDistance> k(200);
  // k.RandomSumInit(data_points, 6789);
  // k.RandomSumInit(data_points, 12345);
  k.InitPlusPlus(data_points, true, utils::Random::Seed(6789));
  // k.RandomProbInit(data_points, 6789);
  // k.Elkan(data_points, true, 43555);
  k.Elkan(data_points, true, utils::Random::Seed(54321));

  // std::cout << "Done." << std::endl;
  // // should be 36
  // std::cout << histograms[276*BIN_SIZE + 29] << std::endl;
  // // should be 0
  // std::cout << e->Distance(302847, 91636) << std::endl;
  // // should be 37339
  // std::cout << e->Distance(315, 302769) << std::endl;
  return 0;
}
