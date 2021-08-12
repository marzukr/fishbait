// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_CEREAL_H_
#define SRC_UTILS_CEREAL_H_

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "array/array.h"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/tuple.hpp"
#include "cereal/types/vector.hpp"

namespace fishbait {

enum class FileAction { Load, Save };

template <typename T>
void CerealSave(const std::string_view path, T* save, bool verbose = false) {
  if (verbose) {
    std::cout << "Saving to " << path << std::endl;
  }

  std::ofstream os(path.data(), std::ios::binary);
  cereal::PortableBinaryOutputArchive archive(os);
  archive(*save);

  if (verbose) {
    std::cout << "Saved to " << path << std::endl;
  }
}  // CerealSave

template <typename T>
void CerealLoad(const std::string_view path, T* load, bool verbose = false) {
  if (verbose) {
    std::cout << "Loading " << path << std::endl;
  }

  std::ifstream ins(path.data(), std::ios::binary);
  cereal::PortableBinaryInputArchive iarchive(ins);
  iarchive(*load);

  if (verbose) {
    std::cout << "Loaded " << path << std::endl;
  }
}  // CerealLoad

template <typename T>
void Cereal(FileAction action, std::string path, T* data,
            bool verbose = false) {
  switch (action) {
    case FileAction::Load:
      CerealLoad(path, data, verbose);
      break;
    case FileAction::Save:
      CerealSave(path, data, verbose);
      break;
  }
}

}  // namespace fishbait

namespace cereal {

/* nda::array cereal load/save */
template<class Archive, class T, class Shape, class Alloc>
void save(Archive& archive,  // NOLINT(runtime/references)
          const nda::array<T, Shape, Alloc>& m) {
  archive(m.get_allocator(), m.shape());
  m.for_each_value([&](const T& x) { archive(x); });
}
template<class Archive, class T, class Shape, class Alloc>
void load(Archive& archive,  // NOLINT(runtime/references)
          nda::array<T, Shape, Alloc>& m) {  // NOLINT(runtime/references)
  Alloc alloc;
  Shape shape;
  archive(alloc, shape);
  nda::array<T, Shape, Alloc> new_array(shape, alloc);
  new_array.for_each_value([&](T& x) { archive(x); });
  m = std::move(new_array);
}

/* nda::shape cereal load/save */
template <class Archive, class... Dims>
void save(Archive& archive,  // NOLINT(runtime/references)
          const nda::shape<Dims...>& m) {
  archive(m.dims());
}
template <class Archive, class... Dims>
void load(Archive& archive,  // NOLINT(runtime/references)
          nda::shape<Dims...>& m) {  // NOLINT(runtime/references)
  std::tuple<Dims...> dims;
  archive(dims);
  nda::shape<Dims...> shape(dims);
  m = std::move(shape);
}

/* nda::dims cereal load/save */
template <class Archive, nda::index_t Min_, nda::index_t Extent_,
          nda::index_t Stride_>
void save(Archive& archive,  // NOLINT(runtime/references)
          const nda::dim<Min_, Extent_, Stride_>& m) {
  archive(m.min(), m.extent(), m.stride());
}
template <class Archive, nda::index_t Min_, nda::index_t Extent_,
          nda::index_t Stride_>
void load(Archive& archive,  // NOLINT(runtime/references)
          nda::dim<Min_, Extent_, Stride_>& m) {  // NOLINT(runtime/references)
  nda::index_t min, extent, stride;
  archive(min, extent, stride);
  nda::dim<Min_, Extent_, Stride_> new_dim(min, extent, stride);
  m = std::move(new_dim);
}

}  // namespace cereal

#endif  // SRC_UTILS_CEREAL_H_
