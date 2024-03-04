#pragma once

#include <concepts>
#include <type_traits>
#include <utility>
#include <cstdint>
#include <string_view>
#if defined(__cpp_lib_source_location)
#include <source_location>
#endif
#include <array>

#define SIMPLE_ENUM_NAME_VERSION "0.5.5"

#include "detail/static_call_operator_prolog.h"

namespace simple_enum::inline v0_5
  {
template<typename Enum>
constexpr auto to_underlying(Enum e) noexcept -> std::underlying_type_t<Enum>
  {
  return static_cast<std::underlying_type_t<Enum>>(e);
  }

// clang-format off
template<typename type>
concept enum_concept = std::is_enum_v<type>;

// clang-format on

#ifndef SIMPLE_ENUM_CUSTOM_UNBOUNDED_RANGE
#define SIMPLE_ENUM_CUSTOM_UNBOUNDED_RANGE
inline constexpr auto default_unbounded_upper_range = 10;
#endif

///\brief info class is intended to custom specialize by users if they wish to add external enum bounds info
template<typename enumeration>
struct info
  {
  };

///\brief info function to use with ADL is intended to custom specialize by users if they wish to add external enum
/// bounds info
/// user overload should return std::initializer list with 2 elements first,last of any std::integral type
/// consteval auto adl_enum_bounds( my_enum ) { return simple_enum::adl_info{my_enum::v1, my_enum::v3}; }

template<typename enumeration>
struct adl_info
  {
  enumeration first;
  enumeration last;
  };

template<typename enumeration>
adl_info(enumeration const &, enumeration const &) -> adl_info<enumeration>;

template<typename enumeration>
constexpr auto adl_enum_bounds() -> void;

// Concept to check if adl_enum_bounds returns std::initializer_list<enumeration>
template<typename enumeration>
concept has_valid_adl_enum_bounds = requires(enumeration e) {
  {
  adl_enum_bounds(e)
  } -> std::same_as<adl_info<enumeration>>;
};

namespace detail
  {

  template<typename T>
  concept lower_bounded_enum = requires(T e) {
    requires enum_concept<T>;
      {
      T::first
      } -> std::convertible_to<T>;
  };
  template<typename T>
  concept upper_bounded_enum = requires(T e) {
    requires enum_concept<T>;
      {
      T::last
      } -> std::convertible_to<T>;
  };

  template<typename T>
  concept internaly_bounded_enum = requires(T e) {
    requires enum_concept<T>;
    requires lower_bounded_enum<T>;
    requires upper_bounded_enum<T>;
    requires(simple_enum::to_underlying(T::last) >= simple_enum::to_underlying(T::first));
  };
  // clang-format off
  template<typename enumeration>
  concept has_info_specialization = requires {
    { info<enumeration>::first } -> std::convertible_to<decltype(info<enumeration>::last)>;
    { info<enumeration>::last } -> std::convertible_to<decltype(info<enumeration>::first)>;

    requires info<enumeration>::first <= info<enumeration>::last;
    };

  // clang-format on
  struct meta_info_bounds_traits
    {
    bool lower_bound;
    bool upper_bound;
    };

  template<typename enumeration, meta_info_bounds_traits bi = meta_info_bounds_traits{}>
  struct meta_info : std::false_type
    {
    static constexpr enumeration first = static_cast<enumeration>(0);
    static constexpr enumeration last = static_cast<enumeration>(default_unbounded_upper_range);
    };

  template<typename enumeration>
  struct meta_info<enumeration, meta_info_bounds_traits{.lower_bound = true, .upper_bound = true}>
    {
    static constexpr enumeration first = enumeration::first;
    static constexpr enumeration last = enumeration::last;
    };

  // default assume range 0 - 10
  template<typename enumeration>
  struct meta_info<enumeration, meta_info_bounds_traits{.lower_bound = true, .upper_bound = false}>
    {
    static constexpr enumeration first = enumeration::first;
    static constexpr enumeration last = static_cast<enumeration>(default_unbounded_upper_range);
    };

  template<typename enumeration>
  struct meta_info<enumeration, meta_info_bounds_traits{.lower_bound = false, .upper_bound = true}>
    {
    static constexpr enumeration first = static_cast<enumeration>(0);
    static constexpr enumeration last = enumeration::last;
    };

  // strange compile time error on msvc when meta_info_bounds_traits are used directly in template arguments
  template<typename enumeration>
  struct msvc_meta_info_wrapper
    {
    static constexpr meta_info_bounds_traits
      traits{.lower_bound = lower_bounded_enum<enumeration>, .upper_bound = upper_bounded_enum<enumeration>};
    using type = meta_info<enumeration, traits>;
    };

  template<typename enumeration>
  consteval auto get_meta_info()
    {
    if constexpr(has_valid_adl_enum_bounds<enumeration>)
      return adl_enum_bounds(enumeration{});
    else if constexpr(has_info_specialization<enumeration>)
      return info<enumeration>{};
    else
      return typename msvc_meta_info_wrapper<enumeration>::type{};
    }
  }  // namespace detail
  }  // namespace simple_enum::inline v0_5

