// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/generic_error_category_impl.hpp>
#include <boost/ut.hpp>  // Ensure boost-ext/ut is available
#include <simple_enum/basic_fixed_string.hpp>

using namespace boost::ut;
using namespace simple_enum;
using namespace std::string_view_literals;

namespace
  {
suite basic_fixed_string_tests = []
{
  "basic_fixed_string content test"_test = []
  {
    constexpr basic_fixed_string my_string{"Hello"};
    expect(eq("Hello"sv, my_string.view())) << "String content does not match";
  };

  "basic_fixed_string size test"_test = []
  {
    constexpr basic_fixed_string my_string{"Test"};
    expect(4_u == my_string.size()) << "String size is incorrect";
  };
};

static constexpr basic_fixed_string my_error_str{"ErrorCategoryName"};
using my_error_cat_name_string_literal = string_literal<my_error_str>;

suite string_literal_tests = []
{
  "string_literal value test"_test = []
  {
    expect(eq("ErrorCategoryName"sv, my_error_cat_name_string_literal::value.view()))
      << "String literal value does not match";
    expect(17_u == my_error_cat_name_string_literal::value.size()) << "String literal size is incorrect";
  };
};
  }  // namespace
// Assuming the error category template and other necessary code is defined above

// Example enum for demonstration purposes
enum class test_error
  {
  success = 0,
  failed_other_reason,
  unknown
  };

consteval auto adl_enum_bounds(test_error)
  {
  using enum test_error;
  return simple_enum::adl_info{success, unknown};
  }

namespace std
  {
template<>
struct is_error_code_enum<test_error> : true_type
  {
  };
  }  // namespace std

// Test cases
static_assert(
  std::is_same_v<decltype(to_camel_case(basic_fixed_string{"hello"})), basic_fixed_string<char, 5>>,
  "Return type mismatch"
);
static_assert(
  to_camel_case(basic_fixed_string{"hello "}) == basic_fixed_string{"Hello "}, "Single word to CamelCase failed"
);
static_assert(
  to_camel_case(basic_fixed_string{"hello world"}) == basic_fixed_string{"Hello World"}, "Two words to CamelCase failed"
);
static_assert(to_camel_case(basic_fixed_string{" "}) == basic_fixed_string{" "}, "Single space handling failed");
static_assert(to_camel_case(basic_fixed_string{" a"}) == basic_fixed_string{" A"}, "Space then char failed");
static_assert(to_camel_case(basic_fixed_string{"a "}) == basic_fixed_string{"A "}, "Char then space failed");
static_assert(
  to_camel_case(basic_fixed_string{"hello world"}) != basic_fixed_string{"hello world"}, "Incorrect CamelCase equality"
);

namespace
  {
suite erorr_category_tests = []
{
  using namespace boost::ut;

  "generic_error_category_instance"_test = []
  {
    auto const & instance = generic_error_category<test_error>::instance();
    expect("Test Error"sv == instance.name());
  };

  "make_error_code_success"_test = []
  {
    auto ec = make_error_code(test_error::success);
    expect(eq(0, ec.value()));
    expect(eq("Success"sv, ec.message()));
  };

  "make_error_code_failed"_test = []
  {
    auto ec = make_error_code(test_error::failed_other_reason);
    expect(eq(1, ec.value()));
    expect(eq("Failed Other Reason"sv, ec.message()));
  };

  "make_error_code_unknown"_test = []
  {
    auto ec = make_error_code(test_error::unknown);
    expect(eq(2, ec.value()));
    expect(eq("Unknown"sv, ec.message()));
  };
};
  }  // namespace

int main() {}
