#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <stdint.h>
#include <iostream>
#include <cmath>
#include <random>

template <typename T> T GetValue(bsoncxx::array::element e) {}

template <> double GetValue<double>(bsoncxx::array::element e) {
    return e.get_double();
}

template <> uint16_t GetValue<uint16_t>(bsoncxx::array::element e) {
    return e.get_int32();
}

template <template<typename> typename S, typename T>
S<T>* LoadData(std::string mongo_server, std::string db_name, 
        std::string street, std::string list_name, int bin_size, 
        bool verbose = false) {
    mongocxx::instance::current();
    mongocxx::uri uri(mongo_server);
    mongocxx::client client(uri);

    mongocxx::database db = client[db_name];
    mongocxx::collection collection = db[street];

    int n_documents = collection.count_documents({});
    if(verbose) {
        std::cout << "n_documents: " << n_documents << std::endl;
        std::cout << "\t" << "expects 1286792 for flops" << std::endl;
        std::cout << "\t" << "expects 13960050 for turns" << std::endl;
        std::cout << "\t" << "expects 123156254 for rivers" << std::endl;
    }
    T* lists = new T[n_documents*bin_size];

    mongocxx::cursor cursor = collection.find({});
    int t = 0;
    for(auto doc : cursor) {
        uint32_t i = doc["_id"].get_int32();
        for(uint32_t j = 0; j < bin_size; j++) {
            lists[i*bin_size + j] = GetValue<T>(doc[list_name][j]);
        }
        t += 1;
        if (verbose && t % 100000 == 0) {
            std::cout << 100.0 * t / n_documents << "%" << std::endl;
        }
    }

    if(verbose) {
        std::cout << "Loaded data." << std::endl;
    }

    S<T>* data = new S<T>(lists, n_documents, bin_size);
    return data;
}

template <typename T>
class HandData {
    public:
        HandData(T* data_array, uint32_t n_documents, uint16_t bin_size) 
            : binSize_(bin_size)
            , nDocuments_(n_documents)
            , data_(data_array)
        {}
        ~HandData() { delete[] data_; }

        virtual double Distance(uint32_t p, uint32_t q) = 0;

        T data(uint32_t i, uint16_t j) {
            return data_[i*binSize_ + j];
        }

        uint32_t nDocuments() { return nDocuments_; }
        uint16_t binSize() { return binSize_; }
    protected:
        const uint16_t binSize_;
        const uint32_t nDocuments_;
        const T* data_;
};

template <typename T>
class EHSDistribution: public HandData<T> {
    public:
        EHSDistribution(T* data_array, uint32_t n_documents, 
                        uint16_t bin_size) 
            : HandData<T>(data_array, n_documents, bin_size)
        {}

        double Distance(uint32_t p, uint32_t q) {
            double prev = 0;
            double sum = 0;
            for(uint8_t i = 1; i < this->binSize_ + 1; i++) {
                // all flops have area of 1081
                T p_i = this->data(p, i-1);
                T q_i = this->data(q, i-1);
                double nxt = p_i + prev - q_i;
                sum += std::abs(nxt);
                prev = nxt;
            }
            return sum;
        }
};

template <typename T>
class KMeans {
    public:
        KMeans(HandData<T>* data, uint32_t k) : data_(data), k_(k) {
            clusters_ = new T[k*data_->binSize()];
        }
        ~KMeans() { delete[] clusters_; }

        void InitPlusPlus(bool verbose = false) {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_real_distribution<> std_unif(0.0,1.0);

            double* squared_dists = new double[data_->nDocuments()];
            for(uint32_t i = 0; i < data_->nDocuments(); i++) {
                squared_dists[i] = 1;
            }

            uint32_t* clusters = new uint32_t[k_];
            
            for(uint32_t c = 0; c < k_; c++) {
                double squared_sum = 0;
                for(uint32_t i = 0; i < data_->nDocuments(); i++) {
                    squared_sum += squared_dists[i];
                }

                double selection = std_unif(rng);
                double cumulative_probability = 0;
                for(uint32_t i = 0; i < data_->nDocuments(); i++) {
                    cumulative_probability += squared_dists[i] / squared_sum;
                    if(selection < cumulative_probability) {
                        clusters[c] = i;
                        break;
                    }
                }

                for(uint32_t i = 0; i < data_->nDocuments(); i++) {
                    double new_distance = 
                        std::pow(data_->Distance(clusters[c], i), 2);
                    if(c == 0 || new_distance < squared_dists[i]) {
                        squared_dists[i] = new_distance;
                    }
                }

                if (verbose) {
                    std::cout << "assigned cluster " << c;
                    std::cout << ": " << clusters[c] << std::endl;
                }
            }

            for(uint32_t c = 0; c < k_; c++) {
                for(uint16_t j = 0; j < data_->binSize(); j++) {
                    clusters_[c*data_->binSize() + j] = 
                        data_->data(clusters[c], j);
                }
            }

            delete[] squared_dists;
            delete[] clusters;
        }
    private:
        HandData<T>* data_;
        const uint32_t k_;
        T* clusters_;
};

int main() {
    const uint16_t binSize = 50;

    EHSDistribution<uint16_t>* e = LoadData<EHSDistribution, uint16_t>(
        "mongodb://localhost:27017",
        "pluribus",
        "flops",
        "hist",
        binSize,
        true
    );

    KMeans k(e, 200);
    k.InitPlusPlus(true);
    // std::cout << "Done." << std::endl;
    // // should be 36
    // std::cout << histograms[276*BIN_SIZE + 29] << std::endl;
    // // should be 0 
    // std::cout << e->Distance(302847, 91636) << std::endl; 
    // // should be 37339
    // std::cout << e->Distance(315, 302769) << std::endl;
    return 0;
}
