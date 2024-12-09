#ifndef AI_SRC_UTILS_FRACTION_H_
#define AI_SRC_UTILS_FRACTION_H_

#include <cstdint>
#include <algorithm>
#include <ostream>
#include <utility>

namespace fishbait {

class Fraction {
 public:
  /*
    @brief Constructs the fraction 0/1.
  */
  Fraction() : numerator_{0}, denominator_{1} {};

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

  /* @brief Fraction serialize function */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(numerator_, denominator_);
  }

  int64_t numerator() { return numerator_; }
  int64_t denominator() { return denominator_; }

  /*
    @brief Sets the fraction to its multiplicative inverse.
  */
  void Invert();

  // Addition
  Fraction& operator+=(const Fraction& rhs);
  template<typename T> Fraction& operator+=(T rhs);

  // Subtraction
  Fraction& operator-=(const Fraction& rhs);
  template<typename T> Fraction& operator-=(T rhs);

  // Multiplication
  Fraction& operator*=(const Fraction& rhs);
  template<typename T> Fraction& operator*=(T rhs);

  // Division
  Fraction& operator/=(const Fraction& rhs);
  template<typename T> Fraction& operator/=(T rhs);

  // Equality operators
  friend bool operator==(const Fraction& lhs, const Fraction& rhs);
  template<typename T> friend bool operator==(const Fraction& lhs, T rhs);
  template<typename T> friend bool operator==(T lhs, const Fraction& rhs);
  friend bool operator==(const Fraction& lhs, double rhs);
  friend bool operator==(double lhs, const Fraction& rhs);

  // Inequality Operators
  friend bool operator!=(const Fraction& lhs, const Fraction& rhs);
  template<typename T> friend bool operator!=(const Fraction& lhs, T rhs);
  template<typename T> friend bool operator!=(T lhs, const Fraction& rhs);
  friend bool operator!=(const Fraction& lhs, double rhs);
  friend bool operator!=(double lhs, const Fraction& rhs);

  // Less than operators
  friend bool operator<(const Fraction& lhs, const Fraction& rhs);
  template<typename T> friend bool operator<(const Fraction& lhs, T rhs);
  template<typename T> friend bool operator<(T lhs, const Fraction& rhs);
  friend bool operator<(const Fraction& lhs, double rhs);
  friend bool operator<(double lhs, const Fraction& rhs);

  // Greater than operators
  friend bool operator>(const Fraction& lhs, const Fraction& rhs);
  template<typename T> friend bool operator>(const Fraction& lhs, T rhs);
  template<typename T> friend bool operator>(T lhs, const Fraction& rhs);
  friend bool operator>(const Fraction& lhs, double rhs);
  friend bool operator>(double lhs, const Fraction& rhs);

  // Less than or equal to operators
  friend bool operator<=(const Fraction& lhs, const Fraction& rhs);
  template<typename T> friend bool operator<=(const Fraction& lhs, T rhs);
  template<typename T> friend bool operator<=(T lhs, const Fraction& rhs);
  friend bool operator<=(const Fraction& lhs, double rhs);
  friend bool operator<=(double lhs, const Fraction& rhs);

  // Greater than or equal to operators
  friend bool operator>=(const Fraction& lhs, const Fraction& rhs);
  template<typename T> friend bool operator>=(const Fraction& lhs, T rhs);
  template<typename T> friend bool operator>=(T lhs, const Fraction& rhs);
  friend bool operator>=(const Fraction& lhs, double rhs);
  friend bool operator>=(double lhs, const Fraction& rhs);

  // Conversion Operators
  template <typename T> explicit operator T() const;
  explicit operator double() const {
    return static_cast<double>(numerator_) / denominator_;
  }
  explicit operator bool() const { return numerator_ != 0; }

  // Print function
  friend std::ostream& operator<<(std::ostream &strm, const Fraction &a) {
    strm << a.numerator_ << "/" << a.denominator_;
    return strm;
  }

  /*
    @brief Calculates the greatest common divisor. 
    
    Uses an optimized Binary GCD algorithm from
    https://hbfs.wordpress.com/2013/12/10/the-speed-of-gcd/
  */
  static uint64_t GCD(uint64_t a, uint64_t b);

  /*
    @brief Calculates the least common multiple.
    
    Uses the fact that:
        LCM(a, b) = (a x b) / GCD(a, b).
  */
  static uint64_t LCM(uint64_t a, uint64_t b) {
    if (a == 0 || b == 0) return 0;
    return (a / GCD(a, b)) * b;
  }

 private:
  /*
    @brief Reduces the fraction assuming that the denominator is greater than 0.
  */
  void Reduce();

  /*
    @brief Moves the sign of the fraction to the numerator.
    
    Leaves the denominator greater than 0.
  */
  void NumeratorSign();

  /*
    @brief Compare this Fraction with the given Fraction.

    @return a negative number if *this < rhs, returns 0 if *this == rhs,
        returns a positive number if *this > rhs.
  */
  int64_t Compare(const Fraction& rhs) const {
    return numerator_ * rhs.denominator_ - rhs.numerator_ * denominator_;
  }
  template<typename T> int64_t Compare(T rhs) const;
  double Compare(double rhs) const {
    return (operator double()) - rhs;
  }

  int64_t numerator_;    // Can be negative, 0, or positive
  int64_t denominator_;  // Always greater than 0
};

// Add fractions
Fraction operator+(Fraction lhs, const Fraction& rhs);

// Add fractions and ints with each other
template<typename T> Fraction operator+(T lhs, Fraction rhs);
template<typename T> Fraction operator+(Fraction lhs, T rhs);

// Add fractions and doubles with each other
double operator+(double lhs, const Fraction& rhs);
double operator+(const Fraction& lhs, double rhs);

// Subtract fractions
Fraction operator-(Fraction lhs, const Fraction& rhs);

// Subtract fractions and ints with each other
template<typename T> Fraction operator-(T lhs, Fraction rhs);
template<typename T> Fraction operator-(Fraction lhs, T rhs);

// Subtract fractions and doubles with each other
double operator-(double lhs, const Fraction& rhs);
double operator-(const Fraction& lhs, double rhs);

// Negative fraction
Fraction operator-(Fraction f);

// Multiply fractions
Fraction operator*(Fraction lhs, const Fraction& rhs);

// Multiply fractions and ints with each other
template<typename T> Fraction operator*(T lhs, Fraction rhs);
template<typename T> Fraction operator*(Fraction lhs, T rhs);

// Multiply fractions and doubles with each other
double operator*(double lhs, const Fraction& rhs);
double operator*(const Fraction& lhs, double rhs);

// Divide fractions
Fraction operator/(Fraction lhs, const Fraction& rhs);

// Divide fractions and ints with each other
template<typename T> Fraction operator/(T lhs, Fraction rhs);
template<typename T> Fraction operator/(Fraction lhs, T rhs);

// Divide fractions and doubles with each other
double operator/(double lhs, const Fraction& rhs);
double operator/(const Fraction& lhs, double rhs);

}  // namespace fishbait

#endif  // AI_SRC_UTILS_FRACTION_H_
