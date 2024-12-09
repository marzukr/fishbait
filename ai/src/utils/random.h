#ifndef AI_SRC_UTILS_RANDOM_H_
#define AI_SRC_UTILS_RANDOM_H_

#include <limits>
#include <random>
#include <type_traits>

#include "utils/cereal.h"

namespace fishbait {

class Random {
 public:
  class Seed {
   public:
    Seed() : seed_(GenerateSeed()) {}
    explicit Seed(const uint32_t seed) : seed_(seed) {}
    Seed(const Seed& other) = default;
    Seed& operator=(const Seed& other) = default;
    uint32_t operator()() {
      return seed_;
    }
    static uint32_t GenerateSeed() {
      std::random_device dev;
      return dev();
    }
   private:
    uint32_t seed_;
  };  // Seed

  Random() : rng_(Seed::GenerateSeed()) {}
  explicit Random(Seed seed) : rng_(seed()) {}
  Random(const Random& other) = default;
  Random& operator=(const Random& other) = default;
  std::mt19937& operator()() { return rng_; }
  void seed(Seed seed) { rng_.seed(seed()); }

  /* @brief Cereal function. */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(rng_);
  }

 private:
  std::mt19937 rng_;
};  // Random

/*
  The following code, starting from "libc++ begin" until "libc++ end" is taken
  from libc++ and slightly modified to conform with the fishbait type naming
  style. libc++ has the following copyright notice:

  Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
  See https://llvm.org/LICENSE.txt for license information.
  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/* ------------------------------ libc++ begin ------------------------------ */

template <unsigned long long _Xp, size_t _Rp>  // NOLINT(runtime/int)
struct Log2Imp {
  static const size_t value = _Xp & ((unsigned long long)(1) << _Rp)  // NOLINT(*)
      ? _Rp : Log2Imp<_Xp, _Rp - 1>::value;
};

template <unsigned long long _Xp>  // NOLINT(runtime/int)
struct Log2Imp<_Xp, 0> {
  static const size_t value = 0;
};

template <size_t _Rp>
struct Log2Imp<0, _Rp> {
  static const size_t value = _Rp + 1;
};

template <class _UIntType, _UIntType _Xp>
struct Log2 {
  static const size_t value =
      Log2Imp<_Xp, sizeof(_UIntType) * __CHAR_BIT__ - 1>::value;
};

template <class _Engine, class _UIntType>
class IndependentBitsEngine {
 public:
  // types
  typedef _UIntType result_type;

 private:
  typedef typename _Engine::result_type _Engine_result_type;
  typedef typename std::conditional<sizeof(_Engine_result_type) <=  // NOLINT(*)
                                    sizeof(result_type), result_type,
                                    _Engine_result_type>::type
                   _Working_result_type;

  _Engine& __e_;
  std::size_t __w_;
  std::size_t __w0_;
  std::size_t __n_;
  std::size_t __n0_;
  _Working_result_type __y0_;
  _Working_result_type __y1_;
  _Engine_result_type __mask0_;
  _Engine_result_type __mask1_;

  static constexpr const _Working_result_type _Rp =
      _Engine::max() - _Engine::min() + _Working_result_type(1);
  static constexpr const size_t __m =
      Log2<_Working_result_type, _Rp>::value;
  static constexpr const size_t _WDt =
      std::numeric_limits<_Working_result_type>::digits;
  static constexpr const size_t _EDt =
      std::numeric_limits<_Engine_result_type>::digits;

 public:
  // constructors and seeding functions
  IndependentBitsEngine(_Engine& __e, std::size_t __w) : __e_(__e), __w_(__w) {
    __n_ = __w_ / __m + (__w_ % __m != 0);
    __w0_ = __w_ / __n_;
    if (_Rp == 0) {
      __y0_ = _Rp;
    } else if (__w0_ < _WDt) {
      __y0_ = (_Rp >> __w0_) << __w0_;
    } else {
      __y0_ = 0;
    }
    if (_Rp - __y0_ > __y0_ / __n_) {
      ++__n_;
      __w0_ = __w_ / __n_;
      if (__w0_ < _WDt) {
        __y0_ = (_Rp >> __w0_) << __w0_;
      } else {
        __y0_ = 0;
      }
    }
    __n0_ = __n_ - __w_ % __n_;
    if (__w0_ < _WDt - 1) {
      __y1_ = (_Rp >> (__w0_ + 1)) << (__w0_ + 1);
    } else {
      __y1_ = 0;
    }
    __mask0_ = __w0_ > 0 ? _Engine_result_type(~0) >> (_EDt - __w0_)
                         : _Engine_result_type(0);
    __mask1_ = __w0_ < _EDt - 1 ? _Engine_result_type(~0) >>
                                  (_EDt - (__w0_ + 1))
                                : _Engine_result_type(~0);
  }

