#include <stdint.h>

#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include <iterator>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "clustering/EarthMoverDistance.h"
#include "clustering/KMeans.h"
#include "clustering/Matrix.h"

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
      (*lists)(i,j) = GetValue<T>(doc[list_name][j]);
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
}

int main() {
  std::string uri = "mongodb://localhost:27017";
  std::string db_name = "pluribus";
  std::string street = "flops";
  std::string list_name = "hist";
  typedef uint16_t data_type;

  auto data_points = LoadData<data_type>(uri, db_name, street, list_name, true);

  KMeans<data_type, EarthMoverDistance> k(200);
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
