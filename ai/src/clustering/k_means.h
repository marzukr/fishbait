#ifndef AI_SRC_CLUSTERING_K_MEANS_H_
#define AI_SRC_CLUSTERING_K_MEANS_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <thread>  // NOLINT(build/c++11)
#include <utility>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "clustering/definitions.h"
#include "utils/combination_matrix.h"
#include "utils/random.h"
#include "utils/timer.h"

namespace fishbait {

enum InitProc { kPlusPlus, kRandomSum, kRandomProb };

template <typename T, template <class, class> class Distance>
class KMeans {
 public:
  explicit KMeans(MeansN k)
      : k_{k}, clusters_{nullptr}, assignments_{nullptr}, loss_{INFINITY} {}
  KMeans(MeansN k, std::unique_ptr<nda::matrix<double>> initial_clusters)
      : k_{k}, clusters_{std::move(initial_clusters)}, assignments_{nullptr},
        loss_{INFINITY} {}

  KMeans(const KMeans& other) : k_{other.k_}, loss_{other.loss_} {
    clusters_ = std::make_unique<nda::matrix<double>>(*other.clusters_);
    assignments_ = std::make_unique<std::vector<MeanId>>(*other.assignments_);
  }
  KMeans& operator=(const KMeans& other) = delete;

  /*
    @brief Run kmeans clustering several times and pick the best one.

    @param data The data points to cluster.
    @param restarts The number of times to run kmeans.
    @param initializer The algorithm to use to genereate the initial clusters.
    @param verbose Print diagnostics information.
    @param seed Seed to use for assigning empty clusters and initializing
        clusters. If no seed is passed then a random seed is chosen.
  */
  void MultipleRestarts(const nda::matrix<T>& data, uint32_t restarts,
                        InitProc initializer = kPlusPlus, bool verbose = false,
                        Random::Seed seed = Random::Seed()) {
    // Variables to store the clustering with the lowest loss
    std::unique_ptr<nda::matrix<double>> clusters(nullptr);
    std::unique_ptr<std::vector<MeanId>> assignments(nullptr);
    double loss = INFINITY;
    uint32_t best_trial = 0;

    // Random number generator to generate seeds
    Random rng(seed);
    std::uniform_int_distribution<uint32_t> seed_gen;

    // Repeat the clustering the specified number of times
    for (uint32_t t = 0; t < restarts; ++t) {
      if (verbose) {
        std::cout << "starting trial " << t << std::endl;
      }

      // Set the starting clusters
      switch (initializer) {
        case kPlusPlus:
          InitPlusPlus(data, verbose, Random::Seed(seed_gen(rng())));
          break;
        case kRandomSum:
          RandomSumInit(data, Random::Seed(seed_gen(rng())));
          break;
        case kRandomProb:
          RandomProbInit(data, Random::Seed(seed_gen(rng())));
          break;
      }  // switch (initializer)

      // Run kmeans until convergence
      Elkan(data, verbose, Random::Seed(seed_gen(rng())));

      // If the new clustering is better than the best one so far, replace it
      if (loss_ < loss) {
        loss = loss_;
        clusters = std::move(clusters_);
        assignments = std::move(assignments_);
        best_trial = t;
      }
    }  // for t

    // Set the instance variables to the best clustering found
    loss_ = loss;
    clusters_ = std::move(clusters);
    assignments_ = std::move(assignments);

    if (verbose) {
      std::cout << "best trial: " << best_trial << std::endl;
      std::cout << "best loss: " << loss_ << std::endl;
    }
  }  // MultipleRestarts()

