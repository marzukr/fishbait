#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <stdint.h>
#include <iostream>
#include <cmath>
#include <random>
#include <cassert>

template <typename T>
class Array {
    public:
        Array(uint32_t n) : n_(n) { data_ = new T[n]; }
        ~Array() { delete[] data_; }
        Array(const Array&) = delete;
        Array& operator= (const Array&) = delete;

        T& operator()(uint32_t i) { 
            assert(i < n_);
            return data_[i]; 
        }
        const T& operator()(uint32_t i) const { 
            assert(i < n_);
            return data_[i]; 
        }

        void Fill(T filler) {
            for(uint32_t i = 0; i < n_; i++) {
                data_[i] = filler;
            }
        }

        T sum() const {
            T total = 0;
            for(uint32_t i = 0; i < n_; i++) {
                total += data_[i];
            }
            return total;
        }

        uint32_t n() const { return n_; };

    private:
        const uint32_t n_; // rows
        T* data_;
};

template <typename T>
class Matrix {
    public:
        Matrix(uint32_t n, uint32_t m) : n_(n), m_(m) { data_ = new T[n*m]; }
        ~Matrix() { delete[] data_; }
        Matrix (const Matrix&) = delete;
        Matrix& operator= (const Matrix&) = delete;

        T& operator()(uint32_t i, uint32_t j) { 
            assert(i < n_);
            assert(j < m_);
            return data_[i*m_ + j]; 
        }
        const T& operator()(uint32_t i, uint32_t j) const { 
            assert(i < n_);
            assert(j < m_);
            return data_[i*m_ + j]; 
        }

        struct Row {
            T* start;
            uint32_t length;
        };
        Row operator()(uint32_t i) const {
            assert(i < n_);
            Row row { &data_[i*m_], m_ };
            return row; 
        }

        void SetRow(uint32_t i, Row r) {
            assert(i < n_);
            assert(r.length == m_);
            for(uint32_t j = 0; j < m_; j++) {
                operator()(i,j) = r.start[j];
            }
        }

        void Fill(T filler) {
            for(uint32_t i = 0; i < n_*m_; i++) {
                data_[i] = filler;
            }
        }

        uint32_t n() const { return n_; };
        uint32_t m() const { return m_; };

    private:
        const uint32_t n_; // rows
        const uint32_t m_; // cols
        T* data_;
};

template <typename T>
class SymmetricMatrix {
    public:
        SymmetricMatrix(uint32_t n) : n_(n) { data_ = new T[n]; }
        ~SymmetricMatrix() { delete[] data_; }
        SymmetricMatrix (const SymmetricMatrix&) = delete;
        SymmetricMatrix& operator= (const SymmetricMatrix&) = delete;

        T& operator()(uint32_t i, uint32_t j) { 
            assert(i != j);
            assert(i < n_);
            assert(j < n_);
            if(i > j) {
                uint32_t temp = i;
                i = j;
                j = temp;
            }
            uint32_t idx = i*this->n_ - i*(i+1)/2 + j - i - 1;
            return this->data_[idx]; 
        }

        uint32_t n() { return n_; };

    private:
        const uint32_t n_; // side length
        T* data_;
};

template <typename T>
struct EarthMoverDistance {
    static double Compute(
            typename Matrix<T>::Row p, 
            typename Matrix<T>::Row q) {
        // The two distributions need to have the same number of buckets
        assert(p.length == q.length);

        double prev = 0;
        double sum = 0;
        for(uint8_t i = 1; i < p.length + 1; i++) {
            // all flops have area of 1081
            T p_i = p.start[i-1];
            T q_i = q.start[i-1];
            double nxt = p_i + prev - q_i;
            sum += std::abs(nxt);
            prev = nxt;
        }
        return sum;
    }
};

class HandDB {
    public:
        HandDB(std::string mongo_server, std::string db_name) {
            mongocxx::instance::current();
            mongocxx::uri uri(mongo_server);
            client_ = mongocxx::client(uri);
            db_ = client_[db_name];
        }
        HandDB(const HandDB&) = delete;
        HandDB& operator=(const HandDB&) = delete;

        uint32_t n_documents(std::string street) {
            mongocxx::collection collection = db_[street];
            return collection.count_documents({});
        }

        uint32_t m_documents(std::string street, std::string list_name) {
            mongocxx::collection collection = db_[street];
            bsoncxx::stdx::optional<bsoncxx::document::value> result =
                collection.find_one({});
            assert(result);
            bsoncxx::array::view v = result->view()[list_name].get_array();
            return std::distance(v.begin(), v.end());
        }

