// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_KMEANS_H_
#define SRC_CLUSTERING_KMEANS_H_

#include <stdint.h>

#include <iostream>
#include <memory>
#include <random>
#include <cassert>
#include <algorithm>
#include <utility>

#include "clustering/Matrix.h"
#include "clustering/Array.h"
#include "clustering/SymmetricMatrix.h"

namespace clustering {

template <typename T, template<class, class> class Distance>
class KMeans {
 public:
  explicit KMeans(uint32_t k) : k_(k), clusters_(nullptr), assignments_(nullptr)
                              , loss_(nullptr) {}

  void Elkan(const Matrix<T>& data, bool verbose = false) {
    std::unique_ptr<Matrix<double>> clusters = InitPlusPlus(data, verbose);

    // Initialize the lower bounds of the distance between x and every
    // cluster to 0
    Matrix<double> lower_bounds(data.n(), k_);
    lower_bounds.Fill(0);

    // Compute and store the distance between each cluster and every
    // other cluster
    SymmetricMatrix<double> cluster_dists(k_);
    for (uint32_t c1 = 0; c1 < k_; ++c1) {
      for (uint32_t c2 = c1 + 1; c2 < k_; ++c2) {
        cluster_dists(c1, c2) = Distance<double, double>::Compute(
            (*clusters)(c1), (*clusters)(c2));
      }
    }

    // Compute the assignments for each point (the closest cluster) using
    // lemma 1 to avoid redudant distance computation, and store the distance
    // to the closest cluster as the upper bound for each point.
    Array<double> upper_bounds(data.n());
    Array<bool> upper_bound_loose(data.n());
    auto assignments = std::make_unique<Array<uint32_t>>(data.n());
    for (uint32_t x = 0; x < data.n(); ++x) {
      bool uninitialized = true;
      uint32_t& c = (*assignments)(x);
      for (uint32_t cprime = 0; cprime < k_; ++cprime) {

        // Check if we need to compute the distance to this cluster using
        // lemma 1 and if we have already computed the distance to at least
        // one other cluster
        bool calc_x_cprime_dist = false;
        if (!uninitialized) {
          double c_cprime_dist = cluster_dists(c, cprime);
          double x_c_dist = upper_bounds(x);
          if (c_cprime_dist/2 < x_c_dist) {
            calc_x_cprime_dist = true;
          }
        }

        // If we haven't computed the distance to any cluster yet, or this
        // cluster might be closer than the current closest from lemma 1, then
        // calculate the distance to this cluster. Also set the lower bound of
        // the distance between x and this cluster to the distance we just
        // calculated
        if (uninitialized || calc_x_cprime_dist) {
          double x_cprime_dist = Distance<T,double>::Compute(
              data(x), (*clusters)(cprime));
          lower_bounds(x, cprime) = x_cprime_dist;

          // If we haven't computed the distance to any cluster yet or this
          // cluster is the closest cluster we have encountered so far, assign
          // x to this cluster and set the upper bound of x to the distance
          // between this cluster and x
          if (uninitialized || x_cprime_dist < upper_bounds(x)) {
            (*assignments)(x) = cprime;
            upper_bounds(x) = x_cprime_dist;
            uninitialized = false;
          }
        }
      }
      upper_bound_loose(x) = false;
    }

    if (verbose) {
      std::cout << "initialized data structures";
      std::cout << std::endl;
    }

    bool converged = false;
    uint32_t iteration = 0;
    while (!converged) {
      // Step 1:
      // From each cluster, compute and store 1/2 the distance to the nearest
      // other cluster
      Array<double> half_min_cluster_dists(k_);
      for (uint32_t c1 = 0; c1 < k_; ++c1) {
        bool uninitialized = true;
        for (uint32_t c2 = 0; c2 < k_; ++c2) {
          if (c1 == c2) continue;
          if (uninitialized) {
            half_min_cluster_dists(c1) = cluster_dists(c1, c2)/2;
          } else {
            half_min_cluster_dists(c1) = std::min(cluster_dists(c1, c2)/2,
                                                  half_min_cluster_dists(c1));
          }
        }
      }

      // Step 3
      for (uint32_t c = 0; c < k_; ++c) {
        for (uint32_t x = 0; x < data.n(); ++x) {
          uint32_t& c_x = (*assignments)(x);

          // Step 2
          if (upper_bounds(x) <= half_min_cluster_dists(c_x)) continue;

          // Step 3(i)
          if (c == c_x) continue;

          // Step 3(ii)
          if (upper_bounds(x) <= lower_bounds(x, c)) continue;

          // Step 3(iii)
          if (upper_bounds(x) <= cluster_dists(c_x, c)/2) continue;

          // Step 3a
          if (upper_bound_loose(x)) {
            upper_bounds(x) = Distance<T, double>::Compute(data(x),
                                                           (*clusters)(c_x));
            lower_bounds(x, c_x) = upper_bounds(x);
            upper_bound_loose(x) = false;
          }

          // Step 3b
          if (upper_bounds(x) > lower_bounds(x, c) ||
              upper_bounds(x) > cluster_dists(c_x, c)/2) {
            double x_c_dist = Distance<T, double>::Compute(data(x),
                                                           (*clusters)(c));
            lower_bounds(x, c) = x_c_dist;
            if (x_c_dist < upper_bounds(x)) {
              c_x = c;
              upper_bounds(x) = x_c_dist;
            }
          }
        }
      }

      // Step 4
      auto means = std::make_unique<Matrix<double>>(k_, data.m());
      means->Fill(0);
      Array<uint32_t> cluster_counts(k_);
      cluster_counts.Fill(0);
      for (uint32_t x = 0; x < data.n(); ++x) {
        uint32_t c = (*assignments)(x);
        means->template AddToRow<T>(c, data(x));
        cluster_counts(c) += 1;
      }
      means->Divide(cluster_counts);

      // Step 5
      Array<double> cluster_to_means(k_);
      for (uint32_t c = 0; c < k_; ++c) {
        cluster_to_means(c) = Distance<double, double>::Compute((*clusters)(c),
                                                                (*means)(c));
        for (uint32_t x = 0; x < data.n(); ++x) {
          lower_bounds(x, c) = std::max(lower_bounds(x,c)-cluster_to_means(c),
                                        0.0);
        }
      }

      // Step 6
      for (uint32_t x = 0; x < data.n(); ++x) {
        uint32_t c_x = (*assignments)(x);
        upper_bounds(x) = upper_bounds(x) + cluster_to_means(c_x);
        upper_bound_loose(x) = true;
      }

      // Step 7
      converged = (*clusters) == (*means);
      clusters = std::move(means);

      if (verbose) {
        std::cout << "computed iteration " << iteration;
        std::cout << ", converged: " << converged << std::endl;
      }
      iteration += 1;
      loss_ = std::make_unique<double>(ComputeLoss(data, *clusters,
                                                    *assignments));
      std::cout << loss() << std::endl;
    }

    // Compute the loss and set the instance variables to the clusters and
    // assignments we just computed
    clusters_ = std::move(clusters);
    assignments_ = std::move(assignments);
  }

