"""Generates catch2 testers for the C++ fraction class"""

import argparse
import fractions
import random

FRACTION_UPPER_BOUND = 10000000

def random_int(sign = 1, bits = 32, signed = True):
  type_upper_bound = 2**(bits - signed) - 1
  upper_bound = min(type_upper_bound, FRACTION_UPPER_BOUND)
  return random.randint(1, upper_bound) * sign

def random_double(sign = 1):
  return random.uniform(1, FRACTION_UPPER_BOUND) * sign

def random_fraction(sign = 1, bits = None, signed = None):
  numerator = None
  denominator = None
  if bits is not None and signed is not None:
    numerator = random_int(sign, bits, signed)
    denominator = random_int(1, bits, signed)
  else:
    numerator = random.randint(1, FRACTION_UPPER_BOUND) * sign
    denominator = random.randint(1, FRACTION_UPPER_BOUND)
  return fractions.Fraction(numerator, denominator)

def sign_to_str(sign):
  if sign == 1:
    return "positive"
  elif sign == -1:
    return "negative"
  else:
    return "0"

def signs_to_str(sign_1, sign_2):
  """sign_to_str of two signs separated by a space."""
  return f"{sign_to_str(sign_1)} {sign_to_str(sign_2)}"

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
    return f"fishbait::Fraction{{{frac_obj.numerator}, {frac_obj.denominator}}}"
  else:
    return (f"fishbait::Fraction f{it}{{{frac_obj.numerator}, "
            f"{frac_obj.denominator}}};")

def construct_fraction_arith(a1, a2, operator, it):
  """Returns C++ code to construct a fraction through arithmetic."""
  return f"fishbait::Fraction f{it} = {a1} {operator} {a2};"

def int_type_str(bits, signed):
  """For instance, with bits=8 and signed = False, returns "uint8_t"."""
  sign_str = "" if signed else "u"
  return f"{sign_str}int{bits}_t"

def construct_int(n, bits, signed, it = None):
  """Returns C++ code to construct the given integer."""
  if it is None:
    return f"{int_type_str(bits, signed)}{{{n}}}"
  else:
    return f"{int_type_str(bits, signed)} i{it}{{{n}}}"

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
  print(f"TEST_CASE(\"{name}\", \"[utils][fraction]\") {{")

def end_test_case(name):
  print(f"}}  // TEST_CASE \"{name}\"")
  print()

def arithmetic_equal_fraction_iter(operator, sign_1, sign_2, it):
  """Single iterator for arithmetic_equal_fraction.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    it: what number to start the fraction variable naming
  """
  print(f"  // {signs_to_str(sign_1, sign_2)}")
  f1 = random_fraction(sign_1)
  print("  " + construct_fraction(f1, it))
  f2 = random_fraction(sign_2)
  print(f"  f{it} {operator}= {construct_fraction(f2)};")

  f1 = perform_operation(f1, f2, operator)

  print(f"  REQUIRE(f{it}.numerator() == {f1.numerator});")
  print(f"  REQUIRE(f{it}.denominator() == {f1.denominator});")

def arithmetic_equal_fraction(operator):
  """Generates a tester for +=, -=, *=, and /= a fraction."""
  name = f"fraction {operator_to_str(operator)} equals fraction"
  begin_test_case(name)
  it = 0
  for sign_1 in [1, -1, 0]:
    for sign_2 in [1, -1, 0]:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_equal_fraction_iter(operator, sign_1, sign_2, it)
      it += 1
  end_test_case(name)

def arithmetic_equal_int_iter(operator, sign_1, sign_2, bits, signed, it):
  """Single iterator for arithmetic_equal_int.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    bits: how many bits to use for the int
    signed: if the int is signed or unsigned
    it: what number to start the fraction variable naming
  """
  print(f"  // {signs_to_str(sign_1, sign_2)}")
  f1 = random_fraction(sign_1)
  print("  " + construct_fraction(f1, it))
  i2 = random_int(sign_2, bits, signed)
  i2_str = construct_int(i2, bits, signed)
  print(f"  f{it} {operator}= {i2_str};")

  f1 = perform_operation(f1, i2, operator)

  print(f"  REQUIRE(f{it}.numerator() == {f1.numerator});")
  print(f"  REQUIRE(f{it}.denominator() == {f1.denominator});")

def arithmetic_equal_int(operator, bits, signed):
  """Generates a tester for +=, -=, *=, and /= an int."""
  name = (f"fraction {operator_to_str(operator)} equals "
          f"{int_type_str(bits, signed)}")
  begin_test_case(name)
  it = 0
  int_sign_options = [1, -1, 0] if signed else [1, 0]
  for sign_1 in [1, -1, 0]:
    for sign_2 in int_sign_options:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_equal_int_iter(operator, sign_1, sign_2, bits, signed, it)
      it += 1
  end_test_case(name)

