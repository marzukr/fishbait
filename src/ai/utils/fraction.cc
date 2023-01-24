#include "utils/fraction.h"

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace fishbait {

/*
  ----------------------------------------------------------------------------
  Public Members -------------------------------------------------------------
  ----------------------------------------------------------------------------
*/

Fraction::Fraction(int64_t n, int64_t d) : numerator_{n}, denominator_{d} {
  if (denominator_ == 0) {
    throw std::invalid_argument("Fraction cannot have a denominator of 0.");
  } else if (numerator_ == 0) {
    denominator_ = 1;
  } else {
    NumeratorSign();
    Reduce();
  }
}  // Fraction()

void Fraction::Invert() {
  if (numerator_ == 0) {
    throw std::domain_error("Multiplicative inverse of 0 is undefined.");
  }
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
template <typename T> Fraction& Fraction::operator+=(T rhs) {
  numerator_ += rhs * denominator_;
  Reduce();
  return *this;
}
template Fraction& Fraction::operator+=<int8_t>(int8_t);
template Fraction& Fraction::operator+=<int16_t>(int16_t);
template Fraction& Fraction::operator+=<int32_t>(int32_t);
template Fraction& Fraction::operator+=<int64_t>(int64_t);
template Fraction& Fraction::operator+=<uint8_t>(uint8_t);
template Fraction& Fraction::operator+=<uint16_t>(uint16_t);
template Fraction& Fraction::operator+=<uint32_t>(uint32_t);
template <typename T> Fraction operator+(T lhs, Fraction rhs) {
  rhs += lhs;
  return rhs;
}
template Fraction operator+<int8_t>(int8_t, Fraction);
template Fraction operator+<int16_t>(int16_t, Fraction);
template Fraction operator+<int32_t>(int32_t, Fraction);
template Fraction operator+<int64_t>(int64_t, Fraction);
template Fraction operator+<uint8_t>(uint8_t, Fraction);
template Fraction operator+<uint16_t>(uint16_t, Fraction);
template Fraction operator+<uint32_t>(uint32_t, Fraction);
template <typename T> Fraction operator+(Fraction lhs, T rhs) {
  lhs += rhs;
  return lhs;
}
template Fraction operator+<int8_t>(Fraction, int8_t);
template Fraction operator+<int16_t>(Fraction, int16_t);
template Fraction operator+<int32_t>(Fraction, int32_t);
template Fraction operator+<int64_t>(Fraction, int64_t);
template Fraction operator+<uint8_t>(Fraction, uint8_t);
template Fraction operator+<uint16_t>(Fraction, uint16_t);
template Fraction operator+<uint32_t>(Fraction, uint32_t);

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
template <typename T> Fraction& Fraction::operator-=(T rhs) {
  numerator_ -= rhs * denominator_;
  Reduce();
  return *this;
}
template Fraction& Fraction::operator-=<int8_t>(int8_t);
template Fraction& Fraction::operator-=<int16_t>(int16_t);
template Fraction& Fraction::operator-=<int32_t>(int32_t);
template Fraction& Fraction::operator-=<int64_t>(int64_t);
template Fraction& Fraction::operator-=<uint8_t>(uint8_t);
template Fraction& Fraction::operator-=<uint16_t>(uint16_t);
template Fraction& Fraction::operator-=<uint32_t>(uint32_t);
template <typename T> Fraction operator-(T lhs, Fraction rhs) {
  rhs *= -1;
  rhs += lhs;
  return rhs;
}
template Fraction operator-<int8_t>(int8_t, Fraction);
template Fraction operator-<int16_t>(int16_t, Fraction);
template Fraction operator-<int32_t>(int32_t, Fraction);
template Fraction operator-<int64_t>(int64_t, Fraction);
template Fraction operator-<uint8_t>(uint8_t, Fraction);
template Fraction operator-<uint16_t>(uint16_t, Fraction);
template Fraction operator-<uint32_t>(uint32_t, Fraction);
template <typename T> Fraction operator-(Fraction lhs, T rhs) {
  lhs -= rhs;
  return lhs;
}
template Fraction operator-<int8_t>(Fraction, int8_t);
template Fraction operator-<int16_t>(Fraction, int16_t);
template Fraction operator-<int32_t>(Fraction, int32_t);
template Fraction operator-<int64_t>(Fraction, int64_t);
template Fraction operator-<uint8_t>(Fraction, uint8_t);
template Fraction operator-<uint16_t>(Fraction, uint16_t);
template Fraction operator-<uint32_t>(Fraction, uint32_t);

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
template <typename T> Fraction& Fraction::operator*=(T rhs) {
  numerator_ *= rhs;
  Reduce();
  return *this;
}
template Fraction& Fraction::operator*=<int8_t>(int8_t);
template Fraction& Fraction::operator*=<int16_t>(int16_t);
template Fraction& Fraction::operator*=<int32_t>(int32_t);
template Fraction& Fraction::operator*=<int64_t>(int64_t);
template Fraction& Fraction::operator*=<uint8_t>(uint8_t);
template Fraction& Fraction::operator*=<uint16_t>(uint16_t);
template Fraction& Fraction::operator*=<uint32_t>(uint32_t);
template <typename T> Fraction operator*(T lhs, Fraction rhs) {
  rhs *= lhs;
  return rhs;
}
template Fraction operator*<int8_t>(int8_t, Fraction);
template Fraction operator*<int16_t>(int16_t, Fraction);
template Fraction operator*<int32_t>(int32_t, Fraction);
template Fraction operator*<int64_t>(int64_t, Fraction);
template Fraction operator*<uint8_t>(uint8_t, Fraction);
template Fraction operator*<uint16_t>(uint16_t, Fraction);
template Fraction operator*<uint32_t>(uint32_t, Fraction);
template <typename T> Fraction operator*(Fraction lhs, T rhs) {
  lhs *= rhs;
  return lhs;
}
template Fraction operator*<int8_t>(Fraction, int8_t);
template Fraction operator*<int16_t>(Fraction, int16_t);
template Fraction operator*<int32_t>(Fraction, int32_t);
template Fraction operator*<int64_t>(Fraction, int64_t);
template Fraction operator*<uint8_t>(Fraction, uint8_t);
template Fraction operator*<uint16_t>(Fraction, uint16_t);
template Fraction operator*<uint32_t>(Fraction, uint32_t);

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
template <typename T> Fraction& Fraction::operator/=(T rhs) {
  denominator_ *= rhs;
  NumeratorSign();
  Reduce();
  return *this;
}
template Fraction& Fraction::operator/=<int8_t>(int8_t);
template Fraction& Fraction::operator/=<int16_t>(int16_t);
template Fraction& Fraction::operator/=<int32_t>(int32_t);
template Fraction& Fraction::operator/=<int64_t>(int64_t);
template Fraction& Fraction::operator/=<uint8_t>(uint8_t);
template Fraction& Fraction::operator/=<uint16_t>(uint16_t);
template Fraction& Fraction::operator/=<uint32_t>(uint32_t);
template <typename T> Fraction operator/(T lhs, Fraction rhs) {
  rhs.Invert();
  rhs *= lhs;
  return rhs;
}
template Fraction operator/<int8_t>(int8_t, Fraction);
template Fraction operator/<int16_t>(int16_t, Fraction);
template Fraction operator/<int32_t>(int32_t, Fraction);
template Fraction operator/<int64_t>(int64_t, Fraction);
template Fraction operator/<uint8_t>(uint8_t, Fraction);
template Fraction operator/<uint16_t>(uint16_t, Fraction);
template Fraction operator/<uint32_t>(uint32_t, Fraction);
template <typename T> Fraction operator/(Fraction lhs, T rhs) {
  lhs /= rhs;
  return lhs;
}
template Fraction operator/<int8_t>(Fraction, int8_t);
template Fraction operator/<int16_t>(Fraction, int16_t);
template Fraction operator/<int32_t>(Fraction, int32_t);
template Fraction operator/<int64_t>(Fraction, int64_t);
template Fraction operator/<uint8_t>(Fraction, uint8_t);
template Fraction operator/<uint16_t>(Fraction, uint16_t);
template Fraction operator/<uint32_t>(Fraction, uint32_t);

// Divide fractions and doubles with each other
double operator/(double lhs, const Fraction& rhs) {
  return lhs / static_cast<double>(rhs);
}
double operator/(const Fraction& lhs, double rhs) {
  return static_cast<double>(lhs) / rhs;
}

// Equality operators
bool operator==(const Fraction& lhs, const Fraction& rhs) {
  return lhs.Compare(rhs) == 0;
}
template <typename T> bool operator==(const Fraction& lhs, T rhs) {
  return lhs.Compare(rhs) == 0;
}
template bool operator==<int8_t>(const Fraction&, int8_t);
template bool operator==<int16_t>(const Fraction&, int16_t);
template bool operator==<int32_t>(const Fraction&, int32_t);
template bool operator==<int64_t>(const Fraction&, int64_t);
template bool operator==<uint8_t>(const Fraction&, uint8_t);
template bool operator==<uint16_t>(const Fraction&, uint16_t);
template bool operator==<uint32_t>(const Fraction&, uint32_t);
template <typename T> bool operator==(T lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) == 0;
}
template bool operator==<int8_t>(int8_t, const Fraction&);
template bool operator==<int16_t>(int16_t, const Fraction&);
template bool operator==<int32_t>(int32_t, const Fraction&);
template bool operator==<int64_t>(int64_t, const Fraction&);
template bool operator==<uint8_t>(uint8_t, const Fraction&);
template bool operator==<uint16_t>(uint16_t, const Fraction&);
template bool operator==<uint32_t>(uint32_t, const Fraction&);
bool operator==(const Fraction& lhs, double rhs) {
  return lhs.Compare(rhs) == 0;
}
bool operator==(double lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) == 0;
}