  double loss() const {
    if (loss_) return *loss_;
    return -1;
  }

 private:
  std::unique_ptr<Matrix<double>> InitPlusPlus(const Matrix<T>& data,
                                                bool verbose = false) const {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> std_unif(0.0, 1.0);

    // Array to store the squared distances to the nearest cluster. Initially
    // filled with ones so all points are equally likely to be chosen as the
    // first cluster
    Array<double> squared_dists(data.n());
    squared_dists.Fill(1);
    double squared_sum = data.n();

    Array<uint32_t> clusters(k_);

    // Assign each cluster
    for (uint32_t c = 0; c < k_; ++c) {
      // Select the next cluster based on the D^2 probability distribution
      double selection = std_unif(rng);
      double cumulative_probability = 0;
      for (uint32_t x = 0; x < data.n(); ++x) {
        assert(squared_sum != 0);
        cumulative_probability += squared_dists(x) / squared_sum;
        if (selection < cumulative_probability) {
          clusters(c) = x;
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
        double new_dist = Distance<T, T>::Compute(data(clusters(c)), data(x));
        double new_sq_dist = std::pow(new_dist, 2);
        if (c == 0 || new_sq_dist < squared_dists(x)) {
          squared_dists(x) = new_sq_dist;
        }
        squared_sum += squared_dists(x);
      }

      if (verbose) {
        std::cout << "assigned cluster " << c;
        std::cout << ": " << clusters(c) << std::endl;
      }
    }

    // Copy the values of the cluster data points into a new Matrix
    auto filled_clusters = std::make_unique<Matrix<double>>(k_, data.m());
    for (uint32_t c = 0; c < k_; ++c) {
      filled_clusters->template SetRow<T>(c, data(clusters(c)));
    }
    return filled_clusters;
  }

  double ComputeLoss(const Matrix<T>& data, const Matrix<double>& clusters,
                      const Array<uint32_t>& assignments) {
    double squared_sum = 0;
    for (uint32_t x = 0; x < data.n(); ++x) {
      uint32_t c_x = assignments(x);
      double dist_to_cluster = Distance<T, double>::Compute(data(x),
                                                            clusters(c_x));
      squared_sum += std::pow(dist_to_cluster/1081, 2);
    }
    return squared_sum/data.n();
  }

  const uint32_t k_;
  std::unique_ptr<Matrix<double>> clusters_;
  std::unique_ptr<Array<uint32_t>> assignments_;
  std::unique_ptr<double> loss_;
};

}  // namespace clustering

#endif  // SRC_CLUSTERING_KMEANS_H_