// this namespace is for reducing time crunching source location and .text data
namespace se
  {
template<auto enumeration>
constexpr auto f() noexcept
  {
#if defined(__clang__) || defined(__GNUC__)
  char const * const func{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  char const * const func{__FUNCSIG__};
#elif defined(__cpp_lib_source_location)
  char const * const func{std::source_location::current().function_name()};
#else
#error "Can not help you not a clang, gcc or msvc"
#endif
  return func;
  }

#if defined(__clang__)
#if __clang_major__ < 13
#error "Clang minimal supported version is 13"
#endif
// This is just for speeding up user use cases to skip computation of strlen at compile time

// index of the = character in the given line "auto se::f() [enumeration =" is 26
inline constexpr auto initial_offset{26 + 1};
inline constexpr auto end_of_enumeration_name = ']';

#elif defined(__GNUC__)
#if __GNUC__ < 10
#error "Gcc minimal supported version is 10"
#endif
// index of the = character in the given line "constexpr auto se::f() [with auto enumeration =" is 46.
inline constexpr auto initial_offset{46 + 1};
inline constexpr char end_of_enumeration_name = ']';

#elif defined(_MSC_VER)
// index of the < character in the given line "auto __cdecl se::f<" is 18
//                         0x00007ff76b5d33b0 "auto __cdecl se::f<v1>(void) noexcept"
inline constexpr auto initial_offset{18};
inline constexpr char end_of_enumeration_name = '>';
#else
#error "supply information to author about Your compiler"
#endif

#ifdef SIMPLE_ENUM_OPT_IN_STATIC_ASSERTS
// OPT IN TESTING CODE
enum struct verify_ennum_
  {
  v1
  };

constexpr size_t find_enumeration_offset()
  {
  auto const func{std::string_view{f<verify_ennum_::v1>()}};
#if defined(_MSC_VER)
  size_t pos = func.find('<');
  if(pos == std::string_view::npos)
    throw;
  return pos;
#else
  size_t pos = func.find("enumeration =");
  if(pos == std::string_view::npos)
    throw;
  return pos + 12 + 1;
#endif
  }

auto constexpr verify_offset() -> bool { return find_enumeration_offset() == initial_offset; }

static_assert(verify_offset());
#endif
  }  // namespace se

namespace simple_enum::inline v0_5
  {
namespace detail
  {
  struct meta_name
    {
    char const * data;
    size_t size;

    constexpr auto as_view() const noexcept -> std::string_view { return std::string_view{data, size}; }

    constexpr auto operator==(meta_name r) const noexcept -> bool { return as_view() == r.as_view(); }

    constexpr auto operator<=>(meta_name r) const noexcept { return as_view() <=> r.as_view(); }
    };

  template<auto enumeration>
  constexpr auto first_pass(meta_name & res) noexcept
    {
    char const * const func{se::f<enumeration>()};
    char const * end_of_name{func + se::initial_offset};
    char const * last_colon{end_of_name};
#ifdef _MSC_VER
    size_t was_undefined{};
#endif
    for(; *end_of_name != se::end_of_enumeration_name; ++end_of_name)
#ifdef _MSC_VER
      if(*end_of_name == ':')
        last_colon = end_of_name;
      else if(*end_of_name == ')')
        {
        last_colon = end_of_name;
        was_undefined = 5;
        }
#else
      if(*end_of_name == ':' || *end_of_name == ')')
        last_colon = end_of_name;
#endif

    res.data = last_colon + 1;
    res.size = size_t(end_of_name - res.data);
#ifdef _MSC_VER
    return size_t(last_colon - func) + 1 - was_undefined;
#else
    return size_t(last_colon - func) + 1;
#endif
    }

  template<auto enumeration>
  constexpr void cont_pass(meta_name & res, std::size_t enum_beg) noexcept
    {
    char const * const func{se::f<enumeration>()};
    char const * end_of_name{func + enum_beg};
    char const * enumeration_name{end_of_name};
    while(*end_of_name != se::end_of_enumeration_name)
      ++end_of_name;  // for other enumerations we only need to find end of string

    res.data = enumeration_name;
    res.size = size_t(end_of_name - res.data);
    }

  template<typename enum_type, std::integral auto first, std::size_t size, typename name_array, std::size_t... indices>
  constexpr void apply_meta_enum(name_array & meta, size_t enum_beg, std::index_sequence<indices...>)
    {
    // unpack and call cont_pass for each index, using fold expression
    using utype = std::underlying_type_t<enum_type>;
    (..., (cont_pass<static_cast<enum_type>(first + utype(indices))>(meta[indices + 1], enum_beg)));
    }

  template<typename enum_type, std::integral auto first, std::integral auto last, std::size_t size, typename name_array>
  constexpr void fold_array(name_array & meta)
    {
#ifdef SIMPLE_ENUM_OPT_IN_STATIC_ASSERTS
    static_assert(size == static_cast<std::size_t>(last - first + 1), "size must match the number of enum values");
#endif
    size_t enum_beg{first_pass<static_cast<enum_type>(first)>(meta[0])};
    if constexpr(size > 1)
      apply_meta_enum<enum_type, first + 1, size - 1>(meta, enum_beg, std::make_index_sequence<size - 1>{});
    }

  template<enum_concept enum_type, std::integral auto first_index, std::integral auto last_index>
  constexpr auto prepare_meta_data() noexcept -> std::array<detail::meta_name, last_index - first_index + 1>
    {
    std::array<detail::meta_name, last_index - first_index + 1> meta;
    detail::fold_array<enum_type, first_index, last_index, last_index - first_index + 1>(meta);
    return meta;
    }

  template<enum_concept enum_type>
  struct enum_meta_info_t
    {
    static constexpr auto emum_info_ = get_meta_info<enum_type>();

    static constexpr auto first() noexcept { return emum_info_.first; }

    static constexpr auto last() noexcept { return emum_info_.last; }

    static constexpr auto first_index() noexcept { return simple_enum::to_underlying(emum_info_.first); }

    static constexpr auto last_index() noexcept { return simple_enum::to_underlying(emum_info_.last); }

    static constexpr auto meta_data{detail::prepare_meta_data<enum_type, first_index(), last_index()>()};

    static constexpr auto size() noexcept -> std::size_t { return last_index() - first_index() + 1; }
    };

  }  // namespace detail

template<typename enumeration>
concept bounded_enum = has_valid_adl_enum_bounds<enumeration> || detail::has_info_specialization<enumeration>
                       || detail::internaly_bounded_enum<enumeration>;

struct enum_name_t
  {
  template<enum_concept enum_type>
  static_call_operator constexpr auto operator()(enum_type value) static_call_operator_const noexcept
    -> std::string_view
    {
    using enum_meta_info = detail::enum_meta_info_t<enum_type>;
    auto const requested_index{simple_enum::to_underlying(value)};
    if(requested_index >= enum_meta_info::first_index() && requested_index <= enum_meta_info::last_index())
      {
      detail::meta_name const & res{enum_meta_info::meta_data[size_t(requested_index - enum_meta_info::first_index())]};
      return std::string_view{res.data, res.size};
      }
    else
      return {""};  // return empty but null terminated
    }
  };

inline constexpr enum_name_t enum_name;

  }  // namespace simple_enum::inline v0_5

#include "detail/static_call_operator_epilog.h"

