#include "simple_enum/simple_enum.hpp"
#include <boost/ut.hpp>

enum weak_global_untyped_e
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };

namespace simple_enum
  {
namespace ut = boost::ut;
enum struct strong_typed : uint8_t
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };
enum struct strong_untyped
  {
  v1 = 1,
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
    first = v1,
    last = v3
    };

  namespace subnamespace
    {
    enum struct example_3_e : uint8_t
      {
      v1 = 1,
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

// invalid
// auto fast_enum_name_raw() [enumeration = (example_enum_e)5]
// auto fast_enum_name() [enumeration = example_enum_e::v1]
// auto fast_enum_name() [enumeration = test::example_enum_e::v1]
// auto fast_enum_name() [enumeration = test::subnamespace::example_enum_e::v1]
// auto fast_enum_name() [enumeration = test::subnamespace::detail::example_enum_e::v1]
// auto fast_enum_name() [enumeration = test::subnamespace::v2::(anonymous namespace)::example_enum_e::v1]
template<auto enumeration>
constexpr auto se_view() noexcept -> std::string_view
  {
  using enum_type = std::remove_cvref_t<decltype(enumeration)>;
  meta_name value{};
  size_t beg{se::b<enum_type::first>(value)};
  se::e<enumeration>(value, beg);
  return std::string_view{value.data, value.size};
  }

static ut::suite<"simple_enum"> _ = []
{
  ut::expect(se_view<static_cast<sparse_untyped>(2)>() == "2");
  ut::expect(enum_name(static_cast<sparse_untyped>(2)) == "2");
  using namespace ut;
  "test variations"_test = []
  {
    ut::expect(enum_name(one_elem_untyped::v1) == "v1");
    ut::expect(enum_name(static_cast<one_elem_untyped>(2)) == "");
  };

  "test se meta name cut"_test = []
  {
    ut::expect(se_view<weak_global_untyped_e::v1>() == "v1");
    ut::expect(
      se_view<static_cast<one_elem_untyped>(2)>() == "2"
    );  // it is out of range or on sparse enum just value when used directly
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

int main()
  {
  auto p = __builtin_FUNCTION();
  auto e = __PRETTY_FUNCTION__;
  }