def arithmetic_int_fraction_iter(operator, sign_1, sign_2, bits, signed, it,
                                 int_first):
  """Single iterator for arithmetic_int_fraction.

  Args:
    operator: +, -, *, or /
    sign_1: 1, -1, or 0 for the sign of the first fraction to compare
    sign_2: 1, -1, or 0 for the sign of the second fraction to compare
    bits: how many bits to use for the int
    signed: if the int is signed or unsigned
    it: what number to start the fraction variable naming
    int_first: True to create a tester for ints op fractions. False to create a
        tester for fractions op ints.
  """
  print(f"  // {signs_to_str(sign_1, sign_2)}")
  arg1 = None
  arg1_str = None
  arg2 = None
  arg2_str = None
  frac = None
  if int_first:
    arg1 = random_int(sign_1, bits, signed)
    arg1_str = construct_int(arg1, bits, signed)
    arg2 = random_fraction(sign_2)
    print("  " + construct_fraction(arg2, it))
    arg2_str = f"f{it}"
    frac = arg2
  else:
    arg1 = random_fraction(sign_1)
    print("  " + construct_fraction(arg1, it))
    arg1_str = f"f{it}"
    arg2 = random_int(sign_2, bits, signed)
    arg2_str = construct_int(arg2, bits, signed)
    frac = arg1
  print(f"  f{it} = {arg1_str} {operator} {arg2_str};")

  frac = perform_operation(arg1, arg2, operator)

  print(f"  REQUIRE(f{it}.numerator() == {frac.numerator});")
  print(f"  REQUIRE(f{it}.denominator() == {frac.denominator});")

def arithmetic_int_fraction(operator, bits, signed, int_first):
  """Generates a tester for +, -, *, and / a fraction and an int.

  Args:
    operator: +, -, *, or /
    bits: how many bits to use for the int
    signed: if the int is signed or unsigned
    int_first: True to create a tester for ints op fractions. False to create a
        tester for fractions op ints.
  """
  name = None
  if int_first:
    name = f"{int_type_str(bits, signed)} {operator_to_str(operator)} fraction"
  else:
    name = f"fraction {operator_to_str(operator)} {int_type_str(bits, signed)}"
  begin_test_case(name)
  it = 0
  sign_1_options = [1, -1, 0]
  sign_2_options = [1, -1, 0]
  sign_1_options = [1, 0] if not signed and int_first else sign_1_options
  sign_2_options = [1, 0] if not signed and not int_first else sign_2_options
  for sign_1 in sign_1_options:
    for sign_2 in sign_2_options:
      if operator == "/" and sign_2 == 0:
        continue
      arithmetic_int_fraction_iter(operator, sign_1, sign_2, bits, signed, it,
                                   int_first)
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
  print(f"  // {signs_to_str(sign_1, sign_2)}")
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
    arg2_str = f"f{it}"
    double = arg1
  else:
    arg1 = random_fraction(sign_1)
    print("  " + construct_fraction(arg1, it))
    arg1_str = f"f{it}"
    arg2 = random_double(sign_2)
    arg2_str = arg2
    double = arg2

  double = perform_operation(arg1, arg2, operator)

  print(f"  REQUIRE({arg1_str} {operator} {arg2_str} == Approx({double}));")

def arithmetic_double_fraction(operator, double_first):
  """Generates a tester for +, -, *, and / a fraction and a double.

  Args:
    operator: +, -, *, or /
    double_first: True to create a tester for doubles op fractions. False to
        create a tester for fractions op doubles.
  """
  name = None
  if double_first:
    name = f"double {operator_to_str(operator)} fraction"
  else:
    name = f"fraction {operator_to_str(operator)} double"
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
  print(f"  // {signs_to_str(sign_1, sign_2)}")
  f1 = random_fraction(sign_1)
  f1_str = construct_fraction(f1)
  f2 = random_fraction(sign_2)
  f2_str = construct_fraction(f2)
  print("  " + construct_fraction_arith(f1_str, f2_str, operator, it))

  f1 = perform_operation(f1, f2, operator)

  print(f"  REQUIRE(f{it}.numerator() == {f1.numerator});")
  print(f"  REQUIRE(f{it}.denominator() == {f1.denominator});")

def arithmetic_fraction_fraction(operator):
  """Generates a tester for +, -, *, and / two fractions."""
  name = f"fraction {operator_to_str(operator)} fraction"
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
  for bits in [8, 16, 32, 64]:
    for signed in [True, False]:
      if not signed and bits == 64:
        continue
      arithmetic_equal_int(operator, bits, signed)
      arithmetic_int_fraction(operator, bits, signed, True)
      arithmetic_int_fraction(operator, bits, signed, False)
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
  return f"// {signs_to_str(sign_1, sign_2)}{is_same_qualifier}"

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
  f1_str = f"f{it}"
  print("  " + construct_fraction(f1, it))
  f2 = f1
  if not is_same:
    f2 = random_fraction(sign_2)
  f2_str = f"f{it + 1}"
  print("  " + construct_fraction(f2, it + 1))

  result = perform_operation(f1, f2, operator)

  print(f"  REQUIRE(({f1_str} {operator} {f2_str}) == {result});")

