# Copyright 2021 Marzuk Rashid

"""Generates catch2 testers for the C++ fraction class"""

import argparse
import fractions
import random

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
  elif operator == "==":
    return "equals"
  elif operator == "!=":
    return "not equals"
  elif operator == "<":
    return "less than"
  elif operator == ">":
    return "greater than"
  elif operator == "<=":
    return "less than equal to"
  elif operator == ">=":
    return "greater than equal to"

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

def bool_py_to_cpp(bool_exp):
  """Given a python bool, return the equivalent C++ expression."""
  if bool_exp:
    return "true"
  else:
    return "false"

def perform_operation(a1, a2, operator):
  if operator == "+":
    return a1 + a2
  elif operator == "-":
    return a1 - a2
  elif operator == "*":
    return a1 * a2
  elif operator == "/":
    return a1 / a2
  elif operator == "==":
    return bool_py_to_cpp(a1 == a2)
  elif operator == "!=":
    return bool_py_to_cpp(a1 != a2)
  elif operator == "<":
    return bool_py_to_cpp(a1 < a2)
  elif operator == ">":
    return bool_py_to_cpp(a1 > a2)
  elif operator == "<=":
    return bool_py_to_cpp(a1 <= a2)
  elif operator == ">=":
    return bool_py_to_cpp(a1 >= a2)

def begin_test_case(name):
  print("TEST_CASE(\"{}\", \"[utils][fraction]\") {{".format(name))

def end_test_case(name):
  print("}}  // TEST_CASE \"{}\"".format(name))
  print()

def arithmetic_equal_fraction_iter(operator, sign_1, sign_2, it):
  """Single iterator for arithmetic_equal_fraction.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
  """
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  f1 = random_fraction(sign_1)
  print("  " + construct_fraction(f1, it))
  f2 = random_fraction(sign_2)
  print("  f{} {}= {};".format(it, operator, construct_fraction(f2)))

  f1 = perform_operation(f1, f2, operator)

  print("  REQUIRE(f{}.numerator() == {});".format(it, f1.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, f1.denominator))

def arithmetic_equal_fraction(operator):
  """Generates a tester for +=, -=, *=, and /= a fraction."""
  name = "fraction {} equals fraction".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_equal_fraction_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def arithmetic_equal_int_iter(operator, sign_1, sign_2, it):
  """Single iterator for arithmetic_equal_int.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
  """
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  f1 = random_fraction(sign_1)
  print("  " + construct_fraction(f1, it))
  i2 = random_int(sign_2)
  print("  f{} {}= {};".format(it, operator, i2))

  f1 = perform_operation(f1, i2, operator)

  print("  REQUIRE(f{}.numerator() == {});".format(it, f1.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, f1.denominator))

def arithmetic_equal_int(operator):
  """Generates a tester for +=, -=, *=, and /= an int."""
  name = "fraction {} equals int".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_equal_int_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def arithmetic_int_fraction_iter(operator, sign_1, sign_2, it, int_first):
  """Single iterator for arithmetic_int_fraction.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
    int_first: True to create a tester for ints op fractions. False to create a
        tester for fractions op ints.
  """
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

  frac = perform_operation(arg1, arg2, operator)

  print("  REQUIRE(f{}.numerator() == {});".format(it, frac.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, frac.denominator))

def arithmetic_int_fraction(operator, int_first):
  """Generates a tester for +, -, *, and / a fraction and an int.

  Args:
    operator: +, -, *, or /
    int_first: True to create a tester for ints op fractions. False to create a
        tester for fractions op ints.
  """
  name = None
  if int_first:
    name = "int {} fraction".format(operator_to_str(operator))
  else:
    name = "fraction {} int".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_int_fraction_iter(operator, sign_1, sign_2, it, int_first)
      it += 1
  end_test_case(name)

def arithmetic_double_fraction_iter(operator, sign_1, sign_2, it, double_first):
  """Single iterator for arithmetic_double_fraction.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
    double_first: True to create a tester for doubles op fractions. False to
        create a tester for fractions op doubles.
  """
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

  double = perform_operation(arg1, arg2, operator)

  print("  REQUIRE({} {} {} == Approx({}));".format(arg1_str, operator,
                                                    arg2_str, double))

def arithmetic_double_fraction(operator, double_first):
  """Generates a tester for +, -, *, and / a fraction and a double.

  Args:
    operator: +, -, *, or /
    double_first: True to create a tester for doubles op fractions. False to
        create a tester for fractions op doubles.
  """
  name = None
  if double_first:
    name = "double {} fraction".format(operator_to_str(operator))
  else:
    name = "fraction {} double".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_double_fraction_iter(operator, sign_1, sign_2, it,
                                      double_first)
      it += 1
  end_test_case(name)

def arithmetic_fraction_fraction_iter(operator, sign_1, sign_2, it):
  """Single iterator for arithmetic_fraction_fraction.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
  """
  print("  // {}".format(signs_to_str(sign_1, sign_2)))
  f1 = random_fraction(sign_1)
  f1_str = construct_fraction(f1)
  f2 = random_fraction(sign_2)
  f2_str = construct_fraction(f2)
  print("  " + construct_fraction_arith(f1_str, f2_str, operator, it))

  f1 = perform_operation(f1, f2, operator)

  print("  REQUIRE(f{}.numerator() == {});".format(it, f1.numerator))
  print("  REQUIRE(f{}.denominator() == {});".format(it, f1.denominator))

