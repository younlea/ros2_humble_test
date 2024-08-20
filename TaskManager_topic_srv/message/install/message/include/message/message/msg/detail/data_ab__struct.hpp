// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from message:msg/DataAB.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__MSG__DETAIL__DATA_AB__STRUCT_HPP_
#define MESSAGE__MSG__DETAIL__DATA_AB__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__message__msg__DataAB __attribute__((deprecated))
#else
# define DEPRECATED__message__msg__DataAB __declspec(deprecated)
#endif

namespace message
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct DataAB_
{
  using Type = DataAB_<ContainerAllocator>;

  explicit DataAB_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->data_ab = 0l;
    }
  }

  explicit DataAB_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->data_ab = 0l;
    }
  }

  // field types and members
  using _data_ab_type =
    int32_t;
  _data_ab_type data_ab;

  // setters for named parameter idiom
  Type & set__data_ab(
    const int32_t & _arg)
  {
    this->data_ab = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    message::msg::DataAB_<ContainerAllocator> *;
  using ConstRawPtr =
    const message::msg::DataAB_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<message::msg::DataAB_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<message::msg::DataAB_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      message::msg::DataAB_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<message::msg::DataAB_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      message::msg::DataAB_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<message::msg::DataAB_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<message::msg::DataAB_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<message::msg::DataAB_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__message__msg__DataAB
    std::shared_ptr<message::msg::DataAB_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__message__msg__DataAB
    std::shared_ptr<message::msg::DataAB_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DataAB_ & other) const
  {
    if (this->data_ab != other.data_ab) {
      return false;
    }
    return true;
  }
  bool operator!=(const DataAB_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DataAB_

// alias to use template instance with default allocator
using DataAB =
  message::msg::DataAB_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace message

#endif  // MESSAGE__MSG__DETAIL__DATA_AB__STRUCT_HPP_