        template <typename T>
        void LoadData(Matrix<T>& lists, std::string street, 
                      std::string list_name, bool verbose = false) {

            mongocxx::collection collection = db_[street];
            mongocxx::cursor cursor = collection.find({});

            int t = 0;
            for(auto doc : cursor) {
                uint32_t i = doc["_id"].get_int32();
                for(uint32_t j = 0; j < lists.m(); j++) {
                    lists(i,j) = GetValue<T>(doc[list_name][j]);
                }
                t += 1;
                if (verbose && t % 100000 == 0) {
                    std::cout << 100.0 * t / lists.n() << "%" << std::endl;
                }
            }

            if(verbose) {
                std::cout << "Loaded data." << std::endl;
            }
        }

    private:
        template <typename T> T GetValue(bsoncxx::array::element e) {}
        template <> double GetValue<double>(bsoncxx::array::element e) {
            return e.get_double();
        }
        template <> uint16_t GetValue<uint16_t>(bsoncxx::array::element e) {
            return e.get_int32();
        } 

        mongocxx::client client_;
        mongocxx::database db_;
};

template <typename T, template<class> class Distance>
class KMeans {
    public:
        KMeans(uint32_t k) 
            : k_(k)
            , clusters_(nullptr)
            , assignments_(nullptr) {}
        KMeans(const KMeans&) = delete;
        KMeans& operator=(const KMeans&) = delete;
        ~KMeans() {
            delete clusters_;
            delete assignments_;
        }


        void Elkan(Matrix<T>& data, bool verbose = false) {
            InitPlusPlus(data, verbose);

            // Initialize the lower bounds of the distance between x and every
            // cluster to 0
            Matrix<double> lower_bounds(data.n(), k_);
            lower_bounds.Fill(0);

            // Compute and store the distance between each cluster and every
            // other cluster
            SymmetricMatrix<double> cluster_dists(k_);
            for(uint32_t c1 = 0; c1 < k_; c1++) {
                for(uint32_t c2 = c1 + 1; c2 < k_; c2++) {
                    cluster_dists(c1, c2) = Distance<T>::Compute(
                            (*clusters_)(c1), (*clusters_)(c2));
                }
            }

            // Compute the assignments for each point (the closest cluster)
            // using lemma 1 to avoid redudant distance computation, and store
            // the distance to the closest cluster as the upper bound for each
            // point
            Array<double> upper_bounds(data.n());
            Array<bool> upper_bound_loose(data.n());
            delete assignments_;
            assignments_ = new Array<uint32_t>(data.n());
            for(uint32_t x = 0; x < data.n(); x++) {
                bool uninitialized = true;
                uint32_t& c = (*assignments_)(x);
                for(uint32_t cprime = 0; cprime < k_; cprime++) {

                    // Check if we need to compute the distance to this cluster
                    // using lemma 1 and if we have already computed the 
                    // distance to at least one other cluster
                    bool calc_x_cprime_dist = false;
                    if(!uninitialized) {
                        double c_cprime_dist = cluster_dists(c, cprime);
                        double x_c_dist = upper_bounds(x);
                        if(c_cprime_dist/2 < x_c_dist) {
                            calc_x_cprime_dist = true;
                        }
                    }

                    // If we haven't computed the distance to any cluster yet,
                    // or this cluster might be closer than the current closest
                    // from lemma 1, then calculate the distance to this 
                    // cluster. Also set the lower bound of the distance between
                    // x and this cluster to the distance we just calculated
                    if(uninitialized || calc_x_cprime_dist) {
                        double x_cprime_dist = Distance<T>::Compute(
                                data(x), (*clusters_)(cprime));
                        lower_bounds(x,cprime) = x_cprime_dist;

                        // If we haven't computed the distance to any cluster 
                        // yet or this cluster is the closest cluster we have
                        // encountered so far, assign x to this cluster and set 
                        // the upper bound of x to the distance between this
                        // cluster and x
                        if(uninitialized || x_cprime_dist < upper_bounds(x)) {
                            (*assignments_)(x) = cprime;
                            upper_bounds(x) = x_cprime_dist;
                            uninitialized = false;
                        }
                    }
                }
                upper_bound_loose(x) = false;
            }

            if(verbose) {
                std::cout << "initialized data structures";
                std::cout << std::endl;
            }

            // Step 1:
            // From each cluster, compute and store 1/2 the distance to the
            // nearest other cluster
            Array<double> min_cluster_dists(k_);
            for(uint32_t c1 = 0; c1 < k_; c1++) {
                bool uninitialized = true;
                for(uint32_t c2 = 0; c2 < k_; c2++) {
                    if (c1 == c2) continue;
                    if (uninitialized) {
                        min_cluster_dists(c1) = cluster_dists(c1, c2)/2;
                    } else {
                        min_cluster_dists(c1) = std::min(
                            cluster_dists(c1, c2)/2,
                            min_cluster_dists(c1)
                        );
                    }
                }
            }

            // Step 3
            for(uint32_t c = 0; c < k_; c++) {
                for(uint32_t x = 0; x < data.n(); x++) {
                    // Step 2
                    double s_c_x = min_cluster_dists((*assignments_)(x));
                    if(upper_bounds(x) <= s_c_x) continue;

                    // Step 3(i)
                    if(c == (*assignments_)(x)) continue;

                    // Step 3(ii)
                    if(upper_bounds(x) <= lower_bounds(x, c)) continue;

                    // Step 3(iii)
                    double d_c_x_c = cluster_dists((*assignments_)(x), c)/2;
                    if(upper_bounds(x) <= d_c_x_c) continue;
                   
                    // Step 3a
                    if(upper_bound_loose(x)) {
                        upper_bounds(x) = Distance<T>::Compute(
                                data(x), (*clusters_)((*assignments_)(x)));
                        upper_bound_loose(x) = false;
                    }
                }
            }

            if(verbose) {
                int i;
                std::cout << "Done." << std::endl;
                std::cin >> i;
            }
        }

