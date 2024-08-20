// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from message:srv/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_BA__BUILDER_HPP_
#define MESSAGE__SRV__DETAIL__DATA_BA__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "message/srv/detail/data_ba__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace message
{

namespace srv
{

namespace builder
{

class Init_DataBA_Request_data_ba
{
public:
  Init_DataBA_Request_data_ba()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::message::srv::DataBA_Request data_ba(::message::srv::DataBA_Request::_data_ba_type arg)
  {
    msg_.data_ba = std::move(arg);
    return std::move(msg_);
  }

private:
  ::message::srv::DataBA_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::message::srv::DataBA_Request>()
{
  return message::srv::builder::Init_DataBA_Request_data_ba();
}

}  // namespace message


namespace message
{

namespace srv
{

namespace builder
{

class Init_DataBA_Response_response_data_ba
{
public:
  Init_DataBA_Response_response_data_ba()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::message::srv::DataBA_Response response_data_ba(::message::srv::DataBA_Response::_response_data_ba_type arg)
  {
    msg_.response_data_ba = std::move(arg);
    return std::move(msg_);
  }

private:
  ::message::srv::DataBA_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::message::srv::DataBA_Response>()
{
  return message::srv::builder::Init_DataBA_Response_response_data_ba();
}

}  // namespace message

#endif  // MESSAGE__SRV__DETAIL__DATA_BA__BUILDER_HPP_