// Inequality Operators
bool operator!=(const Fraction& lhs, const Fraction& rhs) {
  return lhs.Compare(rhs) != 0;
}
template <typename T> bool operator!=(const Fraction& lhs, T rhs) {
  return lhs.Compare(rhs) != 0;
}
template bool operator!=<int8_t>(const Fraction&, int8_t);
template bool operator!=<int16_t>(const Fraction&, int16_t);
template bool operator!=<int32_t>(const Fraction&, int32_t);
template bool operator!=<int64_t>(const Fraction&, int64_t);
template bool operator!=<uint8_t>(const Fraction&, uint8_t);
template bool operator!=<uint16_t>(const Fraction&, uint16_t);
template bool operator!=<uint32_t>(const Fraction&, uint32_t);
template <typename T> bool operator!=(T lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) != 0;
}
template bool operator!=<int8_t>(int8_t, const Fraction&);
template bool operator!=<int16_t>(int16_t, const Fraction&);
template bool operator!=<int32_t>(int32_t, const Fraction&);
template bool operator!=<int64_t>(int64_t, const Fraction&);
template bool operator!=<uint8_t>(uint8_t, const Fraction&);
template bool operator!=<uint16_t>(uint16_t, const Fraction&);
template bool operator!=<uint32_t>(uint32_t, const Fraction&);
bool operator!=(const Fraction& lhs, double rhs) {
  return lhs.Compare(rhs) != 0;
}
bool operator!=(double lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) != 0;
}