    private:
        void InitPlusPlus(Matrix<T>& data, bool verbose = false) {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_real_distribution<> std_unif(0.0,1.0);

            // Array to store the squared distances to the nearest cluster. 
            // Initially filled with ones so all points are equally likely to
            // be chosen as the first cluster
            Array<double> squared_dists(data.n());
            squared_dists.Fill(1);
            double squared_sum = data.n();

            Array<uint32_t> clusters(k_);
           
            // Assign each cluster 
            for(uint32_t c = 0; c < k_; c++) {

                // Select the next cluster based on the D^2 probability
                // distribution
                double selection = std_unif(rng);
                double cumulative_probability = 0;
                for(uint32_t x = 0; x < data.n(); x++) {
                    cumulative_probability += squared_dists(x) / squared_sum;
                    if(selection < cumulative_probability) {
                        clusters(c) = x;
                        break;
                    }
                }
                
                // For all points that are closer to the new cluster than their
                // previously assigned cluster, update squared_dists to reflect
                // the squared distance from that point to the new cluster. Also
                // initialize squared_dists if this is the first cluster and 
                // update squared_sums as we go along.
                squared_sum = 0;
                for(uint32_t x = 0; x < data.n(); x++) {
                    double new_dist = Distance<T>::Compute(data(clusters(c)), 
                                                           data(x));
                    double new_sq_dist = std::pow(new_dist, 2);
                    if(c == 0 || new_sq_dist < squared_dists(x)) {
                        squared_dists(x) = new_sq_dist;
                    }
                    squared_sum += squared_dists(x);
                }

                if (verbose) {
                    std::cout << "assigned cluster " << c;
                    std::cout << ": " << clusters(c) << std::endl;
                }
            }

            // Copy the values of the cluster data points into the clusters_
            // member variable.
            delete clusters_;
            clusters_ = new Matrix<T>(k_, data.m());
            for(uint32_t c = 0; c < k_; c++) {
                clusters_->SetRow(c, data(clusters(c)));
            }
        }

        const uint32_t k_;
        Matrix<T>* clusters_;
        Array<uint32_t>* assignments_;
};

int main() {
    std::string uri = "mongodb://localhost:27017";
    std::string db_name = "pluribus";
    std::string street = "flops";
    std::string list_name = "hist";
    typedef uint16_t data_type;

    HandDB db(uri, db_name);

    uint32_t n_documents = db.n_documents(street);
    std::cout << "n_documents: " << n_documents << std::endl;
    std::cout << "\t" << "expects 1286792 for flops" << std::endl;
    std::cout << "\t" << "expects 13960050 for turns" << std::endl;
    std::cout << "\t" << "expects 123156254 for rivers" << std::endl;

    uint32_t m_documents = db.m_documents(street, list_name);
    std::cout << "m_documents: " << m_documents << std::endl;
    std::cout << "\t" << "expects 50 for flops" << std::endl;
    std::cout << "\t" << "expects 50 for turns" << std::endl;
    std::cout << "\t" << "expects 8 for rivers" << std::endl;

    Matrix<data_type> data_points(n_documents, m_documents);
    db.LoadData<data_type>(data_points, street, list_name, true);

    KMeans<data_type, EarthMoverDistance> k(200);
    k.Elkan(data_points, true);

    // std::cout << "Done." << std::endl;
    // // should be 36
    // std::cout << histograms[276*BIN_SIZE + 29] << std::endl;
    // // should be 0 
    // std::cout << e->Distance(302847, 91636) << std::endl; 
    // // should be 37339
    // std::cout << e->Distance(315, 302769) << std::endl;
    return 0;
}
