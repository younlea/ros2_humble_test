// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from message:msg/DataAB.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__MSG__DETAIL__DATA_AB__BUILDER_HPP_
#define MESSAGE__MSG__DETAIL__DATA_AB__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "message/msg/detail/data_ab__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace message
{

namespace msg
{

namespace builder
{

class Init_DataAB_data_ab
{
public:
  Init_DataAB_data_ab()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::message::msg::DataAB data_ab(::message::msg::DataAB::_data_ab_type arg)
  {
    msg_.data_ab = std::move(arg);
    return std::move(msg_);
  }

private:
  ::message::msg::DataAB msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::message::msg::DataAB>()
{
  return message::msg::builder::Init_DataAB_data_ab();
}

}  // namespace message

#endif  // MESSAGE__MSG__DETAIL__DATA_AB__BUILDER_HPP_