  /*
    @brief Run kmeans clustering with Elkan's algorithm.

    @param data The data points to cluster.
    @param verbose Print diagnostics information.
    @param seed Seed to use for assigning empty clusters and initializing
        clusters if they are not already initialized.
  */
  void Elkan(const nda::matrix<T>& data, bool verbose = false,
             Random::Seed seed = Random::Seed()) {
    std::unique_ptr<nda::matrix<double>> clusters(nullptr);
    if (clusters_ == nullptr) {
      InitPlusPlus(data, verbose, seed);
    }
    clusters = std::make_unique<nda::matrix<double>>(*clusters_);
    double loss = INFINITY;

    // Initialize the lower bounds of the distance between x and every cluster
    // to 0
    nda::matrix<double> lower_bounds({data.rows(), k_}, 0);

    // Compute and store the distance between each cluster and every other
    // cluster
    CombinationMatrix<double> cluster_dists(k_);
    for (MeanId c1 = 0; c1 < k_; ++c1) {
      for (MeanId c2 = c1 + 1; c2 < k_; ++c2) {
        cluster_dists(c1, c2) = Distance<double, double>::Compute(
            (*clusters)(c1, nda::all), (*clusters)(c2, nda::all));
      }
    }

    // An array to keep track of 1/2 the distance to the shortest other cluster
    // from each cluster
    std::vector<double> half_min_cluster_dists(k_);

    // Compute the assignments for each point (the closest cluster) using
    // lemma 1 to avoid redudant distance computation, and store the distance
    // to the closest cluster as the upper bound for each point.
    std::vector<double> upper_bounds(data.rows());
    std::vector<bool> upper_bound_loose(data.rows(), true);
    auto assignments = std::make_unique<std::vector<MeanId>>(data.rows());
    for (nda::index_t x = 0; x < data.rows(); ++x) {
      (*assignments)[x] = 0;
      upper_bounds[x] = Distance<T, double>::Compute(data(x, nda::all),
                                                     (*clusters)(0, nda::all));
      lower_bounds(x, 0) = upper_bounds[x];

      MeanId& c = (*assignments)[x];
      for (MeanId cprime = 1; cprime < k_; ++cprime) {
        // If this cluster might be closer than the current closest from
        // lemma 1, then calculate the distance to this cluster. Also set the
        // lower bound of the distance between x and this cluster to the
        // distance we just calculated

        if (cluster_dists(c, cprime)/2 < upper_bounds[x]) {
          double x_cprime_dist = Distance<T, double>::Compute(
              data(x, nda::all), (*clusters)(cprime, nda::all));
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

    // Initialize random number generator for filling empty clusters
    Random rng(seed);
    std::uniform_real_distribution<> std_unif(0.0, 1.0);

    if (verbose) {
      std::cout << "initialized data structures";
      std::cout << std::endl;
    }

    const nda::index_t cores = std::max(std::thread::hardware_concurrency(),
        (uint32_t) 1);
    const uint32_t n_threads = std::min(data.rows(), cores);
    std::vector<std::thread> threads(n_threads);

    bool converged = false;
    uint32_t iteration = 0;
    Timer t;
    while (!converged) {
      // Step 1
      // For all centers c and c', compute the distance between them
      for (MeanId c1 = 0; c1 < k_; ++c1) {
        for (MeanId c2 = c1 + 1; c2 < k_; ++c2) {
          cluster_dists(c1, c2) = Distance<double, double>::Compute(
              (*clusters)(c1, nda::all), (*clusters)(c2, nda::all));
        }
      }
      // From each cluster, compute and store 1/2 the distance to the nearest
      // other cluster
      for (MeanId c1 = 0; c1 < k_; ++c1) {
        bool uninitialized = true;
        for (MeanId c2 = 0; c2 < k_; ++c2) {
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
      if (verbose) t.Reset(std::cout << "step 1: ") << std::endl;

      // Step 3
      for (uint32_t thread = 0; thread < n_threads; ++thread) {
        threads[thread] = std::thread([&, thread]() {
              for (nda::index_t x = thread; x < data.rows(); x += n_threads) {
                MeanId& c_x = (*assignments)[x];

                // Step 2
                if (upper_bounds[x] <= half_min_cluster_dists[c_x]) continue;

                for (MeanId c = 0; c < k_; ++c) {
                  // Step 3(i)
                  if (c == c_x) continue;

                  // Step 3(ii)
                  if (upper_bounds[x] <= lower_bounds(x, c)) continue;

                  // Step 3(iii)
                  if (upper_bounds[x] <= cluster_dists(c_x, c)/2) continue;

                  // Step 3a
                  if (upper_bound_loose[x]) {
                    upper_bounds[x] = Distance<T, double>::Compute(
                        data(x, nda::all), (*clusters)(c_x, nda::all));
                    lower_bounds(x, c_x) = upper_bounds[x];
                    upper_bound_loose[x] = false;
                  }

                  // Step 3b
                  if (upper_bounds[x] > lower_bounds(x, c) ||
                      upper_bounds[x] > cluster_dists(c_x, c)/2) {
                    double x_c_dist = Distance<T, double>::Compute(
                        data(x, nda::all), (*clusters)(c, nda::all));
                    lower_bounds(x, c) = x_c_dist;
                    if (x_c_dist < upper_bounds[x]) {
                      c_x = c;
                      upper_bounds[x] = x_c_dist;
                    }
                  }
                }  // for c
              }  // for x
            }  // [&, thread]()
        );  // std::thread  NOLINT(whitespace/parens)
      }  // for thread
      for (uint32_t thread = 0; thread < n_threads; ++thread) {
        threads[thread].join();
      }  // for thread
      if (verbose) t.Reset(std::cout << "step 2,3: ") << std::endl;

      // Step 4
      // First, sum the data points assigned to each cluster
      nda::matrix_shape<> means_shape{k_, data.columns()};
      auto means = std::make_unique<nda::matrix<double>>(means_shape, 0);
      std::vector<nda::size_t> cluster_counts(k_, 0);
      for (nda::index_t x = 0; x < data.rows(); ++x) {
        MeanId c = (*assignments)[x];
        (*means)(c, nda::all) += data(x, nda::all);
        cluster_counts[c] += 1;
      }
      // Find any empty clusters
      std::vector<MeansN> empty_clusters;
      for (MeanId c = 0; c < k_; ++c) {
        if (cluster_counts[c] == 0) {
          empty_clusters.push_back(c);
        }
      }
      // If there are empty clusters, fill them with the kmeans++ algorithm
      if (empty_clusters.size() > 0) {
        if (verbose) {
          std::cout << "empty clusters: " << empty_clusters.size() << std::endl;
        }
        // Calculate the squared distances between each point and it's assigned
        // cluster, and the sum of those distances. These values are needed for
        // kmeans++.
        std::vector<double> squared_dists(data.rows());
        double squared_sum = 0;
        for (uint32_t x = 0; x < data.rows(); ++x) {
          squared_dists[x] = upper_bounds[x] * upper_bounds[x];
          squared_sum += squared_dists[x];
        }

        // Fill each empty cluster by selecting a point with kmeans++, removing
        // that point from its current cluster, then adding it to the empty
        // cluster.
        for (std::size_t i = 0; i < empty_clusters.size(); ++i) {
          double selection = std_unif(rng());
          nda::index_t x = InitPlusPlusIter(
            data, &squared_dists, &squared_sum, selection);
          MeanId old_c = (*assignments)[x];
          MeanId new_c = empty_clusters[i];

          (*means)(old_c, nda::all) -= data(x, nda::all);
          (*means)(new_c, nda::all) += data(x, nda::all);

          cluster_counts[old_c] += -1;
          cluster_counts[new_c] += 1;
        }
      }
      for (auto i : means->i()) {
        for (auto j : means->j()) {
          (*means)(i, j) /= cluster_counts[i];
        }
      }
      if (verbose) t.Reset(std::cout << "step 4") << std::endl;

      // Step 5
      std::vector<double> cluster_to_means(k_);
      for (MeanId c = 0; c < k_; ++c) {
        cluster_to_means[c] = Distance<double, double>::Compute(
            (*clusters)(c, nda::all), (*means)(c, nda::all));
      }
      for (uint32_t thread = 0; thread < n_threads; ++thread) {
        threads[thread] = std::thread([&, thread]() {
              for (nda::index_t x = thread; x < data.rows(); x += n_threads) {
                for (MeanId c = 0; c < k_; ++c) {
                  double dist_diff = lower_bounds(x, c) - cluster_to_means[c];
                  lower_bounds(x, c) = std::max(dist_diff, 0.0);
                }  // for c
              }  // for x
            }  // [&, thread]()
        );  // std::thread  NOLINT(whitespace/parens)
      }  // for thread
      for (uint32_t thread = 0; thread < n_threads; ++thread) {
        threads[thread].join();
      }  // for thread
      if (verbose) t.Reset(std::cout << "step 5: ") << std::endl;

      // Step 6
      for (nda::index_t x = 0; x < data.rows(); ++x) {
        MeanId c_x = (*assignments)[x];
        upper_bounds[x] = upper_bounds[x] + cluster_to_means[c_x];
        upper_bound_loose[x] = true;
      }
      if (verbose) t.Reset(std::cout << "step 6: ") << std::endl;

      // Step 7
      converged = (*clusters) == (*means);
      clusters = std::move(means);
      loss = ComputeLoss(data, *clusters, *assignments);
      if (verbose) t.Reset(std::cout << "step 7") << std::endl;

      iteration += 1;
      if (verbose) {
        std::cout << "computed iteration " << iteration;
        std::cout << ", converged: " << converged << std::endl;
        std::cout << std::setprecision(17) << loss << std::endl;
      }
    }  // while !converged

    // Update the clusters, assignments, and loss.
    clusters_ = std::move(clusters);
    assignments_ = std::move(assignments);
    loss_ = loss;
  }  // Elkan()

  /*
    @brief Initialize clusters with the kmeans++ algorithm.

    @param data The data points to select from.
    @param verbose Print diagnostics information.
    @param seed Seed to use for the random number generation. If no seed is
        passed then a random one is chosen.
  */
  void InitPlusPlus(const nda::matrix<T>& data, bool verbose = false,
                    Random::Seed seed = Random::Seed()) {
    Random rng(seed);
    std::uniform_real_distribution<> std_unif(0.0, 1.0);

    // Array to store the squared distances to the nearest cluster. Initially
    // filled with ones so all points are equally likely to be chosen as the
    // first cluster
    std::vector<double> squared_dists(data.rows(), INFINITY);
    double squared_sum = data.rows();

    std::vector<nda::index_t> clusters(k_);

    // Assign each cluster
    for (MeanId c = 0; c < k_; ++c) {
      double selection = std_unif(rng());
      clusters[c] = InitPlusPlusIter(data, &squared_dists, &squared_sum,
                                     selection);
      if (verbose) {
        std::cout << "assigned cluster " << c;
        std::cout << ": " << clusters[c] << std::endl;
      }
    }  // for c

    // Copy the values of the cluster data points into a new Matrix
    auto filled_clusters = std::make_unique<nda::matrix<double>>(
        nda::matrix_shape<>{k_, data.columns()});
    for (MeanId c = 0; c < k_; ++c) {
      (*filled_clusters)(c, nda::all).copy_elems(data(clusters[c], nda::all));
    }
    clusters_ = std::move(filled_clusters);
  }  // InitPlusPlus()

  /*
    @brief Initialize clusters whose sum is the same as the data elements.

    @param data The data points whose sum to mimic.
    @param seed Seed to use for the random number generation. If no seed is
        passed then a random one is chosen.
  */
  void RandomSumInit(const nda::matrix<T>& data,
                     Random::Seed seed = Random::Seed()) {
    T row_sum = 0;
    for (nda::index_t j = 0; j < data.columns(); ++j) {
      row_sum += data(0, j);
    }

    Random rng(seed);
    std::uniform_int_distribution<nda::index_t> choose_bucket(
        0, data.columns() - 1);
    std::uniform_real_distribution<double> choose_amount(0.0, row_sum + 1);

    auto clusters = std::make_unique<nda::matrix<double>>(
        nda::matrix_shape<>{k_, data.columns()});

    // Assign each cluster
    for (MeanId c = 0; c < k_; ++c) {
      double sum_remaining = row_sum;
      while (sum_remaining > 0) {
        nda::index_t bucket = choose_bucket(rng());
        double amount = choose_amount(rng());
        if (amount > sum_remaining) {
          amount = sum_remaining;
        }
        (*clusters)(c, bucket) += amount;
        sum_remaining -= amount;
      }  // while sum_remaining > 0
    }  // for c

    clusters_ = std::move(clusters);
  }  // RandomSumInit()

  /*
    @brief Initialize clusters whose values are probabilities.

    @param data The data points to derive dimensionality from.
    @param seed Seed to use for the random number generation. If no seed is
        passed then a random one is chosen.
  */
  void RandomProbInit(const nda::matrix<T>& data,
                      Random::Seed seed = Random::Seed()) {
    Random rng(seed);
    std::uniform_real_distribution<double> choose_amount(0.0, 1.0);

    auto clusters = std::make_unique<nda::matrix<double>>(
        nda::matrix_shape<>{k_, data.columns()});

    // Assign each cluster
    for (MeanId c = 0; c < k_; ++c) {
      for (nda::index_t j = 0; j < data.columns(); ++j) {
        (*clusters)(c, j) = choose_amount(rng());
      }  // for j
    }  // for c

    clusters_ = std::move(clusters);
  }  // RandomProbInit()

  const nda::matrix<double>* clusters() const {
    return clusters_.get();
  }

  const std::vector<MeanId>* assignments() const {
    return assignments_.get();
  }

  double loss() const {
    return loss_;
  }

 private:
  /*
    @brief Run one iteration of the kmeans++ algorithm.

    @param data The data points to select from.
    @param squared_dists The squared distances between each point and the
        closest cluster. Set all values to INFINITY if this is the first
        iteration.
    @param squared_sum The sum of the squared distances.
    @param selection A randomly selected number on the interval [0,1).

    @returns Index of the point in data that is selected to be the new cluster.
  */
  nda::index_t InitPlusPlusIter(const nda::matrix<T>& data,
                                std::vector<double>* squared_dists,
                                double* squared_sum, double selection) {
    nda::index_t new_cluster = 0;

    // Select the next cluster based on the D^2 probability distribution
    double cumulative_probability = 0;
    for (nda::index_t x = 0; x < data.rows(); ++x) {
      // If this is the first iteration, give every point equal probability
      if ((*squared_dists)[x] == INFINITY) {
        cumulative_probability += 1.0 / data.rows();
      } else {
        assert(squared_sum != 0);
        cumulative_probability += (*squared_dists)[x] / *squared_sum;
      }

      if (selection < cumulative_probability) {
        new_cluster = x;
        break;
      }
    }

    // For all points that are closer to the new cluster than their
    // previously assigned cluster, update squared_dists to reflect the
    // squared distance from that point to the new cluster. Also update
    // squared_sum as we go along.
    *squared_sum = 0;
    for (nda::index_t x = 0; x < data.rows(); ++x) {
      double new_dist = Distance<T, T>::Compute(data(new_cluster, nda::all),
                                                data(x, nda::all));
      double new_sq_dist = std::pow(new_dist, 2);
      if (new_sq_dist < (*squared_dists)[x]) {
        (*squared_dists)[x] = new_sq_dist;
      }
      *squared_sum += (*squared_dists)[x];
    }

    return new_cluster;
  }  // InitPlusPlusIter()

  double ComputeLoss(const nda::matrix<T>& data,
                     const nda::matrix<double>& clusters,
                     const std::vector<MeanId>& assignments) {
    double squared_sum = 0;
    for (nda::index_t x = 0; x < data.rows(); ++x) {
      MeanId c_x = assignments[x];
      double dist_to_cluster = Distance<T, double>::Compute(
          data(x, nda::all), clusters(c_x, nda::all));
      squared_sum += std::pow(dist_to_cluster, 2);
    }
    return squared_sum/data.rows();
  }  // ComputeLoss()

  const MeansN k_;
  std::unique_ptr<nda::matrix<double>> clusters_;
  std::unique_ptr<std::vector<MeanId>> assignments_;
  double loss_;
};  // KMeans

}  // namespace fishbait

#endif  // AI_SRC_CLUSTERING_K_MEANS_H_