// Less than Operators
bool operator<(const Fraction& lhs, const Fraction& rhs) {
  return lhs.Compare(rhs) < 0;
}
template <typename T> bool operator<(const Fraction& lhs, T rhs) {
  return lhs.Compare(rhs) < 0;
}
template bool operator< <int8_t>(const Fraction&, int8_t);
template bool operator< <int16_t>(const Fraction&, int16_t);
template bool operator< <int32_t>(const Fraction&, int32_t);
template bool operator< <int64_t>(const Fraction&, int64_t);
template bool operator< <uint8_t>(const Fraction&, uint8_t);
template bool operator< <uint16_t>(const Fraction&, uint16_t);
template bool operator< <uint32_t>(const Fraction&, uint32_t);
template <typename T> bool operator<(T lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) > 0;
}
template bool operator< <int8_t>(int8_t, const Fraction&);
template bool operator< <int16_t>(int16_t, const Fraction&);
template bool operator< <int32_t>(int32_t, const Fraction&);
template bool operator< <int64_t>(int64_t, const Fraction&);
template bool operator< <uint8_t>(uint8_t, const Fraction&);
template bool operator< <uint16_t>(uint16_t, const Fraction&);
template bool operator< <uint32_t>(uint32_t, const Fraction&);
bool operator<(const Fraction& lhs, double rhs) {
  return lhs.Compare(rhs) < 0;
}
bool operator<(double lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) > 0;
}

// Greater than Operators
bool operator>(const Fraction& lhs, const Fraction& rhs) {
  return lhs.Compare(rhs) > 0;
}
template <typename T> bool operator>(const Fraction& lhs, T rhs) {
  return lhs.Compare(rhs) > 0;
}
template bool operator> <int8_t>(const Fraction&, int8_t);
template bool operator> <int16_t>(const Fraction&, int16_t);
template bool operator> <int32_t>(const Fraction&, int32_t);
template bool operator> <int64_t>(const Fraction&, int64_t);
template bool operator> <uint8_t>(const Fraction&, uint8_t);
template bool operator> <uint16_t>(const Fraction&, uint16_t);
template bool operator> <uint32_t>(const Fraction&, uint32_t);
template <typename T> bool operator>(T lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) < 0;
}
template bool operator> <int8_t>(int8_t, const Fraction&);
template bool operator> <int16_t>(int16_t, const Fraction&);
template bool operator> <int32_t>(int32_t, const Fraction&);
template bool operator> <int64_t>(int64_t, const Fraction&);
template bool operator> <uint8_t>(uint8_t, const Fraction&);
template bool operator> <uint16_t>(uint16_t, const Fraction&);
template bool operator> <uint32_t>(uint32_t, const Fraction&);
bool operator>(const Fraction& lhs, double rhs) {
  return lhs.Compare(rhs) > 0;
}
bool operator>(double lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) < 0;
}

