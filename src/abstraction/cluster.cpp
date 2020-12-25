#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <stdint.h>
#include <iostream>
#include <cmath>


const uint32_t BIN_SIZE = 50;

double emd(uint16_t* histograms, uint32_t p, uint32_t q) {
    double prev = 0;
    double sum = 0;
    for(uint8_t i = 1; i < BIN_SIZE + 1; i++) {
        // all flops have area of 1081
        uint16_t p_i = histograms[p*BIN_SIZE + i-1];
        uint16_t q_i = histograms[q*BIN_SIZE + i-1];
        double nxt = p_i + prev - q_i;
        sum += std::abs(nxt);
        prev = nxt;
    }
    return sum;
}

int main() {
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri("mongodb://localhost:27017");
    mongocxx::client client(uri);

    mongocxx::database db = client["pluribus"];
    mongocxx::collection flops = db["flops"];

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
    // should be 36
    std::cout << histograms[276*BIN_SIZE + 29] << std::endl;
    // should be 0 
    std::cout << emd(histograms, 302847, 91636) << std::endl; 
    // should be 37339
    std::cout << emd(histograms, 315, 302769) << std::endl; 
}
