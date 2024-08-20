// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from message:srv/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_BA__TRAITS_HPP_
#define MESSAGE__SRV__DETAIL__DATA_BA__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "message/srv/detail/data_ba__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace message
{

namespace srv
{

inline void to_flow_style_yaml(
  const DataBA_Request & msg,
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
  const DataBA_Request & msg,
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

inline std::string to_yaml(const DataBA_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace message

namespace rosidl_generator_traits
{

[[deprecated("use message::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const message::srv::DataBA_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  message::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use message::srv::to_yaml() instead")]]
inline std::string to_yaml(const message::srv::DataBA_Request & msg)
{
  return message::srv::to_yaml(msg);
}

template<>
inline const char * data_type<message::srv::DataBA_Request>()
{
  return "message::srv::DataBA_Request";
}

template<>
inline const char * name<message::srv::DataBA_Request>()
{
  return "message/srv/DataBA_Request";
}

template<>
struct has_fixed_size<message::srv::DataBA_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<message::srv::DataBA_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<message::srv::DataBA_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace message
{

namespace srv
{

inline void to_flow_style_yaml(
  const DataBA_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: response_data_ba
  {
    out << "response_data_ba: ";
    rosidl_generator_traits::value_to_yaml(msg.response_data_ba, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DataBA_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: response_data_ba
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "response_data_ba: ";
    rosidl_generator_traits::value_to_yaml(msg.response_data_ba, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DataBA_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace message

namespace rosidl_generator_traits
{

[[deprecated("use message::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const message::srv::DataBA_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  message::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use message::srv::to_yaml() instead")]]
inline std::string to_yaml(const message::srv::DataBA_Response & msg)
{
  return message::srv::to_yaml(msg);
}

template<>
inline const char * data_type<message::srv::DataBA_Response>()
{
  return "message::srv::DataBA_Response";
}

template<>
inline const char * name<message::srv::DataBA_Response>()
{
  return "message/srv/DataBA_Response";
}

template<>
struct has_fixed_size<message::srv::DataBA_Response>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<message::srv::DataBA_Response>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<message::srv::DataBA_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<message::srv::DataBA>()
{
  return "message::srv::DataBA";
}

template<>
inline const char * name<message::srv::DataBA>()
{
  return "message/srv/DataBA";
}

template<>
struct has_fixed_size<message::srv::DataBA>
  : std::integral_constant<
    bool,
    has_fixed_size<message::srv::DataBA_Request>::value &&
    has_fixed_size<message::srv::DataBA_Response>::value
  >
{
};

template<>
struct has_bounded_size<message::srv::DataBA>
  : std::integral_constant<
    bool,
    has_bounded_size<message::srv::DataBA_Request>::value &&
    has_bounded_size<message::srv::DataBA_Response>::value
  >
{
};

template<>
struct is_service<message::srv::DataBA>
  : std::true_type
{
};

template<>
struct is_service_request<message::srv::DataBA_Request>
  : std::true_type
{
};

template<>
struct is_service_response<message::srv::DataBA_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // MESSAGE__SRV__DETAIL__DATA_BA__TRAITS_HPP_
