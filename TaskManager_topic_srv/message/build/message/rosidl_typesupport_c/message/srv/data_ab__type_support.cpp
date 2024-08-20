// generated from rosidl_typesupport_c/resource/idl__type_support.cpp.em
// with input from message:srv/DataAB.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "message/srv/detail/data_ab__struct.h"
#include "message/srv/detail/data_ab__type_support.h"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/message_type_support_dispatch.h"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace message
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _DataAB_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _DataAB_Request_type_support_ids_t;

static const _DataAB_Request_type_support_ids_t _DataAB_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _DataAB_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _DataAB_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _DataAB_Request_type_support_symbol_names_t _DataAB_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataAB_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, message, srv, DataAB_Request)),
  }
};

typedef struct _DataAB_Request_type_support_data_t
{
  void * data[2];
} _DataAB_Request_type_support_data_t;

static _DataAB_Request_type_support_data_t _DataAB_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _DataAB_Request_message_typesupport_map = {
  2,
  "message",
  &_DataAB_Request_message_typesupport_ids.typesupport_identifier[0],
  &_DataAB_Request_message_typesupport_symbol_names.symbol_name[0],
  &_DataAB_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t DataAB_Request_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_DataAB_Request_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace message

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, message, srv, DataAB_Request)() {
  return &::message::srv::rosidl_typesupport_c::DataAB_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "message/srv/detail/data_ab__struct.h"
// already included above
// #include "message/srv/detail/data_ab__type_support.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace message
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _DataAB_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _DataAB_Response_type_support_ids_t;

static const _DataAB_Response_type_support_ids_t _DataAB_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _DataAB_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _DataAB_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _DataAB_Response_type_support_symbol_names_t _DataAB_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataAB_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, message, srv, DataAB_Response)),
  }
};

typedef struct _DataAB_Response_type_support_data_t
{
  void * data[2];
} _DataAB_Response_type_support_data_t;

static _DataAB_Response_type_support_data_t _DataAB_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _DataAB_Response_message_typesupport_map = {
  2,
  "message",
  &_DataAB_Response_message_typesupport_ids.typesupport_identifier[0],
  &_DataAB_Response_message_typesupport_symbol_names.symbol_name[0],
  &_DataAB_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t DataAB_Response_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_DataAB_Response_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace message

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, message, srv, DataAB_Response)() {
  return &::message::srv::rosidl_typesupport_c::DataAB_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "message/srv/detail/data_ab__type_support.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/service_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace message
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _DataAB_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _DataAB_type_support_ids_t;

static const _DataAB_type_support_ids_t _DataAB_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _DataAB_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _DataAB_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _DataAB_type_support_symbol_names_t _DataAB_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, message, srv, DataAB)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, message, srv, DataAB)),
  }
};

typedef struct _DataAB_type_support_data_t
{
  void * data[2];
} _DataAB_type_support_data_t;

static _DataAB_type_support_data_t _DataAB_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _DataAB_service_typesupport_map = {
  2,
  "message",
  &_DataAB_service_typesupport_ids.typesupport_identifier[0],
  &_DataAB_service_typesupport_symbol_names.symbol_name[0],
  &_DataAB_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t DataAB_service_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_DataAB_service_typesupport_map),
  rosidl_typesupport_c__get_service_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace message

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_c, message, srv, DataAB)() {
  return &::message::srv::rosidl_typesupport_c::DataAB_service_type_support_handle;
}

#ifdef __cplusplus
}
#endif
