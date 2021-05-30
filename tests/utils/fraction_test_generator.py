# Copyright 2021 Marzuk Rashid

"""Generates catch2 testers for the C++ fraction class"""

import fractions
import random
import sys

FRACION_UPPER_BOUND = 10000000

def random_fraction(sign = 1):
  numerator = random.randint(1, FRACION_UPPER_BOUND) * sign
  denominator = random.randint(1, FRACION_UPPER_BOUND)
  return fractions.Fraction(numerator, denominator)

def random_int(sign = 1):
  return random.randint(1, FRACION_UPPER_BOUND) * sign

def random_double(sign = 1):
  return random.uniform(1, FRACION_UPPER_BOUND) * sign

def sign_to_str(sign):
  if sign == 1:
    return "positive"
  elif sign == -1:
    return "negative"
  else:
    return "0"

def signs_to_str(sign_1, sign_2):
  """sign_to_str of two signs separated by a space."""
  return "{} {}".format(sign_to_str(sign_1), sign_to_str(sign_2))

def operator_to_str(operator):
  if operator == "+":
    return "plus"
  elif operator == "-":
    return "minus"
  elif operator == "*":
    return "multiply"
  elif operator == "/":
    return "divide"

def construct_fraction(frac_obj, it = None):
  """Returns C++ code to construct the given Python fraction."""
  if it is None:
    return "utils::Fraction{{{}, {}}}".format(frac_obj.numerator,
                                              frac_obj.denominator)
  else:
    return "utils::Fraction f{}{{{}, {}}};".format(it, frac_obj.numerator,
                                                  frac_obj.denominator)

def construct_fraction_arith(a1, a2, operator, it):
  """Returns C++ code to construct a fraction through arithmetic."""
  return "utils::Fraction f{} = {} {} {};".format(it, a1, operator, a2)

def begin_test_case(name):
  print("TEST_CASE(\"{}\", \"[utils][fraction]\") {{".format(name))

def end_test_case(name):
  print("}}  // TEST_CASE \"{}\"".format(name))
  print()

def operator_equal_fraction_iter(operator, sign_1, sign_2, it):
  """Single iterator for operator_equal_fraction."""
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  f1 = random_fraction(sign_1)
  print("  " + construct_fraction(f1, it))
  f2 = random_fraction(sign_2)
  print("  f{} {}= {};".format(it, operator, construct_fraction(f2)))

  if operator == "+":
    f1 += f2
  elif operator == "-":
    f1 -= f2
  elif operator == "*":
    f1 *= f2
  elif operator == "/":
    f1 /= f2

  print("  REQUIRE(f{}.numerator() == {});".format(it, f1.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, f1.denominator))

def operator_equal_fraction(operator):
  """Generates a tester for +=, -=, *=, and /= a fraction."""
  name = "fraction {} equals fraction".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      operator_equal_fraction_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def operator_equal_int_iter(operator, sign_1, sign_2, it):
  """Single iterator for operator_equal_int."""
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  f1 = random_fraction(sign_1)
  print("  " + construct_fraction(f1, it))
  i2 = random_int(sign_2)
  print("  f{} {}= {};".format(it, operator, i2))

  if operator == "+":
    f1 += i2
  elif operator == "-":
    f1 -= i2
  elif operator == "*":
    f1 *= i2
  elif operator == "/":
    f1 /= i2

  print("  REQUIRE(f{}.numerator() == {});".format(it, f1.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, f1.denominator))

def operator_equal_int(operator):
  """Generates a tester for +=, -=, *=, and /= an int."""
  name = "fraction {} equals int".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      operator_equal_int_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def operator_int_fraction_iter(operator, sign_1, sign_2, it, int_first):
  """Single iterator for operator_int_fraction."""
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  arg1 = None
  arg1_str = None
  arg2 = None
  arg2_str = None
  frac = None
  if int_first:
    arg1 = random_int(sign_1)
    arg1_str = arg1
    arg2 = random_fraction(sign_2)
    print("  " + construct_fraction(arg2, it))
    arg2_str = "f{}".format(it)
    frac = arg2
  else:
    arg1 = random_fraction(sign_1)
    print("  " + construct_fraction(arg1, it))
    arg1_str = "f{}".format(it)
    arg2 = random_int(sign_2)
    arg2_str = arg2
    frac = arg1
  print("  f{} = {} {} {};".format(it, arg1_str, operator, arg2_str))

  if operator == "+":
    frac = arg1 + arg2
  elif operator == "-":
    frac = arg1 - arg2
  elif operator == "*":
    frac = arg1 * arg2
  elif operator == "/":
    frac = arg1 / arg2

  print("  REQUIRE(f{}.numerator() == {});".format(it, frac.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, frac.denominator))

