// Copyright 2021 Marzuk Rashid

#include "utils/fraction.h"

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace utils {

/*
  ----------------------------------------------------------------------------
  Public Members -------------------------------------------------------------
  ----------------------------------------------------------------------------
*/

Fraction::Fraction(int64_t n, int64_t d) : numerator_{n}, denominator_{d} {
  if (denominator_ == 0) {
    throw std::invalid_argument("Fraction cannot have a denominator of 0");
  } else if (numerator_ == 0) {
    denominator_ = 1;
  } else {
    NumeratorSign();
    Reduce();
  }
}  // Fraction()

void Fraction::Invert() {
  std::swap(numerator_, denominator_);
  NumeratorSign();
}

// Add fractions
Fraction& Fraction::operator+=(const Fraction& rhs) {
  numerator_ = numerator_ * rhs.denominator_ + rhs.numerator_ * denominator_;
  denominator_ *= rhs.denominator_;
  Reduce();
  return *this;
}
Fraction operator+(Fraction lhs, const Fraction& rhs) {
  lhs += rhs;
  return lhs;
}

// Add fractions and ints with each other
Fraction& Fraction::operator+=(int64_t rhs) {
  numerator_ += rhs * denominator_;
  Reduce();
  return *this;
}
Fraction operator+(int64_t lhs, Fraction rhs) {
  rhs += lhs;
  return rhs;
}
Fraction operator+(Fraction lhs, int64_t rhs) {
  lhs += rhs;
  return lhs;
}

// Add fractions and doubles with each other
double operator+(double lhs, const Fraction& rhs) {
  return lhs + static_cast<double>(rhs);
}
double operator+(const Fraction& lhs, double rhs) {
  return static_cast<double>(lhs) + rhs;
}

// Subtract fractions
Fraction& Fraction::operator-=(const Fraction& rhs) {
  numerator_ = numerator_ * rhs.denominator_ - rhs.numerator_ * denominator_;
  denominator_ *= rhs.denominator_;
  Reduce();
  return *this;
}
Fraction operator-(Fraction lhs, const Fraction& rhs) {
  lhs -= rhs;
  return lhs;
}

// Subtract fractions and ints with each other
Fraction& Fraction::operator-=(int64_t rhs) {
  numerator_ -= rhs * denominator_;
  Reduce();
  return *this;
}
Fraction operator-(int64_t lhs, Fraction rhs) {
  rhs *= -1;
  rhs += lhs;
  return rhs;
}
Fraction operator-(Fraction lhs, int64_t rhs) {
  lhs -= rhs;
  return lhs;
}

// Subtract fractions and doubles with each other
double operator-(double lhs, const Fraction& rhs) {
  return lhs - static_cast<double>(rhs);
}
double operator-(const Fraction& lhs, double rhs) {
  return static_cast<double>(lhs) - rhs;
}

// Negative fraction
Fraction operator-(Fraction f) {
  f *= -1;
  return f;
}

// Multiply fractions
Fraction& Fraction::operator*=(const Fraction& rhs) {
  numerator_ *= rhs.numerator_;
  denominator_ *= rhs.denominator_;
  Reduce();
  return *this;
}
Fraction operator*(Fraction lhs, const Fraction& rhs) {
  lhs *= rhs;
  return lhs;
}

// Multiply fractions and ints with each other
Fraction& Fraction::operator*=(int64_t rhs) {
  numerator_ *= rhs;
  if (rhs != 1 && rhs != -1) Reduce();
  return *this;
}
Fraction operator*(int64_t lhs, Fraction rhs) {
  rhs *= lhs;
  return rhs;
}
Fraction operator*(Fraction lhs, int64_t rhs) {
  lhs *= rhs;
  return lhs;
}

// Multiply fractions and doubles with each other
double operator*(double lhs, const Fraction& rhs) {
  return lhs * static_cast<double>(rhs);
}
double operator*(const Fraction& lhs, double rhs) {
  return static_cast<double>(lhs) * rhs;
}

// Divide fractions
Fraction& Fraction::operator/=(const Fraction& rhs) {
  numerator_ *= rhs.denominator_;
  denominator_ *= rhs.numerator_;
  NumeratorSign();
  Reduce();
  return *this;
}
Fraction operator/(Fraction lhs, const Fraction& rhs) {
  lhs /= rhs;
  return lhs;
}

// Divide fractions and ints with each other
Fraction& Fraction::operator/=(int64_t rhs) {
  if (rhs == 1 || rhs == -1) {
    numerator_ *= rhs;
    return *this;
  }
  denominator_ *= rhs;
  NumeratorSign();
  Reduce();
  return *this;
}
Fraction operator/(int64_t lhs, Fraction rhs) {
  rhs.Invert();
  rhs *= lhs;
  return rhs;
}
Fraction operator/(Fraction lhs, int64_t rhs) {
  lhs /= rhs;
  return lhs;
}

// Divide fractions and doubles with each other
double operator/(double lhs, const Fraction& rhs) {
  return lhs / static_cast<double>(rhs);
}
double operator/(const Fraction& lhs, double rhs) {
  return static_cast<double>(lhs) / rhs;
}

uint64_t Fraction::GCD(uint64_t a, uint64_t b) {
  if (a == 0) return b;
  if (b == 0) return a;

  int shift = __builtin_ctzll(a | b);
  a >>= __builtin_ctzll(a);

  do {
    b >>= __builtin_ctzll(b);

    if (a > b) std::swap(a, b);
    b -= a;
  } while (b);

  return a << shift;
}  // GCD()

/*
  ----------------------------------------------------------------------------
  Private Members ------------------------------------------------------------
  ----------------------------------------------------------------------------
*/

void Fraction::Reduce() {
  int sign = numerator_ >= 0 ? 1 : -1;
  numerator_ = std::abs(numerator_);
  int64_t tmp = GCD(numerator_, denominator_);
  numerator_ = numerator_/tmp*sign;
  denominator_ = denominator_/tmp;
}

void Fraction::NumeratorSign() {
  int d_sign = denominator_ > 0 ? 1 : -1;
  numerator_ *= d_sign;
  denominator_ = std::abs(denominator_);
}

int Fraction::Compare(const Fraction& rhs) const {
  if (numerator_ == rhs.numerator_ && denominator_ == rhs.denominator_) {
    return 0;
  } else if (numerator_ * rhs.denominator_ < rhs.numerator_ * denominator_) {
    return -1;
  } else {
    return 1;
  }
}

}  // namespace utils
