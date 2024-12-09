#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "catch2/catch.hpp"
#include "clustering/definitions.h"
#include "clustering/distance.h"
#include "clustering/k_means.h"
#include "utils/random.h"

// Elkan test cases generated with python and the kmeans from scikit learn after
// disabling the mean centering and changing the cluster mean computation to
// divide by the cluster count instead of multiplying by 1/cluster_count

TEST_CASE("Elkan 10 double points 2 dimensions 3 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0{-2.849093076616996, -7.50099441961392};
  std::vector<double> d1{-8.376679683595523, -6.575072471573815};
  std::vector<double> d2{1.854593255308436, 2.8373498485777353};
  std::vector<double> d3{-6.693924628259479, -10.798167105362953};
  std::vector<double> d4{-1.8438013762461565, 3.752765455389377};
  std::vector<double> d5{-3.8002521843738686, 10.506326248059725};
  std::vector<double> d6{1.8336376195925768, 11.124731633368821};
  std::vector<double> d7{-0.1036452031827384, 0.4268789785280571};
  std::vector<double> d8{2.1428126285447715, -1.9106654947313197};
  std::vector<double> d9{-3.783596278287021, 7.733529305880115};

  nda::matrix<double> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));

  std::vector<double> c0{0.6425235614350431, 0.898903293670528};
  std::vector<double> c1{0.4670259744021872, 3.2289955311358631};
  std::vector<double> c2{0.03126336690842668, 0.7057866456528065};

  auto initial_centers = std::make_unique<nda::matrix<double>>(
      nda::matrix_shape<>{3, 2});
  (*initial_centers)(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  (*initial_centers)(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  (*initial_centers)(2, nda::all).copy_elems(nda::vector_ref<double>(c2));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(
      3, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{1.2979202268901564, 0.4511877774581576};
  std::vector<double> correct_c1{-1.8985030548286173, 8.27933816067451};
  std::vector<double> correct_c2{-5.973232462823998, -8.291411332183563};

  nda::matrix<double> correct_centers(nda::matrix_shape<>{3, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(correct_c2));

  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{2, 2, 0, 2, 1, 1, 1, 0, 0,
                                                    1};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 9.511703026188766);
}  // TEST_CASE("Elkan 10 double points 2 dimensions 3 clusters")

TEST_CASE("Elkan 10 int points 2 dimensions 5 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{4, 0};
  std::vector<int8_t> d1{-10, -7};
  std::vector<int8_t> d2{5, 4};
  std::vector<int8_t> d3{-3, 3};
  std::vector<int8_t> d4{6, -7};
  std::vector<int8_t> d5{-3, -1};
  std::vector<int8_t> d6{-11, -3};
  std::vector<int8_t> d7{-1, -6};
  std::vector<int8_t> d8{5, -6};
  std::vector<int8_t> d9{-5, 0};

  nda::matrix<int8_t> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  std::vector<double> c0{4, 0};
  std::vector<double> c1{-3, -1};
  std::vector<double> c2{-5, 0};
  std::vector<double> c3{-1, -6};
  std::vector<double> c4{-11, -3};

  auto initial_centers = std::make_unique<nda::matrix<double>>(
      nda::matrix_shape<>{5, 2});
  (*initial_centers)(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  (*initial_centers)(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  (*initial_centers)(2, nda::all).copy_elems(nda::vector_ref<double>(c2));
  (*initial_centers)(3, nda::all).copy_elems(nda::vector_ref<double>(c3));
  (*initial_centers)(4, nda::all).copy_elems(nda::vector_ref<double>(c4));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(
      5, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{4.5, 2};
  std::vector<double> correct_c1{-3, -1};
  std::vector<double> correct_c2{-4, 1.5};
  std::vector<double> correct_c3{3.3333333333333335, -6.333333333333333};
  std::vector<double> correct_c4{-10.5, -5};

  nda::matrix<double> correct_centers({5, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(correct_c2));
  correct_centers(3, nda::all).copy_elems(nda::vector_ref<double>(correct_c3));
  correct_centers(4, nda::all).copy_elems(nda::vector_ref<double>(correct_c4));


  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{0, 4, 0, 2, 3, 1, 4, 3, 3,
                                                    2};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 5.283333333333333);
}  // TEST_CASE("Elkan 10 int points 2 dimensions 5 clusters")

TEST_CASE("Elkan 100 double points 2 dimensions 5 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0{0.71037809, 0.59228149};
  std::vector<double> d1{0.43521826, 0.20536252};
  std::vector<double> d2{0.42099718, 0.72536447};
  std::vector<double> d3{0.28685326, 0.17896796};
  std::vector<double> d4{0.56699846, 0.2066689 };
  std::vector<double> d5{0.2505008 , 0.04128258};
  std::vector<double> d6{0.40910783, 0.49740991};
  std::vector<double> d7{0.83184395, 0.06042595};
  std::vector<double> d8{0.58303171, 0.80870602};
  std::vector<double> d9{0.5110118 , 0.00829052};
  std::vector<double> d10{0.87067952, 0.73352395};
  std::vector<double> d11{0.50848953, 0.39255353};
  std::vector<double> d12{0.202395  , 0.79414119};
  std::vector<double> d13{0.47063875, 0.16934181};
  std::vector<double> d14{0.94024931, 0.93561098};
  std::vector<double> d15{0.83633095, 0.18616019};
  std::vector<double> d16{0.65634763, 0.70305817};
  std::vector<double> d17{0.52843685, 0.29647434};
  std::vector<double> d18{0.39478678, 0.81687561};
  std::vector<double> d19{0.37862747, 0.72488447};
  std::vector<double> d20{0.71024215, 0.7199736 };
  std::vector<double> d21{0.13519057, 0.22432005};
  std::vector<double> d22{0.81436469, 0.03733808};
  std::vector<double> d23{0.81497713, 0.84686035};
  std::vector<double> d24{0.81280439, 0.2135163 };
  std::vector<double> d25{0.67800732, 0.84972272};
  std::vector<double> d26{0.28516836, 0.25734372};
  std::vector<double> d27{0.90666335, 0.83225067};
  std::vector<double> d28{0.01437716, 0.69738476};
  std::vector<double> d29{0.46334119, 0.04849681};
  std::vector<double> d30{0.18270574, 0.72471009};
  std::vector<double> d31{0.15991311, 0.88654659};
  std::vector<double> d32{0.64482129, 0.21750975};
  std::vector<double> d33{0.46858113, 0.56200624};
  std::vector<double> d34{0.82244189, 0.055486  };
  std::vector<double> d35{0.91516033, 0.05434445};
  std::vector<double> d36{0.49174711, 0.82891325};
  std::vector<double> d37{0.40159183, 0.67683017};
  std::vector<double> d38{0.49629342, 0.95006373};
  std::vector<double> d39{0.39327414, 0.20379409};
  std::vector<double> d40{0.03537113, 0.67729425};
  std::vector<double> d41{0.13944478, 0.51132967};
  std::vector<double> d42{0.24786146, 0.9979489 };
  std::vector<double> d43{0.17689153, 0.59747679};
  std::vector<double> d44{0.99960278, 0.62760246};
  std::vector<double> d45{0.95537494, 0.06321662};
  std::vector<double> d46{0.15884366, 0.03777492};
  std::vector<double> d47{0.08399516, 0.40581108};
  std::vector<double> d48{0.2684556 , 0.1495085 };
  std::vector<double> d49{0.84215796, 0.20972184};
  std::vector<double> d50{0.85140573, 0.20241931};
  std::vector<double> d51{0.49137035, 0.48091827};
  std::vector<double> d52{0.20380094, 0.02625395};
  std::vector<double> d53{0.97317805, 0.41711418};
  std::vector<double> d54{0.27358188, 0.93390015};
  std::vector<double> d55{0.21717853, 0.59970123};
  std::vector<double> d56{0.78253112, 0.34964113};
  std::vector<double> d57{0.79472639, 0.37929546};
  std::vector<double> d58{0.05628126, 0.92709535};
  std::vector<double> d59{0.899846  , 0.24461422};
  std::vector<double> d60{0.72613833, 0.56802926};
  std::vector<double> d61{0.53098313, 0.84314209};
  std::vector<double> d62{0.41532588, 0.04324068};
  std::vector<double> d63{0.09626306, 0.62473727};
  std::vector<double> d64{0.080844  , 0.70511902};
  std::vector<double> d65{0.20179152, 0.80349125};
  std::vector<double> d66{0.14198254, 0.25578225};
  std::vector<double> d67{0.24517668, 0.0932282 };
  std::vector<double> d68{0.61135518, 0.26358368};
  std::vector<double> d69{0.19960774, 0.43151394};
  std::vector<double> d70{0.57236918, 0.61012496};
  std::vector<double> d71{0.77469292, 0.96733762};
  std::vector<double> d72{0.38706222, 0.82214022};
  std::vector<double> d73{0.61330617, 0.1852579 };
  std::vector<double> d74{0.8765257 , 0.35298426};
  std::vector<double> d75{0.63746657, 0.65640791};
  std::vector<double> d76{0.70295037, 0.61979672};
  std::vector<double> d77{0.92459424, 0.96084995};
  std::vector<double> d78{0.45774023, 0.88977362};
  std::vector<double> d79{0.81064255, 0.71727283};
  std::vector<double> d80{0.58167621, 0.40543831};
  std::vector<double> d81{0.07375783, 0.00965682};
  std::vector<double> d82{0.35478275, 0.32663972};
  std::vector<double> d83{0.24133482, 0.7247678 };
  std::vector<double> d84{0.67047112, 0.69881828};
  std::vector<double> d85{0.98459936, 0.41066128};
  std::vector<double> d86{0.66816408, 0.76127041};
  std::vector<double> d87{0.12618173, 0.60533953};
  std::vector<double> d88{0.59453877, 0.18010352};
  std::vector<double> d89{0.49279454, 0.02174845};
  std::vector<double> d90{0.58364569, 0.44730103};
  std::vector<double> d91{0.33912786, 0.33848887};
  std::vector<double> d92{0.62918524, 0.8174672 };
  std::vector<double> d93{0.13093402, 0.03886702};
  std::vector<double> d94{0.08773659, 0.48254595};
  std::vector<double> d95{0.4348808 , 0.87695909};
  std::vector<double> d96{0.12026081, 0.21720335};
  std::vector<double> d97{0.27875053, 0.69659322};
  std::vector<double> d98{0.92054913, 0.08922499};
  std::vector<double> d99{0.96321426, 0.91838007};

  nda::matrix<double> features({100, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));
  features(10, nda::all).copy_elems(nda::vector_ref<double>(d10));
  features(11, nda::all).copy_elems(nda::vector_ref<double>(d11));
  features(12, nda::all).copy_elems(nda::vector_ref<double>(d12));
  features(13, nda::all).copy_elems(nda::vector_ref<double>(d13));
  features(14, nda::all).copy_elems(nda::vector_ref<double>(d14));
  features(15, nda::all).copy_elems(nda::vector_ref<double>(d15));
  features(16, nda::all).copy_elems(nda::vector_ref<double>(d16));
  features(17, nda::all).copy_elems(nda::vector_ref<double>(d17));
  features(18, nda::all).copy_elems(nda::vector_ref<double>(d18));
  features(19, nda::all).copy_elems(nda::vector_ref<double>(d19));
  features(20, nda::all).copy_elems(nda::vector_ref<double>(d20));
  features(21, nda::all).copy_elems(nda::vector_ref<double>(d21));
  features(22, nda::all).copy_elems(nda::vector_ref<double>(d22));
  features(23, nda::all).copy_elems(nda::vector_ref<double>(d23));
  features(24, nda::all).copy_elems(nda::vector_ref<double>(d24));
  features(25, nda::all).copy_elems(nda::vector_ref<double>(d25));
  features(26, nda::all).copy_elems(nda::vector_ref<double>(d26));
  features(27, nda::all).copy_elems(nda::vector_ref<double>(d27));
  features(28, nda::all).copy_elems(nda::vector_ref<double>(d28));
  features(29, nda::all).copy_elems(nda::vector_ref<double>(d29));
  features(30, nda::all).copy_elems(nda::vector_ref<double>(d30));
  features(31, nda::all).copy_elems(nda::vector_ref<double>(d31));
  features(32, nda::all).copy_elems(nda::vector_ref<double>(d32));
  features(33, nda::all).copy_elems(nda::vector_ref<double>(d33));
  features(34, nda::all).copy_elems(nda::vector_ref<double>(d34));
  features(35, nda::all).copy_elems(nda::vector_ref<double>(d35));
  features(36, nda::all).copy_elems(nda::vector_ref<double>(d36));
  features(37, nda::all).copy_elems(nda::vector_ref<double>(d37));
  features(38, nda::all).copy_elems(nda::vector_ref<double>(d38));
  features(39, nda::all).copy_elems(nda::vector_ref<double>(d39));
  features(40, nda::all).copy_elems(nda::vector_ref<double>(d40));
  features(41, nda::all).copy_elems(nda::vector_ref<double>(d41));
  features(42, nda::all).copy_elems(nda::vector_ref<double>(d42));
  features(43, nda::all).copy_elems(nda::vector_ref<double>(d43));
  features(44, nda::all).copy_elems(nda::vector_ref<double>(d44));
  features(45, nda::all).copy_elems(nda::vector_ref<double>(d45));
  features(46, nda::all).copy_elems(nda::vector_ref<double>(d46));
  features(47, nda::all).copy_elems(nda::vector_ref<double>(d47));
  features(48, nda::all).copy_elems(nda::vector_ref<double>(d48));
  features(49, nda::all).copy_elems(nda::vector_ref<double>(d49));
  features(50, nda::all).copy_elems(nda::vector_ref<double>(d50));
  features(51, nda::all).copy_elems(nda::vector_ref<double>(d51));
  features(52, nda::all).copy_elems(nda::vector_ref<double>(d52));
  features(53, nda::all).copy_elems(nda::vector_ref<double>(d53));
  features(54, nda::all).copy_elems(nda::vector_ref<double>(d54));
  features(55, nda::all).copy_elems(nda::vector_ref<double>(d55));
  features(56, nda::all).copy_elems(nda::vector_ref<double>(d56));
  features(57, nda::all).copy_elems(nda::vector_ref<double>(d57));
  features(58, nda::all).copy_elems(nda::vector_ref<double>(d58));
  features(59, nda::all).copy_elems(nda::vector_ref<double>(d59));
  features(60, nda::all).copy_elems(nda::vector_ref<double>(d60));
  features(61, nda::all).copy_elems(nda::vector_ref<double>(d61));
  features(62, nda::all).copy_elems(nda::vector_ref<double>(d62));
  features(63, nda::all).copy_elems(nda::vector_ref<double>(d63));
  features(64, nda::all).copy_elems(nda::vector_ref<double>(d64));
  features(65, nda::all).copy_elems(nda::vector_ref<double>(d65));
  features(66, nda::all).copy_elems(nda::vector_ref<double>(d66));
  features(67, nda::all).copy_elems(nda::vector_ref<double>(d67));
  features(68, nda::all).copy_elems(nda::vector_ref<double>(d68));
  features(69, nda::all).copy_elems(nda::vector_ref<double>(d69));
  features(70, nda::all).copy_elems(nda::vector_ref<double>(d70));
  features(71, nda::all).copy_elems(nda::vector_ref<double>(d71));
  features(72, nda::all).copy_elems(nda::vector_ref<double>(d72));
  features(73, nda::all).copy_elems(nda::vector_ref<double>(d73));
  features(74, nda::all).copy_elems(nda::vector_ref<double>(d74));
  features(75, nda::all).copy_elems(nda::vector_ref<double>(d75));
  features(76, nda::all).copy_elems(nda::vector_ref<double>(d76));
  features(77, nda::all).copy_elems(nda::vector_ref<double>(d77));
  features(78, nda::all).copy_elems(nda::vector_ref<double>(d78));
  features(79, nda::all).copy_elems(nda::vector_ref<double>(d79));
  features(80, nda::all).copy_elems(nda::vector_ref<double>(d80));
  features(81, nda::all).copy_elems(nda::vector_ref<double>(d81));
  features(82, nda::all).copy_elems(nda::vector_ref<double>(d82));
  features(83, nda::all).copy_elems(nda::vector_ref<double>(d83));
  features(84, nda::all).copy_elems(nda::vector_ref<double>(d84));
  features(85, nda::all).copy_elems(nda::vector_ref<double>(d85));
  features(86, nda::all).copy_elems(nda::vector_ref<double>(d86));
  features(87, nda::all).copy_elems(nda::vector_ref<double>(d87));
  features(88, nda::all).copy_elems(nda::vector_ref<double>(d88));
  features(89, nda::all).copy_elems(nda::vector_ref<double>(d89));
  features(90, nda::all).copy_elems(nda::vector_ref<double>(d90));
  features(91, nda::all).copy_elems(nda::vector_ref<double>(d91));
  features(92, nda::all).copy_elems(nda::vector_ref<double>(d92));
  features(93, nda::all).copy_elems(nda::vector_ref<double>(d93));
  features(94, nda::all).copy_elems(nda::vector_ref<double>(d94));
  features(95, nda::all).copy_elems(nda::vector_ref<double>(d95));
  features(96, nda::all).copy_elems(nda::vector_ref<double>(d96));
  features(97, nda::all).copy_elems(nda::vector_ref<double>(d97));
  features(98, nda::all).copy_elems(nda::vector_ref<double>(d98));
  features(99, nda::all).copy_elems(nda::vector_ref<double>(d99));

  std::vector<double> c0{0.4348808 , 0.87695909};
  std::vector<double> c1{0.01437716, 0.69738476};
  std::vector<double> c2{0.56699846, 0.2066689 };
  std::vector<double> c3{0.92054913, 0.08922499};
  std::vector<double> c4{0.49137035, 0.48091827};

  auto initial_centers = std::make_unique<nda::matrix<double>>(
      nda::matrix_shape<>{5, 2});
  (*initial_centers)(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  (*initial_centers)(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  (*initial_centers)(2, nda::all).copy_elems(nda::vector_ref<double>(c2));
  (*initial_centers)(3, nda::all).copy_elems(nda::vector_ref<double>(c3));
  (*initial_centers)(4, nda::all).copy_elems(nda::vector_ref<double>(c4));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(
      5, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{0.7148766666666666 , 0.8232170028571429 };
  std::vector<double> correct_c1{0.18534709565217392, 0.7039716665217391 };
  std::vector<double> correct_c2{0.2941162019047619 , 0.13788537095238096};
  std::vector<double> correct_c3{0.8068981166666667 , 0.2085375242857143 };
  std::vector<double> correct_c4{0.5530856528571428 , 0.5379240435714286 };

  nda::matrix<double> correct_centers({5, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(correct_c2));
  correct_centers(3, nda::all).copy_elems(nda::vector_ref<double>(correct_c3));
  correct_centers(4, nda::all).copy_elems(nda::vector_ref<double>(correct_c4));


  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{4, 2, 4, 2, 3, 2, 4, 3, 0,
      2, 0, 4, 1, 2, 0, 3, 0, 4, 1, 1, 0, 2, 3, 0, 3, 0, 2, 0, 1, 2, 1, 1, 3, 4,
      3, 3, 0, 4, 0, 2, 1, 1, 1, 1, 0, 3, 2, 1, 2, 3, 3, 4, 2, 3, 1, 1, 3, 3, 1,
      3, 4, 0, 2, 1, 1, 1, 2, 2, 3, 1, 4, 0, 1, 3, 3, 4, 4, 0, 0, 0, 4, 2, 2, 1,
      0, 3, 0, 1, 3, 2, 4, 2, 0, 2, 1, 0, 2, 1, 3, 0};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 0.03235891455004822);
}  // TEST_CASE("Elkan 100 double points 2 dimensions 5 clusters")

TEST_CASE("Elkan 10 double points 1 dimension 2 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0{-3.17915079};
  std::vector<double> d1{-2.7052052};
  std::vector<double> d2{-5.08696482};
  std::vector<double> d3{-5.39049847};
  std::vector<double> d4{8.25688251};
  std::vector<double> d5{7.44915957};
  std::vector<double> d6{-4.23879291};
  std::vector<double> d7{8.10402117};
  std::vector<double> d8{7.3047543};
  std::vector<double> d9{9.41661351};

  nda::matrix<double> features({10, 1});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));

  std::vector<double> c0{0.64950133};
  std::vector<double> c1{0.95542778};

  auto initial_centers = std::make_unique<nda::matrix<double>>(
      nda::matrix_shape<>{2, 1});
  (*initial_centers)(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  (*initial_centers)(1, nda::all).copy_elems(nda::vector_ref<double>(c1));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(2,
      std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{-4.120122438};
  std::vector<double> correct_c1{8.106286211999999};

  nda::matrix<double> correct_centers({2, 1});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));

  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{0, 0, 0, 0, 1, 1, 0, 1, 1,
                                                    1};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 0.8264051400816704);
}  // TEST_CASE("Elkan 10 double points 1 dimension 2 clusters")

TEST_CASE("Elkan 10 int points 10 dimensions 3 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{-3, -7,  6, -3,  -9, -1,  2, -8,  6, -6};
  std::vector<int8_t> d1{-5, -7,  4,  0, -10, -2, -1, -7,  7, -8};
  std::vector<int8_t> d2{-3,  4, -5,  2,  -1,  8,  7,  7, -6,  6};
  std::vector<int8_t> d3{-4, -8,  6, -1,  11,  0,  2, -8,  7, -7};
  std::vector<int8_t> d4{ 0,  4, -2,  3,  -4,  6,  6,  9, -7,  7};
  std::vector<int8_t> d5{-5, -8,  7, -1,  -9, -1,  1, -6,  7, -5};
  std::vector<int8_t> d6{-5,  9, -2, -4,  -3, -7, -9,  7,  0,  3};
  std::vector<int8_t> d7{-5, 10, -2, -3,  -2, -6, -8,  8,  1,  0};
  std::vector<int8_t> d8{-4,  7, -4, -5,  -5, -6, -9,  8,  0,  3};
  std::vector<int8_t> d9{-2,  3, -5,  3,  -3,  6,  7,  6, -6,  6};

  nda::matrix<int8_t> features({10, 10});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  std::vector<double> c0{1, 0, 0, 1, 0, 1, 1, 1, 0, 1};
  std::vector<double> c1{0, 0, 1, 0, 0, 1, 0, 1, 0, 1};
  std::vector<double> c2{0, 0, 1, 1, 1, 0, 0, 0, 0, 0};

  auto initial_centers = std::make_unique<nda::matrix<double>>(
      nda::matrix_shape<>{3, 10});
  (*initial_centers)(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  (*initial_centers)(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  (*initial_centers)(2, nda::all).copy_elems(nda::vector_ref<double>(c2));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(
      3, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{-1.6666666666666667, 3.6666666666666665, -4,
      2.6666666666666665, -2.6666666666666665, 6.666666666666667,
      6.666666666666667, 7.333333333333333, -6.333333333333333,
      6.333333333333333};
  std::vector<double> correct_c1{-4.666666666666667, 8.666666666666666,
      -2.6666666666666665, -4, -3.3333333333333335, -6.333333333333333,
      -8.666666666666666, 7.666666666666667, 0.3333333333333333, 2};
  std::vector<double> correct_c2{-4.25, -7.5, 5.75, -1.25, -4.25, -1, 1, -7.25,
      6.75, -6.5};

  nda::matrix<double> correct_centers({3, 10});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(correct_c2));

  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{2, 2, 0, 2, 0, 2, 1, 1, 1,
                                                    0};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 38.983333333333334);
}  // TEST_CASE("Elkan 10 int points 10 dimensions 3 clusters")

TEST_CASE("Elkan 10 int points (6+4 duplicates) 2 dimensions 2 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{-7,  3};
  std::vector<int8_t> d1{-7,  3};
  std::vector<int8_t> d2{-7,  3};
  std::vector<int8_t> d3{-7,  3};
  std::vector<int8_t> d4{-7,  3};
  std::vector<int8_t> d5{-7,  3};
  std::vector<int8_t> d6{10, 10};
  std::vector<int8_t> d7{10, 10};
  std::vector<int8_t> d8{10, 10};
  std::vector<int8_t> d9{10, 10};

  nda::matrix<int8_t> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  std::vector<double> c0{1, 0};
  std::vector<double> c1{0, 1};

  auto initial_centers = std::make_unique<nda::matrix<double>>(
      nda::matrix_shape<>{2, 2});
  (*initial_centers)(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  (*initial_centers)(1, nda::all).copy_elems(nda::vector_ref<double>(c1));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(
      2, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{10, 10};
  std::vector<double> correct_c1{-7,  3};

  nda::matrix<double> correct_centers({2, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));

  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{1, 1, 1, 1, 1, 1, 0, 0, 0,
                                                    0};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 0.0);
}  // TEST_CASE("Elkan 10 int points (6+4 duplicates) 2 dimensions 2 clusters")

// kmeans++ test cases generated with spreadsheets and a separate C++ program to
// determine the order of the psuedo random numbers used in InitPlusPlus

TEST_CASE("kmeans++ 10 double points 2 dimensions 3 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0{3.76861611, 3.49741106};
  std::vector<double> d1{3.32578231, 1.84147162};
  std::vector<double> d2{1.99651652, 1.71079686};
  std::vector<double> d3{2.31120319, 2.06483991};
  std::vector<double> d4{3.25797534, 4.60796172};
  std::vector<double> d5{4.72177103, 1.35825865};
  std::vector<double> d6{4.18270633, 4.46822983};
  std::vector<double> d7{2.44595585, 3.44402974};
  std::vector<double> d8{3.36567939, 1.79443539};
  std::vector<double> d9{3.88437902, 4.68319429};

  nda::matrix<double> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(3);
  k.InitPlusPlus(features, false, fishbait::Random::Seed(42));
  const nda::matrix<double>* centers = k.clusters();

  std::vector<double> c0{2.44595585, 3.44402974};
  std::vector<double> c1{1.99651652, 1.71079686};
  std::vector<double> c2{3.36567939, 1.79443539};

  nda::matrix<double> correct_centers({3, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(c2));

  REQUIRE(*centers == correct_centers);
}  // TEST_CASE("kmeans++ 10 double points 2 dimensions 3 clusters")

TEST_CASE("kmeans++ 10 int points 2 dimensions 5 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{3, 6};
  std::vector<int8_t> d1{8, 7};
  std::vector<int8_t> d2{3, 6};
  std::vector<int8_t> d3{3, 5};
  std::vector<int8_t> d4{1, 9};
  std::vector<int8_t> d5{4, 1};
  std::vector<int8_t> d6{7, 5};
  std::vector<int8_t> d7{2, 0};
  std::vector<int8_t> d8{5, 4};
  std::vector<int8_t> d9{6, 9};

  nda::matrix<int8_t> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(5);
  k.InitPlusPlus(features, false, fishbait::Random::Seed(18));
  const nda::matrix<double>* centers = k.clusters();

  std::vector<double> c0{3, 6};
  std::vector<double> c1{4, 1};
  std::vector<double> c2{8, 7};
  std::vector<double> c3{6, 9};
  std::vector<double> c4{1, 9};

  nda::matrix<double> correct_centers({5, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(c2));
  correct_centers(3, nda::all).copy_elems(nda::vector_ref<double>(c3));
  correct_centers(4, nda::all).copy_elems(nda::vector_ref<double>(c4));

  REQUIRE(*centers == correct_centers);
}  // TEST_CASE("kmeans++ 10 int points 2 dimensions 5 clusters")

TEST_CASE("kmeans++ 100 double points 2 dimensions 5 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0{ 0.65813947, 0.01196135};
  std::vector<double> d1{ 0.21775957, 0.59771208};
  std::vector<double> d2{ 0.33957053, 0.75545822};
  std::vector<double> d3{ 0.25420897, 0.3514419 };
  std::vector<double> d4{ 0.86108239, 0.47209367};
  std::vector<double> d5{ 0.25257409, 0.42296846};
  std::vector<double> d6{ 0.26536873, 0.83909654};
  std::vector<double> d7{ 0.26454276, 0.05713374};
  std::vector<double> d8{ 0.85432329, 0.70912714};
  std::vector<double> d9{ 0.4348628 , 0.21604847};
  std::vector<double> d10{0.55629254, 0.63570687};
  std::vector<double> d11{0.20241321, 0.46430921};
  std::vector<double> d12{0.74020197, 0.18458295};
  std::vector<double> d13{0.93067077, 0.39766014};
  std::vector<double> d14{0.69723806, 0.39770099};
  std::vector<double> d15{0.51647343, 0.00651109};
  std::vector<double> d16{0.46394069, 0.91128176};
  std::vector<double> d17{0.80915795, 0.28225283};
  std::vector<double> d18{0.79275706, 0.41622831};
  std::vector<double> d19{0.4654375 , 0.22042504};
  std::vector<double> d20{0.10020094, 0.89510985};
  std::vector<double> d21{0.36637472, 0.56769991};
  std::vector<double> d22{0.61520134, 0.53358243};
  std::vector<double> d23{0.01304606, 0.56288893};
  std::vector<double> d24{0.02347862, 0.30743283};
  std::vector<double> d25{0.18696441, 0.43481898};
  std::vector<double> d26{0.57810079, 0.49887558};
  std::vector<double> d27{0.20906347, 0.39142677};
  std::vector<double> d28{0.65697119, 0.64468842};
  std::vector<double> d29{0.01322537, 0.07734016};
  std::vector<double> d30{0.09459185, 0.78352758};
  std::vector<double> d31{0.56257008, 0.68820197};
  std::vector<double> d32{0.20227448, 0.1057177 };
  std::vector<double> d33{0.75739364, 0.47866531};
  std::vector<double> d34{0.78123294, 0.5951132 };
  std::vector<double> d35{0.76419582, 0.38221766};
  std::vector<double> d36{0.32771615, 0.64853292};
  std::vector<double> d37{0.03839413, 0.35086252};
  std::vector<double> d38{0.55523224, 0.62949049};
  std::vector<double> d39{0.07131521, 0.70106088};
  std::vector<double> d40{0.21382217, 0.39160925};
  std::vector<double> d41{0.17650536, 0.70006479};
  std::vector<double> d42{0.12589853, 0.42839937};
  std::vector<double> d43{0.66056332, 0.23246489};
  std::vector<double> d44{0.59042472, 0.12261709};
  std::vector<double> d45{0.05064213, 0.96807571};
  std::vector<double> d46{0.01205091, 0.05083267};
  std::vector<double> d47{0.63898561, 0.35738671};
  std::vector<double> d48{0.6717339 , 0.56754949};
  std::vector<double> d49{0.6296523 , 0.59118655};
  std::vector<double> d50{0.97959776, 0.32693262};
  std::vector<double> d51{0.66800739, 0.17288597};
  std::vector<double> d52{0.52993484, 0.18785018};
  std::vector<double> d53{0.71248509, 0.6807984 };
  std::vector<double> d54{0.91255087, 0.78544878};
  std::vector<double> d55{0.68342884, 0.47883581};
  std::vector<double> d56{0.24768688, 0.78114745};
  std::vector<double> d57{0.62087899, 0.42600688};
  std::vector<double> d58{0.73882507, 0.75871606};
  std::vector<double> d59{0.91904247, 0.06708523};
  std::vector<double> d60{0.13105943, 0.3454093 };
  std::vector<double> d61{0.64772544, 0.01357345};
  std::vector<double> d62{0.91374092, 0.39308346};
  std::vector<double> d63{0.50643009, 0.33279016};
  std::vector<double> d64{0.73110752, 0.23049449};
  std::vector<double> d65{0.16804333, 0.75561474};
  std::vector<double> d66{0.43864405, 0.18563791};
  std::vector<double> d67{0.17314321, 0.73903703};
  std::vector<double> d68{0.96375694, 0.48551134};
  std::vector<double> d69{0.80735812, 0.54323563};
  std::vector<double> d70{0.49750269, 0.08314243};
  std::vector<double> d71{0.73357908, 0.44391572};
  std::vector<double> d72{0.52264729, 0.49903343};
  std::vector<double> d73{0.37453401, 0.59155678};
  std::vector<double> d74{0.1255591 , 0.34169086};
  std::vector<double> d75{0.49014768, 0.83620028};
  std::vector<double> d76{0.98813997, 0.39967672};
  std::vector<double> d77{0.34507797, 0.81164257};
  std::vector<double> d78{0.98296234, 0.91421504};
  std::vector<double> d79{0.91186147, 0.84822329};
  std::vector<double> d80{0.8458693 , 0.14289518};
  std::vector<double> d81{0.12565588, 0.01943471};
  std::vector<double> d82{0.67921888, 0.65969092};
  std::vector<double> d83{0.25488504, 0.12292637};
  std::vector<double> d84{0.55282039, 0.3335402 };
  std::vector<double> d85{0.71025307, 0.01185484};
  std::vector<double> d86{0.22250232, 0.18785757};
  std::vector<double> d87{0.43165286, 0.95120266};
  std::vector<double> d88{0.36555605, 0.30401102};
  std::vector<double> d89{0.60637127, 0.65238694};
  std::vector<double> d90{0.0475963 , 0.47289757};
  std::vector<double> d91{0.23721612, 0.47291795};
  std::vector<double> d92{0.96841266, 0.48445047};
  std::vector<double> d93{0.25846202, 0.56380124};
  std::vector<double> d94{0.62483503, 0.42935405};
  std::vector<double> d95{0.52859724, 0.73716236};
  std::vector<double> d96{0.59571527, 0.20979482};
  std::vector<double> d97{0.88367585, 0.15743041};
  std::vector<double> d98{0.77983398, 0.48203182};
  std::vector<double> d99{0.2474461 , 0.54835603};

  nda::matrix<double> features({100, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));
  features(10, nda::all).copy_elems(nda::vector_ref<double>(d10));
  features(11, nda::all).copy_elems(nda::vector_ref<double>(d11));
  features(12, nda::all).copy_elems(nda::vector_ref<double>(d12));
  features(13, nda::all).copy_elems(nda::vector_ref<double>(d13));
  features(14, nda::all).copy_elems(nda::vector_ref<double>(d14));
  features(15, nda::all).copy_elems(nda::vector_ref<double>(d15));
  features(16, nda::all).copy_elems(nda::vector_ref<double>(d16));
  features(17, nda::all).copy_elems(nda::vector_ref<double>(d17));
  features(18, nda::all).copy_elems(nda::vector_ref<double>(d18));
  features(19, nda::all).copy_elems(nda::vector_ref<double>(d19));
  features(20, nda::all).copy_elems(nda::vector_ref<double>(d20));
  features(21, nda::all).copy_elems(nda::vector_ref<double>(d21));
  features(22, nda::all).copy_elems(nda::vector_ref<double>(d22));
  features(23, nda::all).copy_elems(nda::vector_ref<double>(d23));
  features(24, nda::all).copy_elems(nda::vector_ref<double>(d24));
  features(25, nda::all).copy_elems(nda::vector_ref<double>(d25));
  features(26, nda::all).copy_elems(nda::vector_ref<double>(d26));
  features(27, nda::all).copy_elems(nda::vector_ref<double>(d27));
  features(28, nda::all).copy_elems(nda::vector_ref<double>(d28));
  features(29, nda::all).copy_elems(nda::vector_ref<double>(d29));
  features(30, nda::all).copy_elems(nda::vector_ref<double>(d30));
  features(31, nda::all).copy_elems(nda::vector_ref<double>(d31));
  features(32, nda::all).copy_elems(nda::vector_ref<double>(d32));
  features(33, nda::all).copy_elems(nda::vector_ref<double>(d33));
  features(34, nda::all).copy_elems(nda::vector_ref<double>(d34));
  features(35, nda::all).copy_elems(nda::vector_ref<double>(d35));
  features(36, nda::all).copy_elems(nda::vector_ref<double>(d36));
  features(37, nda::all).copy_elems(nda::vector_ref<double>(d37));
  features(38, nda::all).copy_elems(nda::vector_ref<double>(d38));
  features(39, nda::all).copy_elems(nda::vector_ref<double>(d39));
  features(40, nda::all).copy_elems(nda::vector_ref<double>(d40));
  features(41, nda::all).copy_elems(nda::vector_ref<double>(d41));
  features(42, nda::all).copy_elems(nda::vector_ref<double>(d42));
  features(43, nda::all).copy_elems(nda::vector_ref<double>(d43));
  features(44, nda::all).copy_elems(nda::vector_ref<double>(d44));
  features(45, nda::all).copy_elems(nda::vector_ref<double>(d45));
  features(46, nda::all).copy_elems(nda::vector_ref<double>(d46));
  features(47, nda::all).copy_elems(nda::vector_ref<double>(d47));
  features(48, nda::all).copy_elems(nda::vector_ref<double>(d48));
  features(49, nda::all).copy_elems(nda::vector_ref<double>(d49));
  features(50, nda::all).copy_elems(nda::vector_ref<double>(d50));
  features(51, nda::all).copy_elems(nda::vector_ref<double>(d51));
  features(52, nda::all).copy_elems(nda::vector_ref<double>(d52));
  features(53, nda::all).copy_elems(nda::vector_ref<double>(d53));
  features(54, nda::all).copy_elems(nda::vector_ref<double>(d54));
  features(55, nda::all).copy_elems(nda::vector_ref<double>(d55));
  features(56, nda::all).copy_elems(nda::vector_ref<double>(d56));
  features(57, nda::all).copy_elems(nda::vector_ref<double>(d57));
  features(58, nda::all).copy_elems(nda::vector_ref<double>(d58));
  features(59, nda::all).copy_elems(nda::vector_ref<double>(d59));
  features(60, nda::all).copy_elems(nda::vector_ref<double>(d60));
  features(61, nda::all).copy_elems(nda::vector_ref<double>(d61));
  features(62, nda::all).copy_elems(nda::vector_ref<double>(d62));
  features(63, nda::all).copy_elems(nda::vector_ref<double>(d63));
  features(64, nda::all).copy_elems(nda::vector_ref<double>(d64));
  features(65, nda::all).copy_elems(nda::vector_ref<double>(d65));
  features(66, nda::all).copy_elems(nda::vector_ref<double>(d66));
  features(67, nda::all).copy_elems(nda::vector_ref<double>(d67));
  features(68, nda::all).copy_elems(nda::vector_ref<double>(d68));
  features(69, nda::all).copy_elems(nda::vector_ref<double>(d69));
  features(70, nda::all).copy_elems(nda::vector_ref<double>(d70));
  features(71, nda::all).copy_elems(nda::vector_ref<double>(d71));
  features(72, nda::all).copy_elems(nda::vector_ref<double>(d72));
  features(73, nda::all).copy_elems(nda::vector_ref<double>(d73));
  features(74, nda::all).copy_elems(nda::vector_ref<double>(d74));
  features(75, nda::all).copy_elems(nda::vector_ref<double>(d75));
  features(76, nda::all).copy_elems(nda::vector_ref<double>(d76));
  features(77, nda::all).copy_elems(nda::vector_ref<double>(d77));
  features(78, nda::all).copy_elems(nda::vector_ref<double>(d78));
  features(79, nda::all).copy_elems(nda::vector_ref<double>(d79));
  features(80, nda::all).copy_elems(nda::vector_ref<double>(d80));
  features(81, nda::all).copy_elems(nda::vector_ref<double>(d81));
  features(82, nda::all).copy_elems(nda::vector_ref<double>(d82));
  features(83, nda::all).copy_elems(nda::vector_ref<double>(d83));
  features(84, nda::all).copy_elems(nda::vector_ref<double>(d84));
  features(85, nda::all).copy_elems(nda::vector_ref<double>(d85));
  features(86, nda::all).copy_elems(nda::vector_ref<double>(d86));
  features(87, nda::all).copy_elems(nda::vector_ref<double>(d87));
  features(88, nda::all).copy_elems(nda::vector_ref<double>(d88));
  features(89, nda::all).copy_elems(nda::vector_ref<double>(d89));
  features(90, nda::all).copy_elems(nda::vector_ref<double>(d90));
  features(91, nda::all).copy_elems(nda::vector_ref<double>(d91));
  features(92, nda::all).copy_elems(nda::vector_ref<double>(d92));
  features(93, nda::all).copy_elems(nda::vector_ref<double>(d93));
  features(94, nda::all).copy_elems(nda::vector_ref<double>(d94));
  features(95, nda::all).copy_elems(nda::vector_ref<double>(d95));
  features(96, nda::all).copy_elems(nda::vector_ref<double>(d96));
  features(97, nda::all).copy_elems(nda::vector_ref<double>(d97));
  features(98, nda::all).copy_elems(nda::vector_ref<double>(d98));
  features(99, nda::all).copy_elems(nda::vector_ref<double>(d99));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(5);
  k.InitPlusPlus(features, false, fishbait::Random::Seed(78));
  const nda::matrix<double>* centers = k.clusters();

  std::vector<double> c0{0.36555605, 0.30401102};
  std::vector<double> c1{0.91904247, 0.06708523};
  std::vector<double> c2{0.73882507, 0.75871606};
  std::vector<double> c3{0.09459185, 0.78352758};
  std::vector<double> c4{0.98813997, 0.39967672};

  nda::matrix<double> correct_centers({5, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(c2));
  correct_centers(3, nda::all).copy_elems(nda::vector_ref<double>(c3));
  correct_centers(4, nda::all).copy_elems(nda::vector_ref<double>(c4));

  REQUIRE(*centers == correct_centers);
}  // TEST_CASE("kmeans++ 100 double points 2 dimensions 5 clusters")

TEST_CASE("kmeans++ 10 double points 1 dimension 2 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0{-4.6183699};
  std::vector<double> d1{5.47460556};
  std::vector<double> d2{5.43029939};
  std::vector<double> d3{6.53281476};
  std::vector<double> d4{-4.66951927};
  std::vector<double> d5{-3.53673779};
  std::vector<double> d6{6.73192642};
  std::vector<double> d7{-4.66895532};
  std::vector<double> d8{-3.05616857};
  std::vector<double> d9{6.8349517};

  nda::matrix<double> features({10, 1});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(2);
  k.InitPlusPlus(features, false, fishbait::Random::Seed(31));
  const nda::matrix<double>* centers = k.clusters();

  std::vector<double> c0{-4.6183699};
  std::vector<double> c1{5.43029939};

  nda::matrix<double> correct_centers({2, 1});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(c1));

  REQUIRE(*centers == correct_centers);
}  // TEST_CASE("kmeans++ 10 double points 1 dimension 2 clusters")

TEST_CASE("kmeans++ 10 int points 10 dimensions 3 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{-1,  -1,  -3,  -3,  -5,  -7,  -5,   1,  -7,  -9};
  std::vector<int8_t> d1{-8,  -9,   1,  -4,  -4,   1,  -6,  -9,  -4,  -2};
  std::vector<int8_t> d2{-7,  -3,   5,  -2,   5,  -3,   9,   6,   3, -11};
  std::vector<int8_t> d3{-7, -11,   2,  -7,  -3,  -1,  -6,  -8,  -4,  -2};
  std::vector<int8_t> d4{-1,  -1,  -3,  -4,  -8,  -5,  -3,   2,  -7, -12};
  std::vector<int8_t> d5{-7,  -9,   2,  -4,  -3,  -2,  -8, -12,  -4,  -1};
  std::vector<int8_t> d6{-1,  -2,  -1,  -1,  -5,  -5,  -5,   3,  -6, -11};
  std::vector<int8_t> d7{-1,  -2,  -1,  -4,  -7,  -4,  -1,   4,  -7,  -9};
  std::vector<int8_t> d8{-7,  -5,   7,  -3,   5,  -5,   9,   7,   2,  -8};
  std::vector<int8_t> d9{-8,  -5,   5,  -5,   6,  -4,   8,   7,   2,  -8};

  nda::matrix<int8_t> features({10, 10});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(3);
  k.InitPlusPlus(features, false, fishbait::Random::Seed(44));
  const nda::matrix<double>* centers = k.clusters();

  std::vector<double> c0{-1, -1, -3, -4, -8, -5, -3, 2, -7, -12};
  std::vector<double> c1{-8, -9, 1, -4, -4, 1, -6, -9, -4, -2};
  std::vector<double> c2{-7, -5, 7, -3, 5, -5, 9, 7, 2, -8};

  nda::matrix<double> correct_centers({3, 10});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(c2));

  REQUIRE(*centers == correct_centers);
}  // TEST_CASE("kmeans++ 10 int points 10 dimensions 3 clusters")

TEST_CASE("kmeans++ 10 int points (6+4 duplicates) 2 dimensions 2 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{ 4, -1};
  std::vector<int8_t> d1{ 4, -1};
  std::vector<int8_t> d2{ 4, -1};
  std::vector<int8_t> d3{ 4, -1};
  std::vector<int8_t> d4{ 4, -1};
  std::vector<int8_t> d5{ 4, -1};
  std::vector<int8_t> d6{-5,  0};
  std::vector<int8_t> d7{-5,  0};
  std::vector<int8_t> d8{-5,  0};
  std::vector<int8_t> d9{-5,  0};

  nda::matrix<int8_t> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(2);
  k.InitPlusPlus(features, false);
  const nda::matrix<double>* centers = k.clusters();

  nda::matrix<double> correct_centers_1({2, 2});
  std::vector<double> d0_d{ 4, -1};
  std::vector<double> d6_d{-5,  0};
  correct_centers_1(0, nda::all).copy_elems(nda::vector_ref<double>(d0_d));
  correct_centers_1(1, nda::all).copy_elems(nda::vector_ref<double>(d6_d));

  nda::matrix<double> correct_centers_2({2, 2});
  correct_centers_2(0, nda::all).copy_elems(nda::vector_ref<double>(d6_d));
  correct_centers_2(1, nda::all).copy_elems(nda::vector_ref<double>(d0_d));

  REQUIRE((*centers == correct_centers_1 || *centers == correct_centers_2));
}  // TEST_CASE("kmeans++ 10 int points (6+4 duplicates) 2 dimensions
   //            2 clusters") NOLINT(whitespace/indent)

TEST_CASE("RandomSumInit 10 int points sum 10", "[clustering][kmeans]") {
  std::vector<int8_t> d0{2,  8};
  std::vector<int8_t> d1{3,  7};
  std::vector<int8_t> d2{1,  9};
  std::vector<int8_t> d3{4,  6};
  std::vector<int8_t> d4{5,  5};
  std::vector<int8_t> d5{6,  4};
  std::vector<int8_t> d6{7,  3};
  std::vector<int8_t> d7{8,  2};
  std::vector<int8_t> d8{9,  1};
  std::vector<int8_t> d9{0, 10};

  nda::matrix<int8_t> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(10);
  k.RandomSumInit(features);
  const nda::matrix<double>* centers = k.clusters();

  REQUIRE(centers->rows() == 10);
  REQUIRE(centers->columns() == 2);
  for (fishbait::MeanId c = 0; c < centers->rows(); ++c) {
    double row_sum = 0;
    for (nda::index_t j = 0; j < centers->columns(); ++j) {
      row_sum += (*centers)(c, j);
    }
    REQUIRE(row_sum == Approx(10.0));
  }
}  // TEST_CASE "RandomSumInit 10 int points, sum 10"

TEST_CASE("RandomSumInit 10 double points sum 10", "[clustering][kmeans]") {
  std::vector<double> d0{2,  8};
  std::vector<double> d1{3,  7};
  std::vector<double> d2{1,  9};
  std::vector<double> d3{4,  6};
  std::vector<double> d4{5,  5};
  std::vector<double> d5{6,  4};
  std::vector<double> d6{7,  3};
  std::vector<double> d7{8,  2};
  std::vector<double> d8{9,  1};
  std::vector<double> d9{0, 10};

  nda::matrix<double> features({10, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(10);
  k.RandomSumInit(features);
  const nda::matrix<double>* centers = k.clusters();

  REQUIRE(centers->rows() == 10);
  REQUIRE(centers->columns() == 2);
  for (fishbait::MeanId c = 0; c < centers->rows(); ++c) {
    double row_sum = 0;
    for (nda::index_t j = 0; j < centers->columns(); ++j) {
      row_sum += (*centers)(c, j);
    }
    REQUIRE(row_sum == Approx(10.0));
  }
}  // TEST_CASE "RandomSumInit 10 double points, sum 10"

TEST_CASE("RandomProbInit 10 int points", "[clustering][kmeans]") {
  nda::matrix<int8_t> features({1000, 1000}, 10);

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(900);
  k.RandomProbInit(features);
  const nda::matrix<double>* centers = k.clusters();

  REQUIRE(centers->rows() == 900);
  REQUIRE(centers->columns() == 1000);
  for (fishbait::MeanId c = 0; c < centers->rows(); ++c) {
    for (nda::index_t j = 0; j < centers->columns(); ++j) {
      REQUIRE((*centers)(c, j) >= 0.0);
      REQUIRE((*centers)(c, j) < 1.0);
    }
  }
}  // TEST_CASE "RandomProbInit 10 int points"

TEST_CASE("multiple restarts 10 int points 10 dimensions 3 clusters",
          "[clustering][kmeans]") {
  std::vector<int8_t> d0{-2, -7,  6, -3,  -9, -1,  2, -8,  6,  8};
  std::vector<int8_t> d1{-5, -7,  4,  0,  10, -2, -1, -7,  7, -8};
  std::vector<int8_t> d2{-3,  4, -5,  2,  -1,  8,  7,  7, -6,  6};
  std::vector<int8_t> d3{-4, -8,  6, -1,  11,  0,  2, -8,  7, -7};
  std::vector<int8_t> d4{ 0,  4, -2,  3,  -4,  6,  6,  9,  4,  7};
  std::vector<int8_t> d5{-5, -8,  7, -1,  -9, -1,  1, -6,  7, -5};
  std::vector<int8_t> d6{-5,  9, -2, -4,  -3, -7, -9,  7,  0,  3};
  std::vector<int8_t> d7{-5, 10,  2, -3,  -2, -6, -8,  8,  1,  0};
  std::vector<int8_t> d8{-4,  7, -4, -5,  -5, -6, -9,  8,  0,  3};
  std::vector<int8_t> d9{-4,  3, -5,  3,  -3,  6,  7,  6, -6, -6};

  nda::matrix<int8_t> features({10, 10});
  features(0, nda::all).copy_elems(nda::vector_ref<int8_t>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<int8_t>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<int8_t>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<int8_t>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<int8_t>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<int8_t>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<int8_t>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<int8_t>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<int8_t>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<int8_t>(d9));

  fishbait::KMeans<int8_t, fishbait::EuclideanDistance> k(3);
  k.MultipleRestarts(features, 10, fishbait::kPlusPlus, false,
      fishbait::Random::Seed(773202));

  std::vector<double> correct_c0{-4.666666666666667, 8.666666666666666,
      -1.3333333333333333, -4.0, -3.3333333333333335, -6.333333333333333,
      -8.666666666666666, 7.666666666666667, 0.3333333333333333, 2.0};
  std::vector<double> correct_c1{-2.3333333333333335, 3.6666666666666665, -4.0,
      2.6666666666666665, -2.6666666666666665, 6.666666666666667,
      6.666666666666667, 7.333333333333333, -2.6666666666666665,
      2.3333333333333335};
  std::vector<double> correct_c2{-4.0, -7.5, 5.75, -1.25, 0.75, -1.0, 1.0,
      -7.25, 6.75, -3.0};

  nda::matrix<double> correct_centers({3, 10});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(correct_c2));

  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{2, 2, 1, 2, 1, 2, 0, 0, 0,
                                                    1};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == 81.40833333333333);
}  // TEST_CASE("multiple restarts 10 int points 10 dimensions 3 clusters")

TEST_CASE("multiple restarts 100 double points 2 dimensions 3 clusters",
          "[clustering][kmeans]") {
  std::vector<double> d0 {0.7117444656751225, 0.5183947791378705};
  std::vector<double> d1 {0.3683820048027214, 0.2758919719768448};
  std::vector<double> d2 {0.3606068333376234, 0.343701138228881 };
  std::vector<double> d3 {0.605397363905246 , 0.1812424936090702};
  std::vector<double> d4 {0.394823733567829 , 0.5192721351593128};
  std::vector<double> d5 {0.9267989822984419, 0.8376291732866885};
  std::vector<double> d6 {0.2494657345036164, 0.8792340737337748};
  std::vector<double> d7 {0.7137327860604881, 0.963436211264801 };
  std::vector<double> d8 {0.3967483489552037, 0.6604328362086366};
  std::vector<double> d9 {0.5788303723558041, 0.1567570606457652};
  std::vector<double> d10{0.3889847914805548, 0.5987040705556782};
  std::vector<double> d11{0.4183007589999629, 0.5999431317964626};
  std::vector<double> d12{0.363402538846292 , 0.5374887830505308};
  std::vector<double> d13{0.703516271208595 , 0.2553658046598557};
  std::vector<double> d14{0.3392038816991938, 0.576132634150041 };
  std::vector<double> d15{0.724910948884376 , 0.7737254520758118};
  std::vector<double> d16{0.9901463654087268, 0.6313908499110841};
  std::vector<double> d17{0.2273056041153998, 0.8932279467134294};
  std::vector<double> d18{0.8512646766658555, 0.0742652894565435};
  std::vector<double> d19{0.6973504766759098, 0.6835561750265661};
  std::vector<double> d20{0.0462928181713017, 0.7869564943748537};
  std::vector<double> d21{0.1829031491191065, 0.7898980206373674};
  std::vector<double> d22{0.6770044147942196, 0.2208140753821454};
  std::vector<double> d23{0.7805828204727754, 0.4200552713640886};
  std::vector<double> d24{0.0459343516214856, 0.4240711537925039};
  std::vector<double> d25{0.9340183835779665, 0.9371206974546742};
  std::vector<double> d26{0.7458112165746832, 0.2545000192185359};
  std::vector<double> d27{0.1933477922329649, 0.4635757908447039};
  std::vector<double> d28{0.7485134750863119, 0.5377953982480412};
  std::vector<double> d29{0.3568986929694667, 0.800531759880144 };
  std::vector<double> d30{0.1449727395912375, 0.4673522761214912};
  std::vector<double> d31{0.5831442621509152, 0.0302401074112227};
  std::vector<double> d32{0.7760349683917125, 0.9434374370752877};
  std::vector<double> d33{0.1755422136584146, 0.1143881025366336};
  std::vector<double> d34{0.1660926811128252, 0.1993804863476211};
  std::vector<double> d35{0.7118130510098497, 0.6776984302147946};
  std::vector<double> d36{0.4373271014215695, 0.1532806131048251};
  std::vector<double> d37{0.0460291703212988, 0.2430582709014697};
  std::vector<double> d38{0.945515332074665 , 0.5660148012249867};
  std::vector<double> d39{0.4459976453376328, 0.4595375284300286};
  std::vector<double> d40{0.4005218062530335, 0.9204860413607193};
  std::vector<double> d41{0.5817478892430803, 0.0985910571546714};
  std::vector<double> d42{0.9730003550203142, 0.1571217836381159};
  std::vector<double> d43{0.2571362142708927, 0.0200259211210193};
  std::vector<double> d44{0.7853007930068963, 0.6118658437247185};
  std::vector<double> d45{0.4007133556506983, 0.0086973554477714};
  std::vector<double> d46{0.2796334433275163, 0.5142614057244773};
  std::vector<double> d47{0.104780519085062 , 0.678591726687258 };
  std::vector<double> d48{0.0744652883679295, 0.8238919273047925};
  std::vector<double> d49{0.0327731487277781, 0.7328648603649979};
  std::vector<double> d50{0.4557027037085982, 0.5041393988733084};
  std::vector<double> d51{0.0997131124184311, 0.3221171661485792};
  std::vector<double> d52{0.2691412000915313, 0.2220832243796228};
  std::vector<double> d53{0.721867945214847 , 0.369287668783555 };
  std::vector<double> d54{0.9590424988540428, 0.9581663936948601};
  std::vector<double> d55{0.1738608749251775, 0.2231762496126273};
  std::vector<double> d56{0.4902420084706361, 0.0909015185739526};
  std::vector<double> d57{0.030842458784927 , 0.0665764584780152};
  std::vector<double> d58{0.2401116551508806, 0.5533436747100818};
  std::vector<double> d59{0.6881889735101843, 0.2064310207295679};
  std::vector<double> d60{0.521745053423386 , 0.6970514797167666};
  std::vector<double> d61{0.1971725579219639, 0.7604125149477484};
  std::vector<double> d62{0.026954026838871 , 0.7215396422726154};
  std::vector<double> d63{0.7843208902325638, 0.6856080204630505};
  std::vector<double> d64{0.7123574825364504, 0.0022181990391646};
  std::vector<double> d65{0.283699618054434 , 0.5969913308311623};
  std::vector<double> d66{0.6707996881740695, 0.171529393982821 };
  std::vector<double> d67{0.5408695268890366, 0.4092059836495721};
  std::vector<double> d68{0.2013658278431493, 0.4891230629310168};
  std::vector<double> d69{0.7464634644647593, 0.5205078095118937};
  std::vector<double> d70{0.5193223374990981, 0.7796635627602038};
  std::vector<double> d71{0.1447536417346845, 0.0538472899229593};
  std::vector<double> d72{0.5147106090516818, 0.3872140562620865};
  std::vector<double> d73{0.079847726128716 , 0.9433932485471429};
  std::vector<double> d74{0.4624806034764272, 0.2897054440575902};
  std::vector<double> d75{0.6727059339110086, 0.8320649052034932};
  std::vector<double> d76{0.5174703840143786, 0.629397367367882 };
  std::vector<double> d77{0.5283287931663228, 0.8956719351793212};
  std::vector<double> d78{0.8535087037787867, 0.2359592472634928};
  std::vector<double> d79{0.1291835268812417, 0.992884853932646 };
  std::vector<double> d80{0.6678326852076982, 0.7960005412945941};
  std::vector<double> d81{0.3600296609387231, 0.4209761770652614};
  std::vector<double> d82{0.6653200780895066, 0.2954459446749802};
  std::vector<double> d83{0.8531467150660482, 0.8766776324743616};
  std::vector<double> d84{0.1010530237184628, 0.2986856545628958};
  std::vector<double> d85{0.5792893928961358, 0.8176636600766691};
  std::vector<double> d86{0.3840323491226068, 0.4755325278650355};
  std::vector<double> d87{0.5372936560624523, 0.8425648499018098};
  std::vector<double> d88{0.7302695161782243, 0.1038967837396756};
  std::vector<double> d89{0.9765587192179228, 0.6383282652763711};
  std::vector<double> d90{0.9826588281343266, 0.5764182533673311};
  std::vector<double> d91{0.3359238531672544, 0.634864876434738 };
  std::vector<double> d92{0.627900487734526 , 0.3603771575308298};
  std::vector<double> d93{0.2379033733779484, 0.801714208729408 };
  std::vector<double> d94{0.0322130728520428, 0.394781510559754 };
  std::vector<double> d95{0.4864807118073925, 0.1148085616261003};
  std::vector<double> d96{0.2403717127079792, 0.8158357032467249};
  std::vector<double> d97{0.6277989691431987, 0.9500833071545923};
  std::vector<double> d98{0.157682723172287 , 0.6838146973978699};
  std::vector<double> d99{0.8738585658551963, 0.8194934229360593};

  nda::matrix<double> features({100, 2});
  features(0, nda::all).copy_elems(nda::vector_ref<double>(d0));
  features(1, nda::all).copy_elems(nda::vector_ref<double>(d1));
  features(2, nda::all).copy_elems(nda::vector_ref<double>(d2));
  features(3, nda::all).copy_elems(nda::vector_ref<double>(d3));
  features(4, nda::all).copy_elems(nda::vector_ref<double>(d4));
  features(5, nda::all).copy_elems(nda::vector_ref<double>(d5));
  features(6, nda::all).copy_elems(nda::vector_ref<double>(d6));
  features(7, nda::all).copy_elems(nda::vector_ref<double>(d7));
  features(8, nda::all).copy_elems(nda::vector_ref<double>(d8));
  features(9, nda::all).copy_elems(nda::vector_ref<double>(d9));
  features(10, nda::all).copy_elems(nda::vector_ref<double>(d10));
  features(11, nda::all).copy_elems(nda::vector_ref<double>(d11));
  features(12, nda::all).copy_elems(nda::vector_ref<double>(d12));
  features(13, nda::all).copy_elems(nda::vector_ref<double>(d13));
  features(14, nda::all).copy_elems(nda::vector_ref<double>(d14));
  features(15, nda::all).copy_elems(nda::vector_ref<double>(d15));
  features(16, nda::all).copy_elems(nda::vector_ref<double>(d16));
  features(17, nda::all).copy_elems(nda::vector_ref<double>(d17));
  features(18, nda::all).copy_elems(nda::vector_ref<double>(d18));
  features(19, nda::all).copy_elems(nda::vector_ref<double>(d19));
  features(20, nda::all).copy_elems(nda::vector_ref<double>(d20));
  features(21, nda::all).copy_elems(nda::vector_ref<double>(d21));
  features(22, nda::all).copy_elems(nda::vector_ref<double>(d22));
  features(23, nda::all).copy_elems(nda::vector_ref<double>(d23));
  features(24, nda::all).copy_elems(nda::vector_ref<double>(d24));
  features(25, nda::all).copy_elems(nda::vector_ref<double>(d25));
  features(26, nda::all).copy_elems(nda::vector_ref<double>(d26));
  features(27, nda::all).copy_elems(nda::vector_ref<double>(d27));
  features(28, nda::all).copy_elems(nda::vector_ref<double>(d28));
  features(29, nda::all).copy_elems(nda::vector_ref<double>(d29));
  features(30, nda::all).copy_elems(nda::vector_ref<double>(d30));
  features(31, nda::all).copy_elems(nda::vector_ref<double>(d31));
  features(32, nda::all).copy_elems(nda::vector_ref<double>(d32));
  features(33, nda::all).copy_elems(nda::vector_ref<double>(d33));
  features(34, nda::all).copy_elems(nda::vector_ref<double>(d34));
  features(35, nda::all).copy_elems(nda::vector_ref<double>(d35));
  features(36, nda::all).copy_elems(nda::vector_ref<double>(d36));
  features(37, nda::all).copy_elems(nda::vector_ref<double>(d37));
  features(38, nda::all).copy_elems(nda::vector_ref<double>(d38));
  features(39, nda::all).copy_elems(nda::vector_ref<double>(d39));
  features(40, nda::all).copy_elems(nda::vector_ref<double>(d40));
  features(41, nda::all).copy_elems(nda::vector_ref<double>(d41));
  features(42, nda::all).copy_elems(nda::vector_ref<double>(d42));
  features(43, nda::all).copy_elems(nda::vector_ref<double>(d43));
  features(44, nda::all).copy_elems(nda::vector_ref<double>(d44));
  features(45, nda::all).copy_elems(nda::vector_ref<double>(d45));
  features(46, nda::all).copy_elems(nda::vector_ref<double>(d46));
  features(47, nda::all).copy_elems(nda::vector_ref<double>(d47));
  features(48, nda::all).copy_elems(nda::vector_ref<double>(d48));
  features(49, nda::all).copy_elems(nda::vector_ref<double>(d49));
  features(50, nda::all).copy_elems(nda::vector_ref<double>(d50));
  features(51, nda::all).copy_elems(nda::vector_ref<double>(d51));
  features(52, nda::all).copy_elems(nda::vector_ref<double>(d52));
  features(53, nda::all).copy_elems(nda::vector_ref<double>(d53));
  features(54, nda::all).copy_elems(nda::vector_ref<double>(d54));
  features(55, nda::all).copy_elems(nda::vector_ref<double>(d55));
  features(56, nda::all).copy_elems(nda::vector_ref<double>(d56));
  features(57, nda::all).copy_elems(nda::vector_ref<double>(d57));
  features(58, nda::all).copy_elems(nda::vector_ref<double>(d58));
  features(59, nda::all).copy_elems(nda::vector_ref<double>(d59));
  features(60, nda::all).copy_elems(nda::vector_ref<double>(d60));
  features(61, nda::all).copy_elems(nda::vector_ref<double>(d61));
  features(62, nda::all).copy_elems(nda::vector_ref<double>(d62));
  features(63, nda::all).copy_elems(nda::vector_ref<double>(d63));
  features(64, nda::all).copy_elems(nda::vector_ref<double>(d64));
  features(65, nda::all).copy_elems(nda::vector_ref<double>(d65));
  features(66, nda::all).copy_elems(nda::vector_ref<double>(d66));
  features(67, nda::all).copy_elems(nda::vector_ref<double>(d67));
  features(68, nda::all).copy_elems(nda::vector_ref<double>(d68));
  features(69, nda::all).copy_elems(nda::vector_ref<double>(d69));
  features(70, nda::all).copy_elems(nda::vector_ref<double>(d70));
  features(71, nda::all).copy_elems(nda::vector_ref<double>(d71));
  features(72, nda::all).copy_elems(nda::vector_ref<double>(d72));
  features(73, nda::all).copy_elems(nda::vector_ref<double>(d73));
  features(74, nda::all).copy_elems(nda::vector_ref<double>(d74));
  features(75, nda::all).copy_elems(nda::vector_ref<double>(d75));
  features(76, nda::all).copy_elems(nda::vector_ref<double>(d76));
  features(77, nda::all).copy_elems(nda::vector_ref<double>(d77));
  features(78, nda::all).copy_elems(nda::vector_ref<double>(d78));
  features(79, nda::all).copy_elems(nda::vector_ref<double>(d79));
  features(80, nda::all).copy_elems(nda::vector_ref<double>(d80));
  features(81, nda::all).copy_elems(nda::vector_ref<double>(d81));
  features(82, nda::all).copy_elems(nda::vector_ref<double>(d82));
  features(83, nda::all).copy_elems(nda::vector_ref<double>(d83));
  features(84, nda::all).copy_elems(nda::vector_ref<double>(d84));
  features(85, nda::all).copy_elems(nda::vector_ref<double>(d85));
  features(86, nda::all).copy_elems(nda::vector_ref<double>(d86));
  features(87, nda::all).copy_elems(nda::vector_ref<double>(d87));
  features(88, nda::all).copy_elems(nda::vector_ref<double>(d88));
  features(89, nda::all).copy_elems(nda::vector_ref<double>(d89));
  features(90, nda::all).copy_elems(nda::vector_ref<double>(d90));
  features(91, nda::all).copy_elems(nda::vector_ref<double>(d91));
  features(92, nda::all).copy_elems(nda::vector_ref<double>(d92));
  features(93, nda::all).copy_elems(nda::vector_ref<double>(d93));
  features(94, nda::all).copy_elems(nda::vector_ref<double>(d94));
  features(95, nda::all).copy_elems(nda::vector_ref<double>(d95));
  features(96, nda::all).copy_elems(nda::vector_ref<double>(d96));
  features(97, nda::all).copy_elems(nda::vector_ref<double>(d97));
  features(98, nda::all).copy_elems(nda::vector_ref<double>(d98));
  features(99, nda::all).copy_elems(nda::vector_ref<double>(d99));

  fishbait::KMeans<double, fishbait::EuclideanDistance> k(3);
  k.MultipleRestarts(features, 10, fishbait::kRandomProb, false,
                     fishbait::Random::Seed(1029384));

  std::vector<double> correct_c0{0.5204443935870346, 0.19430779443165194};
  std::vector<double> correct_c1{0.22296618475567098, 0.630273664551855};
  std::vector<double> correct_c2{0.7515275770955788, 0.7385338595306439};

  nda::matrix<double> correct_centers({3, 2});
  correct_centers(0, nda::all).copy_elems(nda::vector_ref<double>(correct_c0));
  correct_centers(1, nda::all).copy_elems(nda::vector_ref<double>(correct_c1));
  correct_centers(2, nda::all).copy_elems(nda::vector_ref<double>(correct_c2));

  REQUIRE(*k.clusters() == correct_centers);

  std::vector<fishbait::MeanId> correct_assignments{2, 0, 0, 0, 1, 2, 1, 2, 1,
      0, 1, 1, 1, 0, 1, 2, 2, 1, 0, 2, 1, 1, 0, 2, 1, 2, 0, 1, 2, 1, 1, 0, 2, 0,
      0, 2, 0, 1, 2, 0, 1, 0, 0, 0, 2, 0, 1, 1, 1, 1, 1, 1, 0, 0, 2, 0, 0, 0, 1,
      0, 2, 1, 1, 2, 0, 1, 0, 0, 1, 2, 2, 0, 0, 1, 0, 2, 2, 2, 0, 1, 2, 1, 0, 2,
      1, 2, 1, 2, 0, 2, 2, 1, 0, 1, 1, 0, 1, 2, 1, 2};

  REQUIRE(*k.assignments() == correct_assignments);

  REQUIRE(k.loss() == Approx(0.05509715642422428));
}  // TEST_CASE("multiple restarts 100 double points 2 dimensions 3 clusters")
