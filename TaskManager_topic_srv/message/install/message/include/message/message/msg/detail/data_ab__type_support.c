// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from message:msg/DataAB.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "message/msg/detail/data_ab__rosidl_typesupport_introspection_c.h"
#include "message/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "message/msg/detail/data_ab__functions.h"
#include "message/msg/detail/data_ab__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  message__msg__DataAB__init(message_memory);
}

void message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_fini_function(void * message_memory)
{
  message__msg__DataAB__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_member_array[1] = {
  {
    "data_ab",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(message__msg__DataAB, data_ab),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_members = {
  "message__msg",  // message namespace
  "DataAB",  // message name
  1,  // number of fields
  sizeof(message__msg__DataAB),
  message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_member_array,  // message members
  message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_init_function,  // function to initialize message memory (memory has to be allocated)
  message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_type_support_handle = {
  0,
  &message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_message
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, message, msg, DataAB)() {
  if (!message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_type_support_handle.typesupport_identifier) {
    message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &message__msg__DataAB__rosidl_typesupport_introspection_c__DataAB_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
