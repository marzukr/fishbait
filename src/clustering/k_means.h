// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_K_MEANS_H_
#define SRC_CLUSTERING_K_MEANS_H_

#include <stdint.h>

#include <iostream>
#include <memory>
#include <random>
#include <cassert>
#include <algorithm>
#include <utility>
#include <vector>

#include "utils/matrix.h"
#include "utils/combination_matrix.h"
#include "utils/vector_view.h"

namespace clustering {

template <typename T, template<class, class> class Distance>
class KMeans {
 public:
  explicit KMeans(uint32_t k)
      : k_(k), clusters_(nullptr), assignments_(nullptr), loss_(-1) {}
  KMeans(uint32_t k, std::unique_ptr<utils::Matrix<double>> initial_clusters)
      : k_(k), clusters_(std::move(initial_clusters)), assignments_(nullptr),
        loss_(-1) {}

  void Elkan(const utils::Matrix<T>& data, bool verbose = false) {
    std::unique_ptr<utils::Matrix<double>> clusters(nullptr);
    if (clusters_ == nullptr) {
      clusters = InitPlusPlus(data, verbose);
    } else {
      clusters = std::make_unique<utils::Matrix<double>>(*clusters_);
    }

    // Initialize the lower bounds of the distance between x and every
    // cluster to 0
    utils::Matrix<double> lower_bounds(data.n(), k_, 0);

    // Compute and store the distance between each cluster and every
    // other cluster
    utils::CombinationMatrix<double> cluster_dists(k_);
    for (uint32_t c1 = 0; c1 < k_; ++c1) {
      for (uint32_t c2 = c1 + 1; c2 < k_; ++c2) {
        cluster_dists(c1, c2) = Distance<double, double>::Compute(
            (*clusters)(c1), (*clusters)(c2));
      }
    }

    // An array to keep track of 1/2 the distance to the shortest other cluster
    // from each cluster
    std::vector<double> half_min_cluster_dists(k_);

    // Compute the assignments for each point (the closest cluster) using
    // lemma 1 to avoid redudant distance computation, and store the distance
    // to the closest cluster as the upper bound for each point.
    std::vector<double> upper_bounds(data.n());
    std::vector<bool> upper_bound_loose(data.n(), true);
    auto assignments = std::make_unique<std::vector<uint32_t>>(data.n());
    for (uint32_t x = 0; x < data.n(); ++x) {
      (*assignments)[x] = 0;
      upper_bounds[x] = Distance<T, double>::Compute(data(x), (*clusters)(0));
      lower_bounds(x, 0) = upper_bounds[x];

      uint32_t& c = (*assignments)[x];
      for (uint32_t cprime = 1; cprime < k_; ++cprime) {
        // If this cluster might be closer than the current closest from
        // lemma 1, then calculate the distance to this cluster. Also set the
        // lower bound of the distance between x and this cluster to the
        // distance we just calculated

        if (cluster_dists(c, cprime)/2 < upper_bounds[x]) {
          double x_cprime_dist = Distance<T, double>::Compute(
              data(x), (*clusters)(cprime));
          lower_bounds(x, cprime) = x_cprime_dist;

          // If this cluster is the closest cluster we have encountered so far,
          // assign x to this cluster and set the upper bound of x to the
          // distance between this cluster and x
          if (x_cprime_dist < upper_bounds[x]) {
            (*assignments)[x] = cprime;
            upper_bounds[x] = x_cprime_dist;
          }
        }
      }  // for c
      upper_bound_loose[x] = false;
    }  // for x

    if (verbose) {
      std::cout << "initialized data structures";
      std::cout << std::endl;
    }

    bool converged = false;
    uint32_t iteration = 0;
    while (!converged) {
      // Step 1
      // For all centers c and c', compute the distance between them
      for (uint32_t c1 = 0; c1 < k_; ++c1) {
        for (uint32_t c2 = c1 + 1; c2 < k_; ++c2) {
          cluster_dists(c1, c2) = Distance<double, double>::Compute(
              (*clusters)(c1), (*clusters)(c2));
        }
      }
      // From each cluster, compute and store 1/2 the distance to the nearest
      // other cluster
      for (uint32_t c1 = 0; c1 < k_; ++c1) {
        bool uninitialized = true;
        for (uint32_t c2 = 0; c2 < k_; ++c2) {
          if (c1 == c2) continue;
          if (uninitialized) {
            half_min_cluster_dists[c1] = cluster_dists(c1, c2)/2;
            uninitialized = false;
          } else {
            half_min_cluster_dists[c1] = std::min(cluster_dists(c1, c2)/2,
                                                  half_min_cluster_dists[c1]);
          }
        }
      }

      // Step 3
      for (uint32_t x = 0; x < data.n(); ++x) {
        uint32_t& c_x = (*assignments)[x];

        // Step 2
        if (upper_bounds[x] <= half_min_cluster_dists[c_x]) continue;

        for (uint32_t c = 0; c < k_; ++c) {
          // Step 3(i)
          if (c == c_x) continue;

          // Step 3(ii)
          if (upper_bounds[x] <= lower_bounds(x, c)) continue;

          // Step 3(iii)
          if (upper_bounds[x] <= cluster_dists(c_x, c)/2) continue;

          // Step 3a
          if (upper_bound_loose[x]) {
            upper_bounds[x] = Distance<T, double>::Compute(data(x),
                                                           (*clusters)(c_x));
            lower_bounds(x, c_x) = upper_bounds[x];
            upper_bound_loose[x] = false;
          }

          // Step 3b
          if (upper_bounds[x] > lower_bounds(x, c) ||
              upper_bounds[x] > cluster_dists(c_x, c)/2) {
            double x_c_dist = Distance<T, double>::Compute(data(x),
                                                           (*clusters)(c));
            lower_bounds(x, c) = x_c_dist;
            if (x_c_dist < upper_bounds[x]) {
              c_x = c;
              upper_bounds[x] = x_c_dist;
            }
          }
        }  // for c
      }  // for x

      // Step 4
      auto means = std::make_unique<utils::Matrix<double>>(k_, data.m(), 0);
      std::vector<uint32_t> cluster_counts(k_, 0);
      for (uint32_t x = 0; x < data.n(); ++x) {
        uint32_t c = (*assignments)[x];
        means->template AddToRow<T>(c, data(x));
        cluster_counts[c] += 1;
      }
      means->Divide(utils::VectorView(cluster_counts));

      // Step 5
      std::vector<double> cluster_to_means(k_);
      for (uint32_t c = 0; c < k_; ++c) {
        cluster_to_means[c] = Distance<double, double>::Compute((*clusters)(c),
                                                                (*means)(c));
        for (uint32_t x = 0; x < data.n(); ++x) {
          lower_bounds(x, c) = std::max(lower_bounds(x, c)-cluster_to_means[c],
                                        0.0);
        }
      }

      // Step 6
      for (uint32_t x = 0; x < data.n(); ++x) {
        uint32_t c_x = (*assignments)[x];
        upper_bounds[x] = upper_bounds[x] + cluster_to_means[c_x];
        upper_bound_loose[x] = true;
      }

      // Step 7
      converged = (*clusters) == (*means);
      clusters = std::move(means);
      loss_ = ComputeLoss(data, *clusters, *assignments);

      iteration += 1;
      if (verbose) {
        std::cout << "computed iteration " << iteration;
        std::cout << ", converged: " << converged << std::endl;
        std::cout << loss_ << std::endl;
      }
    }  // while !converged

    // Compute the loss and set the instance variables to the clusters and
    // assignments we just computed
    clusters_ = std::move(clusters);
    assignments_ = std::move(assignments);
  }  // Elkan

