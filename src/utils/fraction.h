// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_FRACTION_H_
#define SRC_UTILS_FRACTION_H_

#include <cstdint>
#include <algorithm>
#include <ostream>
#include <utility>

namespace utils {

class Fraction {
 public:
  /*
    @brief Constructs the fraction n/1.
  */
  explicit Fraction(int64_t n) : numerator_{n}, denominator_{1} {};

  /*
    @brief Constructs the fraction n/d.
  */
  Fraction(int64_t n, int64_t d);

  Fraction(const Fraction& other) = default;
  Fraction& operator=(const Fraction& other) = default;

  int64_t numerator() { return numerator_; }
  int64_t denominator() { return denominator_; }

  /*
    @brief Sets the fraction to its multiplicative inverse.
  */
  void Invert();

  // Addition
  Fraction& operator+=(const Fraction& rhs);
  Fraction& operator+=(int64_t rhs);

  // Subtraction
  Fraction& operator-=(const Fraction& rhs);
  Fraction& operator-=(int64_t rhs);

  // Multiplication
  Fraction& operator*=(const Fraction& rhs);
  Fraction& operator*=(int64_t rhs);

  // Division
  Fraction& operator/=(const Fraction& rhs);
  Fraction& operator/=(int64_t rhs);

  // Comparison operators
  friend bool operator==(const Fraction& lhs, const Fraction& rhs) {
    return lhs.Compare(rhs) == 0;
  }
  friend bool operator!=(const Fraction& lhs, const Fraction& rhs) {
    return lhs.Compare(rhs) != 0;
  }
  friend bool operator<(const Fraction& lhs, const Fraction& rhs) {
    return lhs.Compare(rhs) < 0;
  }
  friend bool operator>(const Fraction& lhs, const Fraction& rhs) {
    return lhs.Compare(rhs) > 0;
  }
  friend bool operator<=(const Fraction& lhs, const Fraction& rhs) {
    return lhs.Compare(rhs) <= 0;
  }
  friend bool operator>=(const Fraction& lhs, const Fraction& rhs) {
    return lhs.Compare(rhs) >= 0;
  }

  // Conversion Operators
  operator int64_t() const { return numerator_ / denominator_; }
  operator double() const {
    return static_cast<double>(numerator_) / denominator_;
  }

  // Print function
  friend std::ostream& operator<<(std::ostream &strm, const Fraction &a) {
    strm << a.numerator_ << "/" << a.denominator_;
    return strm;
  }

  /*
    @brief Calculates the greatest common divisor with an optimized Binary GCD
        algorithm from https://hbfs.wordpress.com/2013/12/10/the-speed-of-gcd/
  */
  static uint64_t GCD(uint64_t a, uint64_t b);

  /*
    @brief Calculates the least common multiple using the following fact:
        LCM(a, b) = (a x b) / GCD(a, b).
  */
  static uint64_t LCM(uint64_t a, uint64_t b) {
    return (a / GCD(a, b)) * b;
  }

 private:
  /*
    @brief Reduces the fraction assuming that the denominator is greater than 0.
  */
  void Reduce();

  /*
    @brief Consolidates the sign of the fraction to the numerator and leaves the
        denominator greater than 0.
  */
  void NumeratorSign();

  /*
    @brief Returns -1 if *this < rhs, returns 0 if *this == rhs, returns 1 if
        *this > rhs.
  */
  int Compare(const Fraction& rhs) const;

  int64_t numerator_;    // Can be negative, 0, or positive
  int64_t denominator_;  // Always greater than 0
};

// Add fractions
Fraction operator+(Fraction lhs, const Fraction& rhs);

// Add fractions and ints with each other
Fraction operator+(int64_t lhs, Fraction rhs);
Fraction operator+(Fraction lhs, int64_t rhs);

// Add fractions and doubles with each other
double operator+(double lhs, const Fraction& rhs);
double operator+(const Fraction& lhs, double rhs);

// Subtract fractions
Fraction operator-(Fraction lhs, const Fraction& rhs);

// Subtract fractions and ints with each other
Fraction operator-(int64_t lhs, Fraction rhs);
Fraction operator-(Fraction lhs, int64_t rhs);

// Subtract fractions and doubles with each other
double operator-(double lhs, const Fraction& rhs);
double operator-(const Fraction& lhs, double rhs);

// Negative fraction
Fraction operator-(Fraction f);

// Multiply fractions
Fraction operator*(Fraction lhs, const Fraction& rhs);

// Multiply fractions and ints with each other
Fraction operator*(int64_t lhs, Fraction rhs);
Fraction operator*(Fraction lhs, int64_t rhs);

// Multiply fractions and doubles with each other
double operator*(double lhs, const Fraction& rhs);
double operator*(const Fraction& lhs, double rhs);

// Divide fractions
Fraction operator/(Fraction lhs, const Fraction& rhs);

// Divide fractions and ints with each other
Fraction operator/(int64_t lhs, Fraction rhs);
Fraction operator/(Fraction lhs, int64_t rhs);

// Divide fractions and doubles with each other
double operator/(double lhs, const Fraction& rhs);
double operator/(const Fraction& lhs, double rhs);

}  // namespace utils

#endif  // SRC_UTILS_FRACTION_H_
