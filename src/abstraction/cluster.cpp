#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include <stdint.h>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

int main() {
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri("mongodb://localhost:27017");
    mongocxx::client client(uri);

    mongocxx::database db = client["pluribus"];
    mongocxx::collection flops = db["flops"];

    const uint32_t BIN_SIZE = 50; 
    uint16_t* histograms = new uint16_t[1286792*BIN_SIZE];

    mongocxx::cursor cursor = flops.find({});
    int t = 0;
    for(auto doc : cursor) {
        uint32_t i = doc["_id"].get_int32();
        for(uint32_t j = 0; j < BIN_SIZE; j++) {
            histograms[i*BIN_SIZE + j] = doc["hist"][j].get_int32();
        }
        t += 1;
        if (t % 10000 == 0) {
            std::cout << t << std::endl;
        }
    }

    std::cout << "Done." << std::endl;
    std::cout << histograms[276*BIN_SIZE + 29] << std::endl; // should be 36
}