def operator_int_fraction(operator, int_first):
  """Generates a tester for +, -, *, and / a fraction and an int."""
  name = None
  if int_first:
    name = "int {} fraction".format(operator_to_str(operator))
    begin_test_case(name)
  else:
    name = "fraction {} int".format(operator_to_str(operator))
    begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      operator_int_fraction_iter(operator, sign_1, sign_2, it, int_first)
      it += 1
  end_test_case(name)

def operator_double_fraction_iter(operator, sign_1, sign_2, it, double_first):
  """Single iterator for operator_double_fraction."""
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  arg1 = None
  arg1_str = None
  arg2 = None
  arg2_str = None
  double = None
  if double_first:
    arg1 = random_double(sign_1)
    arg1_str = arg1
    arg2 = random_fraction(sign_2)
    print("  " + construct_fraction(arg2, it))
    arg2_str = "f{}".format(it)
    double = arg1
  else:
    arg1 = random_fraction(sign_1)
    print("  " + construct_fraction(arg1, it))
    arg1_str = "f{}".format(it)
    arg2 = random_double(sign_2)
    arg2_str = arg2
    double = arg2

  if operator == "+":
    double = arg1 + arg2
  elif operator == "-":
    double = arg1 - arg2
  elif operator == "*":
    double = arg1 * arg2
  elif operator == "/":
    double = arg1 / arg2

  print("  REQUIRE({} {} {} == Approx({}));".format(arg1_str, operator,
                                                    arg2_str, double))

def operator_double_fraction(operator, double_first):
  """Generates a tester for +, -, *, and / a fraction and a double."""
  name = None
  if double_first:
    name = "double {} fraction".format(operator_to_str(operator))
    begin_test_case(name)
  else:
    name = "fraction {} double".format(operator_to_str(operator))
    begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      operator_double_fraction_iter(operator, sign_1, sign_2, it, double_first)
      it += 1
  end_test_case(name)

def operator_fraction_fraction_iter(operator, sign_1, sign_2, it):
  """Single iterator for operator_fraction_fraction."""
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  f1 = random_fraction(sign_1)
  f1_str = construct_fraction(f1)
  f2 = random_fraction(sign_2)
  f2_str = construct_fraction(f2)
  print("  " + construct_fraction_arith(f1_str, f2_str, operator, it))

  if operator == "+":
    f1 += f2
  elif operator == "-":
    f1 -= f2
  elif operator == "*":
    f1 *= f2
  elif operator == "/":
    f1 /= f2

  print("  REQUIRE(f{}.numerator() == {});".format(it, f1.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, f1.denominator))

def operator_fraction_fraction(operator):
  """Generates a tester for +, -, *, and / two fractions."""
  name = "fraction {} fraction".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      operator_fraction_fraction_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def arithmetic_tester(operator):
  """Generates a tester for basic arithmetic (+, -, *, /)."""
  operator_equal_fraction(operator)
  operator_equal_int(operator)
  operator_int_fraction(operator, True)
  operator_int_fraction(operator, False)
  operator_double_fraction(operator, True)
  operator_double_fraction(operator, False)
  operator_fraction_fraction(operator)

def main(argv):
  if len(argv) < 2 or argv[1] not in ["+", "-", "*", "/"]:
    sys.exit("usage: {} <operator>\n"
             "operator must be +, -, *, or /".format(argv[0]))
  arithmetic_tester(argv[1])

if __name__ == "__main__":
  main(sys.argv)
