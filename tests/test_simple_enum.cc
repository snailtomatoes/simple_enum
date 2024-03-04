#include "simple_enum_tests.hpp"

enum struct enum_bounded
  {
  v1 = 1,
  v2,
  v3,
  // first = v1,
  // last = v3
  };

consteval auto adl_enum_bounds(enum_bounded) { return simple_enum::adl_info{enum_bounded::v1, enum_bounded::v3}; }

static_assert(simple_enum::detail::enum_meta_info_t<enum_bounded>::first() == enum_bounded::v1);
static_assert(simple_enum::detail::enum_meta_info_t<enum_bounded>::last() == enum_bounded::v3);

enum struct enum_upper_bounded
  {
  v1 = 1,
  v2,
  v3,
  last = v3
  };
static_assert(simple_enum::detail::enum_meta_info_t<enum_upper_bounded>::first() == static_cast<enum_upper_bounded>(0));
static_assert(simple_enum::detail::enum_meta_info_t<enum_upper_bounded>::last() == enum_upper_bounded::last);

enum struct enum_lower_bounded
  {
  v1 = 9,
  v2,
  first = v1
  };

static_assert(simple_enum::detail::enum_meta_info_t<enum_lower_bounded>::first() == enum_lower_bounded::v1);
static_assert(
  simple_enum::detail::enum_meta_info_t<enum_lower_bounded>::last()
  == static_cast<enum_lower_bounded>(simple_enum::default_unbounded_upper_range)
);

enum struct enum_unbounded
  {
  v1,
  v2,
  v3,
  vx,
  v5,
  v6a
  };

enum struct enum_unbounded_sparse
  {
  v1 = 9,
  v2
  };
static_assert(
  simple_enum::detail::enum_meta_info_t<enum_unbounded_sparse>::first() == static_cast<enum_unbounded_sparse>(0)
);
static_assert(
  simple_enum::detail::enum_meta_info_t<enum_unbounded_sparse>::last()
  == static_cast<enum_unbounded_sparse>(simple_enum::default_unbounded_upper_range)
);

enum weak_global_untyped_e
  {
  v1 = 1,
  v2,
  v3,
  };

consteval auto adl_enum_bounds(weak_global_untyped_e)
  {
  return simple_enum::adl_info{weak_global_untyped_e::v1, weak_global_untyped_e::v3};
  }

// check for external declarations
enum struct global_untyped_externaly_e
  {
  v1 = 1,
  v2,
  v3,
  };

template<>
struct simple_enum::info<global_untyped_externaly_e>
  {
  static constexpr auto first = global_untyped_externaly_e::v1;
  static constexpr auto last = global_untyped_externaly_e::v3;
  };

static_assert(simple_enum::detail::enum_meta_info_t<global_untyped_externaly_e>::first_index() == 1);
static_assert(simple_enum::detail::enum_meta_info_t<global_untyped_externaly_e>::last_index() == 3);

namespace simple_enum
  {

namespace ut = boost::ut;
enum struct strong_typed : int8_t
  {
  v1 = -121,
  v2,
  v3,
  first = v1,
  last = v3
  };
enum struct strong_untyped
  {
  v1 = 1500100900,
  v2,
  v3,
  first = v1,
  last = v3
  };

enum weak_typed_e : uint8_t
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };

namespace test
  {
  enum struct strong_typed_2_e : int16_t
    {
    v1 = 1,
    v2,
    v3,
    first = v1,
    last = v3
    };

  enum struct strong_untyped_2_e
    {
    v1 = 1,
    v2,
    v3,
    };

  consteval auto adl_enum_bounds(strong_untyped_2_e)
    {
    return simple_enum::adl_info{strong_untyped_2_e::v1, strong_untyped_2_e::v3};
    }

  namespace subnamespace
    {
    enum struct example_3_e : uint8_t
      {
      v1 = 192,
      v2,
      v3,
      first = v1,
      last = v3
      };

    enum weak_untyped_3_e
      {
      v1 = 1,
      v2,
      v3,
      first = v1,
      last = v3
      };

    namespace detail
      {
      namespace
        {
        enum struct example_4_e : uint8_t
          {
          v1 = 1,
          v2,
          v3,
          first = v1,
          last = v3
          };
        }
      }  // namespace detail

    inline namespace v2_0
      {
      namespace
        {
        enum struct example_5_e : uint8_t
          {
          v1 = 1,
          v2,
          v3,
          first = v1,
          last = v3
          };

        enum weak_typed_5_e : uint8_t
          {
          v1 = 1,
          v2,
          v3,
          first = v1,
          last = v3
          };
        }  // namespace
      }    // namespace v2_0
    }      // namespace subnamespace
  }        // namespace test

enum struct one_elem_untyped
  {
  v1 = 1,
  first = v1,
  last = v1
  };
enum struct sparse_untyped
  {
  v1 = 1,
  v3 = 3,
  first = v1,
  last = v3
  };
enum struct sparse_offseted_untyped
  {
  unknown = -1,
  v1 = 1,
  v3 = 3,
  first = unknown,  // simulate counting below the range
  last = v3
  };

using detail::cont_pass;
using detail::first_pass;
using detail::meta_name;

