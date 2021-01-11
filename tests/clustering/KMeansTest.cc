// Copyright 2021 Marzuk Rashid

#include <vector>
#include <memory>
#include <utility>
#include <iostream>

#include "Catch2/single_include/catch2/catch.hpp"

#include "clustering/KMeans.h"
#include "clustering/Distance.h"
#include "utils/Matrix.h"
#include "utils/VectorView.h"

TEST_CASE("10 double points, 2 dimensions, 3 clusters",
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

  utils::Matrix<double> features(10, 2);
  features.SetRow(0, utils::VectorView(d0));
  features.SetRow(1, utils::VectorView(d1));
  features.SetRow(2, utils::VectorView(d2));
  features.SetRow(3, utils::VectorView(d3));
  features.SetRow(4, utils::VectorView(d4));
  features.SetRow(5, utils::VectorView(d5));
  features.SetRow(6, utils::VectorView(d6));
  features.SetRow(7, utils::VectorView(d7));
  features.SetRow(8, utils::VectorView(d8));
  features.SetRow(9, utils::VectorView(d9));

  std::vector<double> c0{0.6425235614350431, 0.898903293670528};
  std::vector<double> c1{0.4670259744021872, 3.2289955311358631};
  std::vector<double> c2{0.03126336690842668, 0.7057866456528065};

  auto initial_centers = std::make_unique<utils::Matrix<double>>(3, 2);
  initial_centers->SetRow(0, utils::VectorView(c0));
  initial_centers->SetRow(1, utils::VectorView(c1));
  initial_centers->SetRow(2, utils::VectorView(c2));

  clustering::KMeans<double, clustering::EuclideanDistance>
      k(3, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{1.2979202268901564, 0.4511877774581576};
  std::vector<double> correct_c1{-1.8985030548286173, 8.27933816067451};
  std::vector<double> correct_c2{-5.973232462823998, -8.291411332183563};

  utils::Matrix<double> correct_centers(3, 2);
  correct_centers.SetRow(0, utils::VectorView(correct_c0));
  correct_centers.SetRow(1, utils::VectorView(correct_c1));
  correct_centers.SetRow(2, utils::VectorView(correct_c2));


  REQUIRE(k.clusters() == correct_centers);

  std::vector<uint32_t> correct_assignments{2, 2, 0, 2, 1, 1, 1, 0, 0, 1};

  REQUIRE(k.assignments() == correct_assignments);
}  // TEST_CASE("10 double points, 2 dimensions, 3 clusters")

TEST_CASE("10 int points, 2 dimensions, 5 clusters",
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

  utils::Matrix<double> features(10, 2);
  features.SetRow(0, utils::VectorView(d0));
  features.SetRow(1, utils::VectorView(d1));
  features.SetRow(2, utils::VectorView(d2));
  features.SetRow(3, utils::VectorView(d3));
  features.SetRow(4, utils::VectorView(d4));
  features.SetRow(5, utils::VectorView(d5));
  features.SetRow(6, utils::VectorView(d6));
  features.SetRow(7, utils::VectorView(d7));
  features.SetRow(8, utils::VectorView(d8));
  features.SetRow(9, utils::VectorView(d9));

  std::vector<double> c0{4, 0};
  std::vector<double> c1{-3, -1};
  std::vector<double> c2{-5, 0};
  std::vector<double> c3{-1, -6};
  std::vector<double> c4{-11, -3};

  auto initial_centers = std::make_unique<utils::Matrix<double>>(5, 2);
  initial_centers->SetRow(0, utils::VectorView(c0));
  initial_centers->SetRow(1, utils::VectorView(c1));
  initial_centers->SetRow(2, utils::VectorView(c2));
  initial_centers->SetRow(3, utils::VectorView(c3));
  initial_centers->SetRow(4, utils::VectorView(c4));

  clustering::KMeans<double, clustering::EuclideanDistance>
      k(5, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{4.5, 2};
  std::vector<double> correct_c1{-3, -1};
  std::vector<double> correct_c2{-4, 1.5};
  std::vector<double> correct_c3{3.3333333333333335, -6.333333333333333};
  std::vector<double> correct_c4{-10.5, -5};

  utils::Matrix<double> correct_centers(5, 2);
  correct_centers.SetRow(0, utils::VectorView(correct_c0));
  correct_centers.SetRow(1, utils::VectorView(correct_c1));
  correct_centers.SetRow(2, utils::VectorView(correct_c2));
  correct_centers.SetRow(3, utils::VectorView(correct_c3));
  correct_centers.SetRow(4, utils::VectorView(correct_c4));


  REQUIRE(k.clusters() == correct_centers);

  std::vector<uint32_t> correct_assignments{0, 4, 0, 2, 3, 1, 4, 3, 3, 2};

  REQUIRE(k.assignments() == correct_assignments);
}  // TEST_CASE("10 int points, 2 dimensions, 5 clusters")

TEST_CASE("100 double points, 2 dimensions, 5 clusters",
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

  utils::Matrix<double> features(100, 2);
  features.SetRow(0, utils::VectorView(d0));
  features.SetRow(1, utils::VectorView(d1));
  features.SetRow(2, utils::VectorView(d2));
  features.SetRow(3, utils::VectorView(d3));
  features.SetRow(4, utils::VectorView(d4));
  features.SetRow(5, utils::VectorView(d5));
  features.SetRow(6, utils::VectorView(d6));
  features.SetRow(7, utils::VectorView(d7));
  features.SetRow(8, utils::VectorView(d8));
  features.SetRow(9, utils::VectorView(d9));
  features.SetRow(10, utils::VectorView(d10));
  features.SetRow(11, utils::VectorView(d11));
  features.SetRow(12, utils::VectorView(d12));
  features.SetRow(13, utils::VectorView(d13));
  features.SetRow(14, utils::VectorView(d14));
  features.SetRow(15, utils::VectorView(d15));
  features.SetRow(16, utils::VectorView(d16));
  features.SetRow(17, utils::VectorView(d17));
  features.SetRow(18, utils::VectorView(d18));
  features.SetRow(19, utils::VectorView(d19));
  features.SetRow(20, utils::VectorView(d20));
  features.SetRow(21, utils::VectorView(d21));
  features.SetRow(22, utils::VectorView(d22));
  features.SetRow(23, utils::VectorView(d23));
  features.SetRow(24, utils::VectorView(d24));
  features.SetRow(25, utils::VectorView(d25));
  features.SetRow(26, utils::VectorView(d26));
  features.SetRow(27, utils::VectorView(d27));
  features.SetRow(28, utils::VectorView(d28));
  features.SetRow(29, utils::VectorView(d29));
  features.SetRow(30, utils::VectorView(d30));
  features.SetRow(31, utils::VectorView(d31));
  features.SetRow(32, utils::VectorView(d32));
  features.SetRow(33, utils::VectorView(d33));
  features.SetRow(34, utils::VectorView(d34));
  features.SetRow(35, utils::VectorView(d35));
  features.SetRow(36, utils::VectorView(d36));
  features.SetRow(37, utils::VectorView(d37));
  features.SetRow(38, utils::VectorView(d38));
  features.SetRow(39, utils::VectorView(d39));
  features.SetRow(40, utils::VectorView(d40));
  features.SetRow(41, utils::VectorView(d41));
  features.SetRow(42, utils::VectorView(d42));
  features.SetRow(43, utils::VectorView(d43));
  features.SetRow(44, utils::VectorView(d44));
  features.SetRow(45, utils::VectorView(d45));
  features.SetRow(46, utils::VectorView(d46));
  features.SetRow(47, utils::VectorView(d47));
  features.SetRow(48, utils::VectorView(d48));
  features.SetRow(49, utils::VectorView(d49));
  features.SetRow(50, utils::VectorView(d50));
  features.SetRow(51, utils::VectorView(d51));
  features.SetRow(52, utils::VectorView(d52));
  features.SetRow(53, utils::VectorView(d53));
  features.SetRow(54, utils::VectorView(d54));
  features.SetRow(55, utils::VectorView(d55));
  features.SetRow(56, utils::VectorView(d56));
  features.SetRow(57, utils::VectorView(d57));
  features.SetRow(58, utils::VectorView(d58));
  features.SetRow(59, utils::VectorView(d59));
  features.SetRow(60, utils::VectorView(d60));
  features.SetRow(61, utils::VectorView(d61));
  features.SetRow(62, utils::VectorView(d62));
  features.SetRow(63, utils::VectorView(d63));
  features.SetRow(64, utils::VectorView(d64));
  features.SetRow(65, utils::VectorView(d65));
  features.SetRow(66, utils::VectorView(d66));
  features.SetRow(67, utils::VectorView(d67));
  features.SetRow(68, utils::VectorView(d68));
  features.SetRow(69, utils::VectorView(d69));
  features.SetRow(70, utils::VectorView(d70));
  features.SetRow(71, utils::VectorView(d71));
  features.SetRow(72, utils::VectorView(d72));
  features.SetRow(73, utils::VectorView(d73));
  features.SetRow(74, utils::VectorView(d74));
  features.SetRow(75, utils::VectorView(d75));
  features.SetRow(76, utils::VectorView(d76));
  features.SetRow(77, utils::VectorView(d77));
  features.SetRow(78, utils::VectorView(d78));
  features.SetRow(79, utils::VectorView(d79));
  features.SetRow(80, utils::VectorView(d80));
  features.SetRow(81, utils::VectorView(d81));
  features.SetRow(82, utils::VectorView(d82));
  features.SetRow(83, utils::VectorView(d83));
  features.SetRow(84, utils::VectorView(d84));
  features.SetRow(85, utils::VectorView(d85));
  features.SetRow(86, utils::VectorView(d86));
  features.SetRow(87, utils::VectorView(d87));
  features.SetRow(88, utils::VectorView(d88));
  features.SetRow(89, utils::VectorView(d89));
  features.SetRow(90, utils::VectorView(d90));
  features.SetRow(91, utils::VectorView(d91));
  features.SetRow(92, utils::VectorView(d92));
  features.SetRow(93, utils::VectorView(d93));
  features.SetRow(94, utils::VectorView(d94));
  features.SetRow(95, utils::VectorView(d95));
  features.SetRow(96, utils::VectorView(d96));
  features.SetRow(97, utils::VectorView(d97));
  features.SetRow(98, utils::VectorView(d98));
  features.SetRow(99, utils::VectorView(d99));

  std::vector<double> c0{0.4348808 , 0.87695909};
  std::vector<double> c1{0.01437716, 0.69738476};
  std::vector<double> c2{0.56699846, 0.2066689 };
  std::vector<double> c3{0.92054913, 0.08922499};
  std::vector<double> c4{0.49137035, 0.48091827};

  auto initial_centers = std::make_unique<utils::Matrix<double>>(5, 2);
  initial_centers->SetRow(0, utils::VectorView(c0));
  initial_centers->SetRow(1, utils::VectorView(c1));
  initial_centers->SetRow(2, utils::VectorView(c2));
  initial_centers->SetRow(3, utils::VectorView(c3));
  initial_centers->SetRow(4, utils::VectorView(c4));

  clustering::KMeans<double, clustering::EuclideanDistance>
      k(5, std::move(initial_centers));
  k.Elkan(features);

  std::vector<double> correct_c0{0.7148766666666666 , 0.8232170028571429 };
  std::vector<double> correct_c1{0.18534709565217392, 0.7039716665217391 };
  std::vector<double> correct_c2{0.2941162019047619 , 0.13788537095238096};
  std::vector<double> correct_c3{0.8068981166666667 , 0.2085375242857143 };
  std::vector<double> correct_c4{0.5530856528571428 , 0.5379240435714286 };

  utils::Matrix<double> correct_centers(5, 2);
  correct_centers.SetRow(0, utils::VectorView(correct_c0));
  correct_centers.SetRow(1, utils::VectorView(correct_c1));
  correct_centers.SetRow(2, utils::VectorView(correct_c2));
  correct_centers.SetRow(3, utils::VectorView(correct_c3));
  correct_centers.SetRow(4, utils::VectorView(correct_c4));


  REQUIRE(k.clusters() == correct_centers);

  std::vector<uint32_t> correct_assignments{4, 2, 4, 2, 3, 2, 4, 3, 0, 2, 0, 4,
      1, 2, 0, 3, 0, 4, 1, 1, 0, 2, 3, 0, 3, 0, 2, 0, 1, 2, 1, 1, 3, 4, 3, 3, 0,
      4, 0, 2, 1, 1, 1, 1, 0, 3, 2, 1, 2, 3, 3, 4, 2, 3, 1, 1, 3, 3, 1, 3, 4, 0,
      2, 1, 1, 1, 2, 2, 3, 1, 4, 0, 1, 3, 3, 4, 4, 0, 0, 0, 4, 2, 2, 1, 0, 3, 0,
      1, 3, 2, 4, 2, 0, 2, 1, 0, 2, 1, 3, 0};

  REQUIRE(k.assignments() == correct_assignments);
}  // TEST_CASE("100 double points, 2 dimensions, 5 clusters")
