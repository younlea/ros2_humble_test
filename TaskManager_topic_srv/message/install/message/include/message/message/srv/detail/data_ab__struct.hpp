// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from message:srv/DataAB.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_AB__STRUCT_HPP_
#define MESSAGE__SRV__DETAIL__DATA_AB__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__message__srv__DataAB_Request __attribute__((deprecated))
#else
# define DEPRECATED__message__srv__DataAB_Request __declspec(deprecated)
#endif

namespace message
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct DataAB_Request_
{
  using Type = DataAB_Request_<ContainerAllocator>;

  explicit DataAB_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->data_ab = 0l;
    }
  }

  explicit DataAB_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
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
    message::srv::DataAB_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const message::srv::DataAB_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<message::srv::DataAB_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<message::srv::DataAB_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      message::srv::DataAB_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataAB_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      message::srv::DataAB_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataAB_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<message::srv::DataAB_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<message::srv::DataAB_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__message__srv__DataAB_Request
    std::shared_ptr<message::srv::DataAB_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__message__srv__DataAB_Request
    std::shared_ptr<message::srv::DataAB_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DataAB_Request_ & other) const
  {
    if (this->data_ab != other.data_ab) {
      return false;
    }
    return true;
  }
  bool operator!=(const DataAB_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DataAB_Request_

// alias to use template instance with default allocator
using DataAB_Request =
  message::srv::DataAB_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace message


#ifndef _WIN32
# define DEPRECATED__message__srv__DataAB_Response __attribute__((deprecated))
#else
# define DEPRECATED__message__srv__DataAB_Response __declspec(deprecated)
#endif

namespace message
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct DataAB_Response_
{
  using Type = DataAB_Response_<ContainerAllocator>;

  explicit DataAB_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->response_data_ab = 0l;
    }
  }

  explicit DataAB_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->response_data_ab = 0l;
    }
  }

  // field types and members
  using _response_data_ab_type =
    int32_t;
  _response_data_ab_type response_data_ab;

  // setters for named parameter idiom
  Type & set__response_data_ab(
    const int32_t & _arg)
  {
    this->response_data_ab = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    message::srv::DataAB_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const message::srv::DataAB_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<message::srv::DataAB_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<message::srv::DataAB_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      message::srv::DataAB_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataAB_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      message::srv::DataAB_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataAB_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<message::srv::DataAB_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<message::srv::DataAB_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__message__srv__DataAB_Response
    std::shared_ptr<message::srv::DataAB_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__message__srv__DataAB_Response
    std::shared_ptr<message::srv::DataAB_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DataAB_Response_ & other) const
  {
    if (this->response_data_ab != other.response_data_ab) {
      return false;
    }
    return true;
  }
  bool operator!=(const DataAB_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DataAB_Response_

// alias to use template instance with default allocator
using DataAB_Response =
  message::srv::DataAB_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace message

namespace message
{

namespace srv
{

struct DataAB
{
  using Request = message::srv::DataAB_Request;
  using Response = message::srv::DataAB_Response;
};

}  // namespace srv

}  // namespace message

#endif  // MESSAGE__SRV__DETAIL__DATA_AB__STRUCT_HPP_