template<auto enumeration>
constexpr auto se_view() noexcept -> std::string_view
  {
  using enum_type = std::remove_cvref_t<decltype(enumeration)>;
  meta_name value{};
  size_t beg{first_pass<enum_type::first>(value)};
  cont_pass<enumeration>(value, beg);
  return std::string_view{value.data, value.size};
  }

static ut::suite<"simple_enum"> _ = []
{
  using namespace ut;
  "test enum_unbounded_sparse offseting"_test = []
  {
    constexpr auto v{static_cast<enum_unbounded_sparse>(0)};
    meta_name value{};
    auto offset{first_pass<v>(value)};
    cont_pass<v>(value, offset);
    cont_pass<enum_unbounded_sparse::v1>(value, offset);
    cont_pass<enum_unbounded_sparse::v2>(value, offset);
  };

  "test unbounded"_test = []
  {
    ut::expect(enum_name(enum_unbounded::v1) == "v1");
    ut::expect(enum_name(enum_unbounded::v2) == "v2");
    ut::expect(enum_name(enum_unbounded::v6a) == "v6a");

    ut::expect(enum_name(enum_unbounded_sparse::v1) == "v1");
    ut::expect(enum_name(enum_unbounded_sparse::v2) == "v2");
  };
  "test variations"_test = []
  {
    ut::expect(enum_name(one_elem_untyped::v1) == "v1");
    ut::expect(enum_name(static_cast<one_elem_untyped>(2)) == "");
    // TESTING UB dosabled
    // ut::expect(se_view<static_cast<sparse_untyped>(2)>() == "2");
    // ut::expect(enum_name(static_cast<sparse_untyped>(2)) == "2");
    ut::expect(enum_name(sparse_offseted_untyped::unknown) == "unknown");
    // TESTING UB dosabled
    // ut::expect(enum_name(static_cast<sparse_offseted_untyped>(0)) == "0");
  };

  "test se meta name cut"_test = []
  {
    // ut::expect(se_view<weak_global_untyped_e::v1>() == "v1");
    // it is out of range or on sparse enum just value when used directly with clang gcc
    // TESTING UB dosabled, testing when first pass for UB is not a case of lower unbounded
    // ut::expect(se_view<static_cast<one_elem_untyped>(2)>() == "2");

    ut::expect(se_view<strong_typed::v1>() == "v1");
    ut::expect(se_view<strong_typed::v2>() == "v2");
    ut::expect(se_view<strong_typed::v3>() == "v3");
    ut::expect(se_view<strong_typed::last>() == "v3");

    ut::expect(se_view<strong_untyped::v1>() == "v1");
    ut::expect(se_view<strong_untyped::v2>() == "v2");
    ut::expect(se_view<strong_untyped::v3>() == "v3");

    ut::expect(se_view<weak_typed_e::v1>() == "v1");
    ut::expect(se_view<weak_typed_e::v2>() == "v2");
    ut::expect(se_view<weak_typed_e::v3>() == "v3");

    ut::expect(se_view<v1>() == "v1");
    ut::expect(se_view<v2>() == "v2");
    ut::expect(se_view<v3>() == "v3");

    ut::expect(se_view<test::strong_typed_2_e::v1>() == "v1");
    ut::expect(se_view<test::strong_typed_2_e::v2>() == "v2");
    ut::expect(se_view<test::strong_typed_2_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::example_3_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::example_3_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::example_3_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::detail::example_4_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::detail::example_4_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::detail::example_4_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::example_5_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::example_5_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::example_5_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::weak_typed_5_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::weak_typed_5_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::weak_typed_5_e::v3>() == "v3");
  };
  "test enum name"_test = []
  {
    ut::expect(enum_name(weak_global_untyped_e::v1) == "v1");

    ut::expect(enum_name(strong_typed::v1) == "v1");
    ut::expect(enum_name(strong_typed::v2) == "v2");
    ut::expect(enum_name(strong_typed::v3) == "v3");

    ut::expect(enum_name(strong_untyped::v1) == "v1");
    ut::expect(enum_name(strong_untyped::v2) == "v2");
    ut::expect(enum_name(strong_untyped::v3) == "v3");

    ut::expect(enum_name(weak_typed_e::v1) == "v1");
    ut::expect(enum_name(weak_typed_e::v2) == "v2");
    ut::expect(enum_name(weak_typed_e::v3) == "v3");

    ut::expect(enum_name(v1) == "v1");
    ut::expect(enum_name(v2) == "v2");
    ut::expect(enum_name(v3) == "v3");

    ut::expect(enum_name(test::strong_typed_2_e::v1) == "v1");
    ut::expect(enum_name(test::strong_typed_2_e::v2) == "v2");
    ut::expect(enum_name(test::strong_typed_2_e::v3) == "v3");

    ut::expect(enum_name(test::subnamespace::example_3_e::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::example_3_e::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::example_3_e::v3) == "v3");

    ut::expect(enum_name(test::subnamespace::detail::example_4_e::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::detail::example_4_e::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::detail::example_4_e::v3) == "v3");

    ut::expect(enum_name(test::subnamespace::example_5_e::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::example_5_e::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::example_5_e::v3) == "v3");
    ut::expect(enum_name(test::subnamespace::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::v3) == "v3");
  };
};
  }  // namespace simple_enum

int main() { se::verify_offset(); }