  std::unique_ptr<utils::Matrix<double>> InitPlusPlus(
      const utils::Matrix<T>& data, bool verbose = false, int32_t seed = -1) {
    if (seed < 0) {
      std::random_device dev;
      seed = dev();
    }
    uint16_t unsigned_seed = seed;
    std::mt19937 rng(unsigned_seed);
    std::uniform_real_distribution<> std_unif(0.0, 1.0);

    // Array to store the squared distances to the nearest cluster. Initially
    // filled with ones so all points are equally likely to be chosen as the
    // first cluster
    std::vector<double> squared_dists(data.n(), 1);
    double squared_sum = data.n();

    std::vector<uint32_t> clusters(k_);

    // Assign each cluster
    for (uint32_t c = 0; c < k_; ++c) {
      // Select the next cluster based on the D^2 probability distribution
      double selection = std_unif(rng);
      double cumulative_probability = 0;
      for (uint32_t x = 0; x < data.n(); ++x) {
        assert(squared_sum != 0);
        cumulative_probability += squared_dists[x] / squared_sum;
        if (selection < cumulative_probability) {
          clusters[c] = x;
          break;
        }
      }

      // For all points that are closer to the new cluster than their
      // previously assigned cluster, update squared_dists to reflect the
      // squared distance from that point to the new cluster. Also initialize
      // squared_dists if this is the first cluster and update squared_sums as
      // we go along.
      squared_sum = 0;
      for (uint32_t x = 0; x < data.n(); ++x) {
        double new_dist = Distance<T, T>::Compute(data(clusters[c]), data(x));
        double new_sq_dist = std::pow(new_dist, 2);
        if (c == 0 || new_sq_dist < squared_dists[x]) {
          squared_dists[x] = new_sq_dist;
        }
        squared_sum += squared_dists[x];
      }

      if (verbose) {
        std::cout << "assigned cluster " << c;
        std::cout << ": " << clusters[c] << std::endl;
      }
    }  // for c

    // Copy the values of the cluster data points into a new Matrix
    auto filled_clusters = std::make_unique<utils::Matrix<double>>(k_,
                                                                   data.m());
    for (uint32_t c = 0; c < k_; ++c) {
      filled_clusters->template SetRow<T>(c, data(clusters[c]));
    }
    return filled_clusters;
  }  // InitPlusPlus

  const utils::Matrix<double>* clusters() const {
    return clusters_.get();
  }

  const std::vector<uint32_t>* assignments() const {
    return assignments_.get();
  }

  double loss() const {
    return loss_;
  }

 private:
  double ComputeLoss(const utils::Matrix<T>& data,
                     const utils::Matrix<double>& clusters,
                     const std::vector<uint32_t>& assignments) {
    double squared_sum = 0;
    for (uint32_t x = 0; x < data.n(); ++x) {
      uint32_t c_x = assignments[x];
      double dist_to_cluster = Distance<T, double>::Compute(data(x),
                                                            clusters(c_x));
      squared_sum += std::pow(dist_to_cluster, 2);
    }
    return squared_sum/data.n();
  }

  const uint32_t k_;
  std::unique_ptr<utils::Matrix<double>> clusters_;
  std::unique_ptr<std::vector<uint32_t>> assignments_;
  double loss_;
};  // KMeans

}  // namespace clustering

#endif  // SRC_CLUSTERING_K_MEANS_H_