// Less than or equal to Operators
bool operator<=(const Fraction& lhs, const Fraction& rhs) {
  return lhs.Compare(rhs) <= 0;
}
template <typename T> bool operator<=(const Fraction& lhs, T rhs) {
  return lhs.Compare(rhs) <= 0;
}
template bool operator<= <int8_t>(const Fraction&, int8_t);
template bool operator<= <int16_t>(const Fraction&, int16_t);
template bool operator<= <int32_t>(const Fraction&, int32_t);
template bool operator<= <int64_t>(const Fraction&, int64_t);
template bool operator<= <uint8_t>(const Fraction&, uint8_t);
template bool operator<= <uint16_t>(const Fraction&, uint16_t);
template bool operator<= <uint32_t>(const Fraction&, uint32_t);
template <typename T> bool operator<=(T lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) >= 0;
}
template bool operator<= <int8_t>(int8_t, const Fraction&);
template bool operator<= <int16_t>(int16_t, const Fraction&);
template bool operator<= <int32_t>(int32_t, const Fraction&);
template bool operator<= <int64_t>(int64_t, const Fraction&);
template bool operator<= <uint8_t>(uint8_t, const Fraction&);
template bool operator<= <uint16_t>(uint16_t, const Fraction&);
template bool operator<= <uint32_t>(uint32_t, const Fraction&);
bool operator<=(const Fraction& lhs, double rhs) {
  return lhs.Compare(rhs) <= 0;
}
bool operator<=(double lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) >= 0;
}

// Greater than or equal to Operators
bool operator>=(const Fraction& lhs, const Fraction& rhs) {
  return lhs.Compare(rhs) >= 0;
}
template <typename T> bool operator>=(const Fraction& lhs, T rhs) {
  return lhs.Compare(rhs) >= 0;
}
template bool operator>= <int8_t>(const Fraction&, int8_t);
template bool operator>= <int16_t>(const Fraction&, int16_t);
template bool operator>= <int32_t>(const Fraction&, int32_t);
template bool operator>= <int64_t>(const Fraction&, int64_t);
template bool operator>= <uint8_t>(const Fraction&, uint8_t);
template bool operator>= <uint16_t>(const Fraction&, uint16_t);
template bool operator>= <uint32_t>(const Fraction&, uint32_t);
template <typename T> bool operator>=(T lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) <= 0;
}
template bool operator>= <int8_t>(int8_t, const Fraction&);
template bool operator>= <int16_t>(int16_t, const Fraction&);
template bool operator>= <int32_t>(int32_t, const Fraction&);
template bool operator>= <int64_t>(int64_t, const Fraction&);
template bool operator>= <uint8_t>(uint8_t, const Fraction&);
template bool operator>= <uint16_t>(uint16_t, const Fraction&);
template bool operator>= <uint32_t>(uint32_t, const Fraction&);
bool operator>=(const Fraction& lhs, double rhs) {
  return lhs.Compare(rhs) >= 0;
}
bool operator>=(double lhs, const Fraction& rhs) {
  return rhs.Compare(lhs) <= 0;
}

// Integer conversion operator
template <typename T> Fraction::operator T() const {
  return numerator_ / denominator_;
}
template Fraction::operator int8_t() const;
template Fraction::operator int16_t() const;
template Fraction::operator int32_t() const;
template Fraction::operator int64_t() const;
template Fraction::operator uint8_t() const;
template Fraction::operator uint16_t() const;
template Fraction::operator uint32_t() const;

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

template <typename T> int64_t Fraction::Compare(T rhs) const {
  return numerator_ - rhs * denominator_;
}
template int64_t Fraction::Compare<int8_t>(int8_t) const;
template int64_t Fraction::Compare<int16_t>(int16_t) const;
template int64_t Fraction::Compare<int32_t>(int32_t) const;
template int64_t Fraction::Compare<int64_t>(int64_t) const;
template int64_t Fraction::Compare<uint8_t>(uint8_t) const;
template int64_t Fraction::Compare<uint16_t>(uint16_t) const;
template int64_t Fraction::Compare<uint32_t>(uint32_t) const;

}  // namespace fishbait
