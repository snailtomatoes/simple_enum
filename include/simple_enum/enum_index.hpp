#pragma once

#include <simple_enum/simple_enum.hpp>

#include "detail/static_call_operator_prolog.h"

#include <simple_enum/expected.h>

namespace simple_enum::inline v0_5
  {
using cxx23::bad_expected_access;
using cxx23::expected;
using cxx23::in_place;
using cxx23::in_place_t;
using cxx23::unexpect;
using cxx23::unexpect_t;
using cxx23::unexpected;

enum struct enum_index_error
  {
  out_of_range
  };

template<>
struct info<enum_index_error>
  {
  static constexpr auto first = enum_index_error::out_of_range;
  static constexpr auto last = enum_index_error::out_of_range;
  };

struct enum_index_t
  {
  template<enum_concept enum_type>
  [[nodiscard]]
  static_call_operator constexpr auto
    operator()(enum_type value) static_call_operator_const noexcept -> cxx23::expected<std::size_t, enum_index_error>
    {
    using enum_meta_info = detail::enum_meta_info_t<enum_type>;
    auto const requested_index{simple_enum::to_underlying(value)};
    if(requested_index >= enum_meta_info::first_index() && requested_index <= enum_meta_info::last_index()) [[likely]]
      return requested_index;
    else
      return cxx23::unexpected{enum_index_error::out_of_range};
    }
  };

inline constexpr enum_index_t enum_index;

template<enum_concept auto value>
consteval auto consteval_enum_index() -> std::size_t
  {
  return enum_index(value).or_else([](auto &&) { throw; });
  }

  }  // namespace simple_enum::inline v0_5

#include "detail/static_call_operator_epilog.h"