def arithmetic_fraction_fraction(operator):
  """Generates a tester for +, -, *, and / two fractions."""
  name = "fraction {} fraction".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_fraction_fraction_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def arithmetic_tester(operator):
  """Generates a tester for basic arithmetic (+, -, *, /)."""
  arithmetic_equal_fraction(operator)
  arithmetic_equal_int(operator)
  arithmetic_int_fraction(operator, True)
  arithmetic_int_fraction(operator, False)
  arithmetic_double_fraction(operator, True)
  arithmetic_double_fraction(operator, False)
  arithmetic_fraction_fraction(operator)

def comparison_test_comment(sign_1, sign_2, is_same):
  is_same_qualifier = ""
  if sign_1 == sign_2 and sign_1 != 0:
    if is_same:
      is_same_qualifier = " equal"
    else:
      is_same_qualifier = " not equal"
  return "// {}{}".format(signs_to_str(sign_1, sign_2), is_same_qualifier)

def comparison_fraction_fraction_iter(operator, sign_1, sign_2, it, is_same):
  """Single iterator for comparison_fraction_fraction.

  Args:
    operator: ==, !=, <, >=, etc.
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
    is_same: if the two fractions created to compare should be the same
  """
  print("  " + comparison_test_comment(sign_1, sign_2, is_same))
  f1 = random_fraction(sign_1)
  f1_str = "f{}".format(it)
  print("  " + construct_fraction(f1, it))
  f2 = f1
  if not is_same:
    f2 = random_fraction(sign_2)
  f2_str = "f{}".format(it + 1)
  print("  " + construct_fraction(f2, it + 1))

  result = perform_operation(f1, f2, operator)

  print("  REQUIRE(({} {} {}) == {});".format(f1_str, operator, f2_str, result))

def comparison_fraction_fraction(operator):
  """Generates a tester for ==, !=, <, >, <=, and >= two fractions."""
  name = "fraction fraction {}".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if ((sign_1 == 1 and sign_1 == sign_2) or
          (sign_1 == -1 and sign_1 == sign_2)):
        comparison_fraction_fraction_iter(operator, sign_1, sign_2, it, True)
        it += 2
      comparison_fraction_fraction_iter(operator, sign_1, sign_2, it, False)
      it += 2
  end_test_case(name)

def comparison_fraction_num_iter(operator, sign_1, sign_2, it, is_same,
                                 num_first, is_double):
  """Single iterator for comparison_fraction_num.

  Args:
    operator: ==, !=, <, >=, etc.
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
    is_same: if the two fractions created to compare should be the same
    num_first: True to create a tester for nums op fractions. False to create a
        tester for fractions op nums.
    is_double: True to create a tester for double numbers. False to create a
        tester for int numbers.
  """
  print("  " + comparison_test_comment(sign_1, sign_2, is_same))
  frac_sign = sign_1
  num_sign = sign_2
  if num_first:
    num_sign = sign_1
    frac_sign = sign_2
  num_arg = None
  if is_double:
    num_arg = random_double(num_sign)
  else:
    num_arg = random_int(num_sign)
  frac_arg = None
  if is_same:
    frac_arg = fractions.Fraction(num_arg)
  else:
    frac_arg = random_fraction(frac_sign)
  str1 = "f{}".format(it)
  str2 = num_arg
  if num_first:
    str1 = num_arg
    str2 = "f{}".format(it)
  print("  " + construct_fraction(frac_arg, it))

  result = None
  if num_first:
    result = perform_operation(num_arg, frac_arg, operator)
  else:
    result = perform_operation(frac_arg, num_arg, operator)

  print("  REQUIRE(({} {} {}) == {});".format(str1, operator, str2, result))

def comparison_fraction_num(operator, num_first, is_double):
  """Generates a tester for ==, !=, <, >, <=, and >= two fractions.
  Args:
    operator: ==, !=, <, >=, etc.
    num_first: True to create a tester for nums op fractions. False to create a
        tester for fractions op nums.
    is_double: True to create a tester for double numbers. False to create a
        tester for int numbers.
  """
  name = None
  if num_first:
    if is_double:
      name = "double fraction {}".format(operator_to_str(operator))
    else:
      name = "int fraction {}".format(operator_to_str(operator))
  else:
    if is_double:
      name = "fraction double {}".format(operator_to_str(operator))
    else:
      name = "fraction int {}".format(operator_to_str(operator))
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if ((sign_1 == 1 and sign_1 == sign_2) or
          (sign_1 == -1 and sign_1 == sign_2)):
        comparison_fraction_num_iter(operator, sign_1, sign_2, it, True,
                                     num_first, is_double)
        it += 1
      comparison_fraction_num_iter(operator, sign_1, sign_2, it, False,
                                   num_first, is_double)
      it += 1
  end_test_case(name)

def comparison_tester(operator):
  """Generates a tester for comparisons (==, !=, <, >, <=, >=)."""
  comparison_fraction_fraction(operator)
  comparison_fraction_num(operator, True, False)
  comparison_fraction_num(operator, False, False)
  comparison_fraction_num(operator, True, True)
  comparison_fraction_num(operator, False, True)

def main():
  parser = argparse.ArgumentParser(description="Generate tests for the C++ "
                                               "Fraction class.")
  parser.add_argument("operator", help="operator to generate tests for",
                      choices=["+", "-", "*", "/", "==", "!=", "<", ">", "<=",
                               ">="])
  args = parser.parse_args()
  if args.operator in ["+", "-", "*", "/"]:
    arithmetic_tester(args.operator)
  else:
    comparison_tester(args.operator)

if __name__ == "__main__":
  main()
