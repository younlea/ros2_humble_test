# generated from rosidl_generator_py/resource/_idl.py.em
# with input from message:srv/DataBA.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_DataBA_Request(type):
    """Metaclass of message 'DataBA_Request'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('message')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'message.srv.DataBA_Request')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__srv__data_ba__request
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__srv__data_ba__request
            cls._CONVERT_TO_PY = module.convert_to_py_msg__srv__data_ba__request
            cls._TYPE_SUPPORT = module.type_support_msg__srv__data_ba__request
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__srv__data_ba__request

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class DataBA_Request(metaclass=Metaclass_DataBA_Request):
    """Message class 'DataBA_Request'."""

    __slots__ = [
        '_data_ba',
    ]

    _fields_and_field_types = {
        'data_ba': 'int32',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('int32'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.data_ba = kwargs.get('data_ba', int())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.data_ba != other.data_ba:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def data_ba(self):
        """Message field 'data_ba'."""
        return self._data_ba

    @data_ba.setter
    def data_ba(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'data_ba' field must be of type 'int'"
            assert value >= -2147483648 and value < 2147483648, \
                "The 'data_ba' field must be an integer in [-2147483648, 2147483647]"
        self._data_ba = value


# Import statements for member types

# already imported above
# import builtins

# already imported above
# import rosidl_parser.definition


class Metaclass_DataBA_Response(type):
    """Metaclass of message 'DataBA_Response'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('message')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'message.srv.DataBA_Response')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__srv__data_ba__response
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__srv__data_ba__response
            cls._CONVERT_TO_PY = module.convert_to_py_msg__srv__data_ba__response
            cls._TYPE_SUPPORT = module.type_support_msg__srv__data_ba__response
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__srv__data_ba__response

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class DataBA_Response(metaclass=Metaclass_DataBA_Response):
    """Message class 'DataBA_Response'."""

    __slots__ = [
        '_response_data_ba',
    ]

    _fields_and_field_types = {
        'response_data_ba': 'int32',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('int32'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.response_data_ba = kwargs.get('response_data_ba', int())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.response_data_ba != other.response_data_ba:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def response_data_ba(self):
        """Message field 'response_data_ba'."""
        return self._response_data_ba

    @response_data_ba.setter
    def response_data_ba(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'response_data_ba' field must be of type 'int'"
            assert value >= -2147483648 and value < 2147483648, \
                "The 'response_data_ba' field must be an integer in [-2147483648, 2147483647]"
        self._response_data_ba = value


class Metaclass_DataBA(type):
    """Metaclass of service 'DataBA'."""

    _TYPE_SUPPORT = None

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('message')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'message.srv.DataBA')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._TYPE_SUPPORT = module.type_support_srv__srv__data_ba

            from message.srv import _data_ba
            if _data_ba.Metaclass_DataBA_Request._TYPE_SUPPORT is None:
                _data_ba.Metaclass_DataBA_Request.__import_type_support__()
            if _data_ba.Metaclass_DataBA_Response._TYPE_SUPPORT is None:
                _data_ba.Metaclass_DataBA_Response.__import_type_support__()


class DataBA(metaclass=Metaclass_DataBA):
    from message.srv._data_ba import DataBA_Request as Request
    from message.srv._data_ba import DataBA_Response as Response

    def __init__(self):
        raise NotImplementedError('Service classes can not be instantiated')
