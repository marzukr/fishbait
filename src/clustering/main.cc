// Copyright 2021 Marzuk Rashid

#include <stdint.h>

// #include <iostream>
#include <memory>
#include <string>

#include "clustering/EarthMoverDistance.h"
#include "clustering/KMeans.h"
#include "clustering/Matrix.h"
#include "clustering/LoadData.h"

int main() {
  std::string uri = "mongodb://localhost:27017";
  std::string db_name = "pluribus";
  std::string street = "flops";
  std::string list_name = "hist";
  typedef uint16_t data_type;

  auto data_points = clustering::LoadData<data_type>(uri, db_name, street,
                                                     list_name, true);

  clustering::KMeans<data_type, clustering::EarthMoverDistance> k(200);
  k.Elkan(*data_points, true);

  // std::cout << "Done." << std::endl;
  // // should be 36
  // std::cout << histograms[276*BIN_SIZE + 29] << std::endl;
  // // should be 0
  // std::cout << e->Distance(302847, 91636) << std::endl;
  // // should be 37339
  // std::cout << e->Distance(315, 302769) << std::endl;
  return 0;
}
