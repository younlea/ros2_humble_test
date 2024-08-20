// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from message:msg/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__MSG__DETAIL__DATA_BA__BUILDER_HPP_
#define MESSAGE__MSG__DETAIL__DATA_BA__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "message/msg/detail/data_ba__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace message
{

namespace msg
{

namespace builder
{

class Init_DataBA_data_ba
{
public:
  Init_DataBA_data_ba()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::message::msg::DataBA data_ba(::message::msg::DataBA::_data_ba_type arg)
  {
    msg_.data_ba = std::move(arg);
    return std::move(msg_);
  }

private:
  ::message::msg::DataBA msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::message::msg::DataBA>()
{
  return message::msg::builder::Init_DataBA_data_ba();
}

}  // namespace message

#endif  // MESSAGE__MSG__DETAIL__DATA_BA__BUILDER_HPP_
