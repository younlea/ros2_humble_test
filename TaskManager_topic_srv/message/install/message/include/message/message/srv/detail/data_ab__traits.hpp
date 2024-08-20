// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from message:srv/DataAB.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_AB__TRAITS_HPP_
#define MESSAGE__SRV__DETAIL__DATA_AB__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "message/srv/detail/data_ab__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace message
{

namespace srv
{

inline void to_flow_style_yaml(
  const DataAB_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: data_ab
  {
    out << "data_ab: ";
    rosidl_generator_traits::value_to_yaml(msg.data_ab, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DataAB_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: data_ab
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "data_ab: ";
    rosidl_generator_traits::value_to_yaml(msg.data_ab, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DataAB_Request & msg, bool use_flow_style = false)
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
  const message::srv::DataAB_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  message::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use message::srv::to_yaml() instead")]]
inline std::string to_yaml(const message::srv::DataAB_Request & msg)
{
  return message::srv::to_yaml(msg);
}

template<>
inline const char * data_type<message::srv::DataAB_Request>()
{
  return "message::srv::DataAB_Request";
}

template<>
inline const char * name<message::srv::DataAB_Request>()
{
  return "message/srv/DataAB_Request";
}

template<>
struct has_fixed_size<message::srv::DataAB_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<message::srv::DataAB_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<message::srv::DataAB_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace message
{

namespace srv
{

inline void to_flow_style_yaml(
  const DataAB_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: response_data_ab
  {
    out << "response_data_ab: ";
    rosidl_generator_traits::value_to_yaml(msg.response_data_ab, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DataAB_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: response_data_ab
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "response_data_ab: ";
    rosidl_generator_traits::value_to_yaml(msg.response_data_ab, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DataAB_Response & msg, bool use_flow_style = false)
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
  const message::srv::DataAB_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  message::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use message::srv::to_yaml() instead")]]
inline std::string to_yaml(const message::srv::DataAB_Response & msg)
{
  return message::srv::to_yaml(msg);
}

template<>
inline const char * data_type<message::srv::DataAB_Response>()
{
  return "message::srv::DataAB_Response";
}

template<>
inline const char * name<message::srv::DataAB_Response>()
{
  return "message/srv/DataAB_Response";
}

template<>
struct has_fixed_size<message::srv::DataAB_Response>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<message::srv::DataAB_Response>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<message::srv::DataAB_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<message::srv::DataAB>()
{
  return "message::srv::DataAB";
}

template<>
inline const char * name<message::srv::DataAB>()
{
  return "message/srv/DataAB";
}

template<>
struct has_fixed_size<message::srv::DataAB>
  : std::integral_constant<
    bool,
    has_fixed_size<message::srv::DataAB_Request>::value &&
    has_fixed_size<message::srv::DataAB_Response>::value
  >
{
};

template<>
struct has_bounded_size<message::srv::DataAB>
  : std::integral_constant<
    bool,
    has_bounded_size<message::srv::DataAB_Request>::value &&
    has_bounded_size<message::srv::DataAB_Response>::value
  >
{
};

template<>
struct is_service<message::srv::DataAB>
  : std::true_type
{
};

template<>
struct is_service_request<message::srv::DataAB_Request>
  : std::true_type
{
};

template<>
struct is_service_response<message::srv::DataAB_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // MESSAGE__SRV__DETAIL__DATA_AB__TRAITS_HPP_
