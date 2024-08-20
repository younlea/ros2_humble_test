// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from message:srv/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_BA__STRUCT_HPP_
#define MESSAGE__SRV__DETAIL__DATA_BA__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__message__srv__DataBA_Request __attribute__((deprecated))
#else
# define DEPRECATED__message__srv__DataBA_Request __declspec(deprecated)
#endif

namespace message
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct DataBA_Request_
{
  using Type = DataBA_Request_<ContainerAllocator>;

  explicit DataBA_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->data_ba = 0l;
    }
  }

  explicit DataBA_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->data_ba = 0l;
    }
  }

  // field types and members
  using _data_ba_type =
    int32_t;
  _data_ba_type data_ba;

  // setters for named parameter idiom
  Type & set__data_ba(
    const int32_t & _arg)
  {
    this->data_ba = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    message::srv::DataBA_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const message::srv::DataBA_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<message::srv::DataBA_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<message::srv::DataBA_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      message::srv::DataBA_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataBA_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      message::srv::DataBA_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataBA_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<message::srv::DataBA_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<message::srv::DataBA_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__message__srv__DataBA_Request
    std::shared_ptr<message::srv::DataBA_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__message__srv__DataBA_Request
    std::shared_ptr<message::srv::DataBA_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DataBA_Request_ & other) const
  {
    if (this->data_ba != other.data_ba) {
      return false;
    }
    return true;
  }
  bool operator!=(const DataBA_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DataBA_Request_

// alias to use template instance with default allocator
using DataBA_Request =
  message::srv::DataBA_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace message


#ifndef _WIN32
# define DEPRECATED__message__srv__DataBA_Response __attribute__((deprecated))
#else
# define DEPRECATED__message__srv__DataBA_Response __declspec(deprecated)
#endif

namespace message
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct DataBA_Response_
{
  using Type = DataBA_Response_<ContainerAllocator>;

  explicit DataBA_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->response_data_ba = 0l;
    }
  }

  explicit DataBA_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->response_data_ba = 0l;
    }
  }

  // field types and members
  using _response_data_ba_type =
    int32_t;
  _response_data_ba_type response_data_ba;

  // setters for named parameter idiom
  Type & set__response_data_ba(
    const int32_t & _arg)
  {
    this->response_data_ba = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    message::srv::DataBA_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const message::srv::DataBA_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<message::srv::DataBA_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<message::srv::DataBA_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      message::srv::DataBA_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataBA_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      message::srv::DataBA_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<message::srv::DataBA_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<message::srv::DataBA_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<message::srv::DataBA_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__message__srv__DataBA_Response
    std::shared_ptr<message::srv::DataBA_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__message__srv__DataBA_Response
    std::shared_ptr<message::srv::DataBA_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DataBA_Response_ & other) const
  {
    if (this->response_data_ba != other.response_data_ba) {
      return false;
    }
    return true;
  }
  bool operator!=(const DataBA_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DataBA_Response_

// alias to use template instance with default allocator
using DataBA_Response =
  message::srv::DataBA_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace message

namespace message
{

namespace srv
{

struct DataBA
{
  using Request = message::srv::DataBA_Request;
  using Response = message::srv::DataBA_Response;
};

}  // namespace srv

}  // namespace message

#endif  // MESSAGE__SRV__DETAIL__DATA_BA__STRUCT_HPP_