def comparison_fraction_fraction(operator):
  """Generates a tester for ==, !=, <, >, <=, and >= two fractions."""
  name = f"fraction fraction {operator_to_str(operator)}"
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
                                 num_first, is_double, bits, signed):
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
    bits: how many bits to use for the int if comparing against an int
    signed: if the int is signed or unsigned if comparing against an int
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
    num_arg = random_int(num_sign, bits, signed)
  frac_arg = None
  if is_same:
    frac_arg = fractions.Fraction(num_arg)
  else:
    frac_arg = random_fraction(frac_sign)
  str1 = f"f{it}"
  str2 = num_arg if is_double else construct_int(num_arg, bits, signed)
  if num_first:
    str1 = num_arg if is_double else construct_int(num_arg, bits, signed)
    str2 = f"f{it}"
  print("  " + construct_fraction(frac_arg, it))

  result = None
  if num_first:
    result = perform_operation(num_arg, frac_arg, operator)
  else:
    result = perform_operation(frac_arg, num_arg, operator)

  print(f"  REQUIRE(({str1} {operator} {str2}) == {result});")

def comparison_fraction_num(operator, num_first, is_double, bits, signed):
  """Generates a tester for ==, !=, <, >, <=, and >= two fractions.

  Args:
    operator: ==, !=, <, >=, etc.
    num_first: True to create a tester for nums op fractions. False to create a
        tester for fractions op nums.
    is_double: True to create a tester for double numbers. False to create a
        tester for int numbers.
    bits: how many bits to use for the int if comparing against an int
    signed: if the int is signed or unsigned if comparing against an int
  """
  name = None
  if num_first:
    if is_double:
      name = f"double fraction {operator_to_str(operator)}"
    else:
      name = (f"{int_type_str(bits, signed)} fraction "
              f"{operator_to_str(operator)}")
  else:
    if is_double:
      name = f"fraction double {operator_to_str(operator)}"
    else:
      name = (f"fraction {operator_to_str(operator)} "
              f"{int_type_str(bits, signed)}")
  begin_test_case(name)
  it = 0
  sign_1_options = [1, -1, 0]
  sign_2_options = [1, -1, 0]
  if not signed and num_first and not is_double:
    sign_1_options = [1, 0]
  if not signed and not num_first and not is_double:
    sign_2_options = [1, 0]
  for sign_1 in sign_1_options:
    for sign_2 in sign_2_options:
      if ((sign_1 == 1 and sign_1 == sign_2) or
          (sign_1 == -1 and sign_1 == sign_2)):
        comparison_fraction_num_iter(operator, sign_1, sign_2, it, True,
                                     num_first, is_double, bits, signed)
        it += 1
      comparison_fraction_num_iter(operator, sign_1, sign_2, it, False,
                                   num_first, is_double, bits, signed)
      it += 1
  end_test_case(name)

def comparison_tester(operator):
  """Generates a tester for comparisons (==, !=, <, >, <=, >=)."""
  comparison_fraction_fraction(operator)
  for bits in [8, 16, 32, 64]:
    for signed in [True, False]:
      if not signed and bits == 64:
        continue
      comparison_fraction_num(operator, True, False, bits, signed)
      comparison_fraction_num(operator, False, False, bits, signed)
  comparison_fraction_num(operator, True, True, None, None)
  comparison_fraction_num(operator, False, True, None, None)

def int_conversion_iter(sign, bits, signed, it):
  """Single iterator for int_conversion.

  Args:
    sign: 1, -1, or 0 for the sign of the fraction to convert
    bits: how many bits to use for the int to convert to
    signed: if the int to convert to is signed or unsigned
    it: what number to start the fraction variable naming
  """
  print(f"  // {sign_to_str(sign)}")
  f1 = random_fraction(sign, bits, signed)
  print("  " + construct_fraction(f1, it))
  int_type = int_type_str(bits, signed)

  result = int(f1)

  print(f"  REQUIRE(static_cast<{int_type}>(f{it}) == {result});")

def int_conversion(bits, signed):
  """Generates a tester for int conversions.

  Args:
    bits: how many bits to use for the int to convert to
    signed: if the int to convert to is signed or unsigned
  """
  name = f"{int_type_str(bits, signed)} conversion"
  begin_test_case(name)
  sign_options = [1, -1, 0] if signed else [1, 0]
  it = 0
  for sign in sign_options:
    int_conversion_iter(sign, bits, signed, it)
    it += 1
  end_test_case(name)

def int_conversion_tester():
  """Generates a tester for int conversions."""
  for bits in [8, 16, 32, 64]:
    for signed in [True, False]:
      if not signed and bits == 64:
        continue
      int_conversion(bits, signed)

def main():
  parser = argparse.ArgumentParser(description="Generate tests for the C++ "
                                               "Fraction class.")
  parser.add_argument("operator", help="operator to generate tests for",
                      choices=["+", "-", "*", "/", "==", "!=", "<", ">", "<=",
                               ">=", "int_conversion"])
  args = parser.parse_args()
  if args.operator in ["+", "-", "*", "/"]:
    arithmetic_tester(args.operator)
  elif args.operator in ["==", "!=", "<", ">", "<=", ">="]:
    comparison_tester(args.operator)
  else:
    int_conversion_tester()

if __name__ == "__main__":
  main()