  // generating functions
  result_type operator()() {
    return __eval(std::integral_constant<bool, _Rp != 0>());
  }

 private:
  result_type __eval(std::false_type) {
    return static_cast<result_type>(__e_() & __mask0_);
  }
  result_type __eval(std::true_type) {
    const size_t _WRt = std::numeric_limits<result_type>::digits;
    result_type _Sp = 0;
    for (size_t __k = 0; __k < __n0_; ++__k) {
      _Engine_result_type __u;
      do {
        __u = __e_() - _Engine::min();
      } while (__u >= __y0_);
      if (__w0_ < _WRt) {
        _Sp <<= __w0_;
      } else {
        _Sp = 0;
      }
      _Sp += __u & __mask0_;
    }
    for (size_t __k = __n0_; __k < __n_; ++__k) {
      _Engine_result_type __u;
      do {
        __u = __e_() - _Engine::min();
      } while (__u >= __y1_);
      if (__w0_ < _WRt - 1) {
        _Sp <<= __w0_ + 1;
      } else {
        _Sp = 0;
      }
      _Sp += __u & __mask1_;
    }
    return _Sp;
  }
};  // IndependentBitsEngine

inline constexpr int LibCppClz(unsigned __x) noexcept {
  return __builtin_clz(__x);
}

inline constexpr int LibCppClz(unsigned long __x) noexcept {  // NOLINT(*)
  return __builtin_clzl(__x);
}

inline constexpr int LibCppClz(unsigned long long __x) noexcept {  // NOLINT(*)
  return __builtin_clzll(__x);
}

template <class _IntType = int>
class UniformIntDistribution {
 public:
  // types
  typedef _IntType result_type;

  class param_type {
   private:
    result_type __a_;
    result_type __b_;
   public:
    typedef UniformIntDistribution distribution_type;

    explicit param_type(result_type __a = 0,
        result_type __b = std::numeric_limits<result_type>::max())
        : __a_(__a), __b_(__b) {}

    result_type a() const { return __a_; }
    result_type b() const { return __b_; }

    friend bool operator==(const param_type& __x, const param_type& __y) {
      return __x.__a_ == __y.__a_ && __x.__b_ == __y.__b_;
    }
    friend bool operator!=(const param_type& __x, const param_type& __y) {
      return !(__x == __y);
    }
  };  // UniformIntDistribution::param_type

 private:
  param_type __p_;

 public:
  // constructors and reset functions
  explicit UniformIntDistribution(result_type __a = 0,
      result_type __b = std::numeric_limits<result_type>::max())
      : __p_(param_type(__a, __b)) {}
  explicit UniformIntDistribution(const param_type& __p) : __p_(__p) {}
  void reset() {}

  // generating functions
  template <class _URNG>
  result_type operator()(_URNG& __g) { return (*this)(__g, __p_); }
  template <class _URNG>
  result_type operator()(_URNG& __g, const param_type& __p) {
    typedef typename std::conditional<sizeof(result_type) <= sizeof(uint32_t),  // NOLINT(*)
                                      uint32_t, uint64_t>::type _UIntType;
    const _UIntType _Rp = _UIntType(__p.b()) - _UIntType(__p.a()) +
                          _UIntType(1);
    if (_Rp == 1) return __p.a();
    const size_t _Dt = std::numeric_limits<_UIntType>::digits;
    typedef IndependentBitsEngine<_URNG, _UIntType> _Eng;
    if (_Rp == 0) return static_cast<result_type>(_Eng(__g, _Dt)());
    size_t __w = _Dt - LibCppClz(_Rp) - 1;
    if ((_Rp & (std::numeric_limits<_UIntType>::max() >> (_Dt - __w))) != 0) {
      ++__w;
    }
    _Eng __e(__g, __w);
    _UIntType __u;
    do {
      __u = __e();
    } while (__u >= _Rp);
    return static_cast<result_type>(__u + __p.a());
  }

  // property functions
  result_type a() const { return __p_.a(); }
  result_type b() const { return __p_.b(); }

  param_type param() const { return __p_; }
  void param(const param_type& __p) { __p_ = __p; }

  result_type min() const { return a(); }
  result_type max() const { return b(); }

  friend bool operator==(const UniformIntDistribution& __x,
                         const UniformIntDistribution& __y) {
    return __x.__p_ == __y.__p_;
  }
  friend bool operator!=(const UniformIntDistribution& __x,
                         const UniformIntDistribution& __y) {
    return !(__x == __y);
  }
};  // UniformIntDistribution

/* ------------------------------- libc++ end ------------------------------- */

}  // namespace fishbait

#endif  // AI_SRC_UTILS_RANDOM_H_
