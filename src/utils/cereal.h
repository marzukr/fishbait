// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_CEREAL_H_
#define SRC_UTILS_CEREAL_H_

#include <string>
#include <iostream>
#include <fstream>

#include "cereal/types/vector.hpp"
#include "cereal/archives/portable_binary.hpp"

namespace utils {

template <typename T>
void CerealSave(std::string path, T* save, bool verbose = false) {
  if (verbose) {
    std::cout << "Saving to " << path << std::endl;
  }

  std::ofstream os(path, std::ios::binary);
  cereal::PortableBinaryOutputArchive archive(os);
  archive(*save);

  if (verbose) {
    std::cout << "Saved to " << path << std::endl;
  }
}  // CerealSave

template <typename T>
void CerealLoad(std::string path, T* load, bool verbose = false) {
  if (verbose) {
    std::cout << "Loading " << path << std::endl;
  }

  std::ifstream ins(path, std::ios::binary);
  cereal::PortableBinaryInputArchive iarchive(ins);
  iarchive(*load);

  if (verbose) {
    std::cout << "Loaded " << path << std::endl;
  }
}  // CerealLoad

}  // namespace utils

#endif  // SRC_UTILS_CEREAL_H_
