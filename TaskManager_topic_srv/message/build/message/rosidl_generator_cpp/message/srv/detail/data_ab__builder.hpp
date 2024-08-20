// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from message:srv/DataAB.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_AB__BUILDER_HPP_
#define MESSAGE__SRV__DETAIL__DATA_AB__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "message/srv/detail/data_ab__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace message
{

namespace srv
{

namespace builder
{

class Init_DataAB_Request_data_ab
{
public:
  Init_DataAB_Request_data_ab()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::message::srv::DataAB_Request data_ab(::message::srv::DataAB_Request::_data_ab_type arg)
  {
    msg_.data_ab = std::move(arg);
    return std::move(msg_);
  }

private:
  ::message::srv::DataAB_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::message::srv::DataAB_Request>()
{
  return message::srv::builder::Init_DataAB_Request_data_ab();
}

}  // namespace message


namespace message
{

namespace srv
{

namespace builder
{

class Init_DataAB_Response_response_data_ab
{
public:
  Init_DataAB_Response_response_data_ab()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::message::srv::DataAB_Response response_data_ab(::message::srv::DataAB_Response::_response_data_ab_type arg)
  {
    msg_.response_data_ab = std::move(arg);
    return std::move(msg_);
  }

private:
  ::message::srv::DataAB_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::message::srv::DataAB_Response>()
{
  return message::srv::builder::Init_DataAB_Response_response_data_ab();
}

}  // namespace message

#endif  // MESSAGE__SRV__DETAIL__DATA_AB__BUILDER_HPP_
