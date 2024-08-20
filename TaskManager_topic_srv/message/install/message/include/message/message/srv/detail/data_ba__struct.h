// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from message:srv/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__SRV__DETAIL__DATA_BA__STRUCT_H_
#define MESSAGE__SRV__DETAIL__DATA_BA__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/DataBA in the package message.
typedef struct message__srv__DataBA_Request
{
  int32_t data_ba;
} message__srv__DataBA_Request;

// Struct for a sequence of message__srv__DataBA_Request.
typedef struct message__srv__DataBA_Request__Sequence
{
  message__srv__DataBA_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} message__srv__DataBA_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/DataBA in the package message.
typedef struct message__srv__DataBA_Response
{
  int32_t response_data_ba;
} message__srv__DataBA_Response;

// Struct for a sequence of message__srv__DataBA_Response.
typedef struct message__srv__DataBA_Response__Sequence
{
  message__srv__DataBA_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} message__srv__DataBA_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MESSAGE__SRV__DETAIL__DATA_BA__STRUCT_H_
