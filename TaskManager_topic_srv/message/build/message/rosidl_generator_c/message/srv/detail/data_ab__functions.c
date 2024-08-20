// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from message:srv/DataAB.idl
// generated code does not contain a copyright notice
#include "message/srv/detail/data_ab__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
message__srv__DataAB_Request__init(message__srv__DataAB_Request * msg)
{
  if (!msg) {
    return false;
  }
  // data_ab
  return true;
}

void
message__srv__DataAB_Request__fini(message__srv__DataAB_Request * msg)
{
  if (!msg) {
    return;
  }
  // data_ab
}

bool
message__srv__DataAB_Request__are_equal(const message__srv__DataAB_Request * lhs, const message__srv__DataAB_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // data_ab
  if (lhs->data_ab != rhs->data_ab) {
    return false;
  }
  return true;
}

bool
message__srv__DataAB_Request__copy(
  const message__srv__DataAB_Request * input,
  message__srv__DataAB_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // data_ab
  output->data_ab = input->data_ab;
  return true;
}

message__srv__DataAB_Request *
message__srv__DataAB_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  message__srv__DataAB_Request * msg = (message__srv__DataAB_Request *)allocator.allocate(sizeof(message__srv__DataAB_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(message__srv__DataAB_Request));
  bool success = message__srv__DataAB_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
message__srv__DataAB_Request__destroy(message__srv__DataAB_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    message__srv__DataAB_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
message__srv__DataAB_Request__Sequence__init(message__srv__DataAB_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  message__srv__DataAB_Request * data = NULL;

  if (size) {
    data = (message__srv__DataAB_Request *)allocator.zero_allocate(size, sizeof(message__srv__DataAB_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = message__srv__DataAB_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        message__srv__DataAB_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
message__srv__DataAB_Request__Sequence__fini(message__srv__DataAB_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      message__srv__DataAB_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

message__srv__DataAB_Request__Sequence *
message__srv__DataAB_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  message__srv__DataAB_Request__Sequence * array = (message__srv__DataAB_Request__Sequence *)allocator.allocate(sizeof(message__srv__DataAB_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = message__srv__DataAB_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
message__srv__DataAB_Request__Sequence__destroy(message__srv__DataAB_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    message__srv__DataAB_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
message__srv__DataAB_Request__Sequence__are_equal(const message__srv__DataAB_Request__Sequence * lhs, const message__srv__DataAB_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!message__srv__DataAB_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
message__srv__DataAB_Request__Sequence__copy(
  const message__srv__DataAB_Request__Sequence * input,
  message__srv__DataAB_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(message__srv__DataAB_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    message__srv__DataAB_Request * data =
      (message__srv__DataAB_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!message__srv__DataAB_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          message__srv__DataAB_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!message__srv__DataAB_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
message__srv__DataAB_Response__init(message__srv__DataAB_Response * msg)
{
  if (!msg) {
    return false;
  }
  // response_data_ab
  return true;
}

void
message__srv__DataAB_Response__fini(message__srv__DataAB_Response * msg)
{
  if (!msg) {
    return;
  }
  // response_data_ab
}

bool
message__srv__DataAB_Response__are_equal(const message__srv__DataAB_Response * lhs, const message__srv__DataAB_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // response_data_ab
  if (lhs->response_data_ab != rhs->response_data_ab) {
    return false;
  }
  return true;
}

bool
message__srv__DataAB_Response__copy(
  const message__srv__DataAB_Response * input,
  message__srv__DataAB_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // response_data_ab
  output->response_data_ab = input->response_data_ab;
  return true;
}

message__srv__DataAB_Response *
message__srv__DataAB_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  message__srv__DataAB_Response * msg = (message__srv__DataAB_Response *)allocator.allocate(sizeof(message__srv__DataAB_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(message__srv__DataAB_Response));
  bool success = message__srv__DataAB_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
message__srv__DataAB_Response__destroy(message__srv__DataAB_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    message__srv__DataAB_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
message__srv__DataAB_Response__Sequence__init(message__srv__DataAB_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  message__srv__DataAB_Response * data = NULL;

  if (size) {
    data = (message__srv__DataAB_Response *)allocator.zero_allocate(size, sizeof(message__srv__DataAB_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = message__srv__DataAB_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        message__srv__DataAB_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
message__srv__DataAB_Response__Sequence__fini(message__srv__DataAB_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      message__srv__DataAB_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

message__srv__DataAB_Response__Sequence *
message__srv__DataAB_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  message__srv__DataAB_Response__Sequence * array = (message__srv__DataAB_Response__Sequence *)allocator.allocate(sizeof(message__srv__DataAB_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = message__srv__DataAB_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
message__srv__DataAB_Response__Sequence__destroy(message__srv__DataAB_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    message__srv__DataAB_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
message__srv__DataAB_Response__Sequence__are_equal(const message__srv__DataAB_Response__Sequence * lhs, const message__srv__DataAB_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!message__srv__DataAB_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
message__srv__DataAB_Response__Sequence__copy(
  const message__srv__DataAB_Response__Sequence * input,
  message__srv__DataAB_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(message__srv__DataAB_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    message__srv__DataAB_Response * data =
      (message__srv__DataAB_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!message__srv__DataAB_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          message__srv__DataAB_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!message__srv__DataAB_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
