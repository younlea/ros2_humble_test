// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from message:msg/DataBA.idl
// generated code does not contain a copyright notice

#ifndef MESSAGE__MSG__DETAIL__DATA_BA__STRUCT_H_
#define MESSAGE__MSG__DETAIL__DATA_BA__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/DataBA in the package message.
typedef struct message__msg__DataBA
{
  int32_t data_ba;
} message__msg__DataBA;

// Struct for a sequence of message__msg__DataBA.
typedef struct message__msg__DataBA__Sequence
{
  message__msg__DataBA * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} message__msg__DataBA__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MESSAGE__MSG__DETAIL__DATA_BA__STRUCT_H_
