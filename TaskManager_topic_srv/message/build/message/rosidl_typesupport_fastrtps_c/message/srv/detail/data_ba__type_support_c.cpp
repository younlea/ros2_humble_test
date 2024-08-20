// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from message:srv/DataBA.idl
// generated code does not contain a copyright notice
#include "message/srv/detail/data_ba__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "message/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "message/srv/detail/data_ba__struct.h"
#include "message/srv/detail/data_ba__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif


// forward declare type support functions


using _DataBA_Request__ros_msg_type = message__srv__DataBA_Request;

static bool _DataBA_Request__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _DataBA_Request__ros_msg_type * ros_message = static_cast<const _DataBA_Request__ros_msg_type *>(untyped_ros_message);
  // Field name: data_ba
  {
    cdr << ros_message->data_ba;
  }

  return true;
}

static bool _DataBA_Request__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _DataBA_Request__ros_msg_type * ros_message = static_cast<_DataBA_Request__ros_msg_type *>(untyped_ros_message);
  // Field name: data_ba
  {
    cdr >> ros_message->data_ba;
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_message
size_t get_serialized_size_message__srv__DataBA_Request(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _DataBA_Request__ros_msg_type * ros_message = static_cast<const _DataBA_Request__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name data_ba
  {
    size_t item_size = sizeof(ros_message->data_ba);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

static uint32_t _DataBA_Request__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_message__srv__DataBA_Request(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_message
size_t max_serialized_size_message__srv__DataBA_Request(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: data_ba
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = message__srv__DataBA_Request;
    is_plain =
      (
      offsetof(DataType, data_ba) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _DataBA_Request__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_message__srv__DataBA_Request(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_DataBA_Request = {
  "message::srv",
  "DataBA_Request",
  _DataBA_Request__cdr_serialize,
  _DataBA_Request__cdr_deserialize,
  _DataBA_Request__get_serialized_size,
  _DataBA_Request__max_serialized_size
};

static rosidl_message_type_support_t _DataBA_Request__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_DataBA_Request,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataBA_Request)() {
  return &_DataBA_Request__type_support;
}

#if defined(__cplusplus)
}
#endif

// already included above
// #include <cassert>
// already included above
// #include <limits>
// already included above
// #include <string>
// already included above
// #include "rosidl_typesupport_fastrtps_c/identifier.h"
// already included above
// #include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
// already included above
// #include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
// already included above
// #include "message/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
// already included above
// #include "message/srv/detail/data_ba__struct.h"
// already included above
// #include "message/srv/detail/data_ba__functions.h"
// already included above
// #include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif


// forward declare type support functions


using _DataBA_Response__ros_msg_type = message__srv__DataBA_Response;

static bool _DataBA_Response__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _DataBA_Response__ros_msg_type * ros_message = static_cast<const _DataBA_Response__ros_msg_type *>(untyped_ros_message);
  // Field name: response_data_ba
  {
    cdr << ros_message->response_data_ba;
  }

  return true;
}

static bool _DataBA_Response__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _DataBA_Response__ros_msg_type * ros_message = static_cast<_DataBA_Response__ros_msg_type *>(untyped_ros_message);
  // Field name: response_data_ba
  {
    cdr >> ros_message->response_data_ba;
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_message
size_t get_serialized_size_message__srv__DataBA_Response(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _DataBA_Response__ros_msg_type * ros_message = static_cast<const _DataBA_Response__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name response_data_ba
  {
    size_t item_size = sizeof(ros_message->response_data_ba);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

static uint32_t _DataBA_Response__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_message__srv__DataBA_Response(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_message
size_t max_serialized_size_message__srv__DataBA_Response(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: response_data_ba
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = message__srv__DataBA_Response;
    is_plain =
      (
      offsetof(DataType, response_data_ba) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _DataBA_Response__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_message__srv__DataBA_Response(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_DataBA_Response = {
  "message::srv",
  "DataBA_Response",
  _DataBA_Response__cdr_serialize,
  _DataBA_Response__cdr_deserialize,
  _DataBA_Response__get_serialized_size,
  _DataBA_Response__max_serialized_size
};

static rosidl_message_type_support_t _DataBA_Response__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_DataBA_Response,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataBA_Response)() {
  return &_DataBA_Response__type_support;
}

#if defined(__cplusplus)
}
#endif

#include "rosidl_typesupport_fastrtps_cpp/service_type_support.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "rosidl_typesupport_fastrtps_c/identifier.h"
// already included above
// #include "message/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "message/srv/data_ba.h"

#if defined(__cplusplus)
extern "C"
{
#endif

static service_type_support_callbacks_t DataBA__callbacks = {
  "message::srv",
  "DataBA",
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataBA_Request)(),
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataBA_Response)(),
};

static rosidl_service_type_support_t DataBA__handle = {
  rosidl_typesupport_fastrtps_c__identifier,
  &DataBA__callbacks,
  get_service_typesupport_handle_function,
};

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataBA)() {
  return &DataBA__handle;
}

#if defined(__cplusplus)
}
#endif
