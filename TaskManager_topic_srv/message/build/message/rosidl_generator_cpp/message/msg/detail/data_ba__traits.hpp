// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from message:msg/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__MSG__DETAIL__DATA_BA__TRAITS_HPP_
#define MESSAGE__MSG__DETAIL__DATA_BA__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "message/msg/detail/data_ba__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace message
{

namespace msg
{

inline void to_flow_style_yaml(
  const DataBA & msg,
  std::ostream & out)
{
  out << "{";
  // member: data_ba
  {
    out << "data_ba: ";
    rosidl_generator_traits::value_to_yaml(msg.data_ba, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DataBA & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: data_ba
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "data_ba: ";
    rosidl_generator_traits::value_to_yaml(msg.data_ba, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DataBA & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace message

namespace rosidl_generator_traits
{

[[deprecated("use message::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const message::msg::DataBA & msg,
  std::ostream & out, size_t indentation = 0)
{
  message::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use message::msg::to_yaml() instead")]]
inline std::string to_yaml(const message::msg::DataBA & msg)
{
  return message::msg::to_yaml(msg);
}

template<>
inline const char * data_type<message::msg::DataBA>()
{
  return "message::msg::DataBA";
}

template<>
inline const char * name<message::msg::DataBA>()
{
  return "message/msg/DataBA";
}

template<>
struct has_fixed_size<message::msg::DataBA>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<message::msg::DataBA>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<message::msg::DataBA>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MESSAGE__MSG__DETAIL__DATA_BA__TRAITS_HPP_
