// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_LOADDATA_H_
#define SRC_CLUSTERING_LOADDATA_H_

#include <stdint.h>

#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include <iterator>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>

#include "clustering/Matrix.h"

namespace clustering {

template <typename T> T GetValue(bsoncxx::array::element e) {}
template <> double GetValue<double>(bsoncxx::array::element e) {
  return e.get_double();
}
template <> uint16_t GetValue<uint16_t>(bsoncxx::array::element e) {
  return e.get_int32();
}
template <typename T>
std::unique_ptr<Matrix<T>> LoadData(std::string mongo_server,
                                    std::string db_name, std::string street,
                                    std::string list_name,
                                    bool verbose = false) {
  if (verbose) {
    std::cout << "Loading data for " << street << std::endl;
  }

  mongocxx::instance::current();
  mongocxx::uri uri(mongo_server);
  mongocxx::client client = mongocxx::client(uri);
  mongocxx::database db = client[db_name];

  mongocxx::collection collection = db[street];
  uint32_t n = collection.count_documents({});
  if (verbose) {
    std::cout << "n_documents: " << n << std::endl;
    std::cout << "\t" << "expects 1286792 for flops" << std::endl;
    std::cout << "\t" << "expects 13960050 for turns" << std::endl;
    std::cout << "\t" << "expects 123156254 for rivers" << std::endl;
  }

  bsoncxx::stdx::optional<bsoncxx::document::value> result =
      collection.find_one({});
  assert(result);
  bsoncxx::array::view v = result->view()[list_name].get_array();
  uint32_t m = std::distance(v.begin(), v.end());
  if (verbose) {
    std::cout << "m_documents: " << m << std::endl;
    std::cout << "\t" << "expects 50 for flops" << std::endl;
    std::cout << "\t" << "expects 50 for turns" << std::endl;
    std::cout << "\t" << "expects 8 for rivers" << std::endl;
  }

  std::unique_ptr<Matrix<T>> lists = std::make_unique<Matrix<T>>(n, m);

  int t = 0;
  mongocxx::cursor cursor = collection.find({});
  for (auto doc : cursor) {
    uint32_t i = doc["_id"].get_int32();
    for (uint32_t j = 0; j < lists->m(); ++j) {
      (*lists)(i, j) = GetValue<T>(doc[list_name][j]);
    }
    t += 1;
    if (verbose && t % 100000 == 0) {
      std::cout << 100.0 * t / lists->n() << "%";
      std::cout << " of data loaded" << std::endl;
    }
  }

  if (verbose) {
    std::cout << "Loaded data." << std::endl;
  }

  return lists;
}  // LoadData

}  // namespace clustering

#endif  // SRC_CLUSTERING_LOADDATA_H_
