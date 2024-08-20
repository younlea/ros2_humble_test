topic 과 service를 같이 사용하는 예제

```
# 1. create message package, node A, node B
ros2 pkg create message
ros2 pkg create --build-type ament_python nodeA
ros2 pkg create --build-type ament_python nodeB

# 2. add msg and srv
in message 
.
├── CMakeLists.txt
├── msg
│   ├── DataAB.msg
│   └── DataBA.msg
├── package.xml
└── srv
    ├── srvDataAB.srv
    └── srvDataBA.srv
```
message 
`message/msg/DataAB.msg`:
```plaintext
int32 data_ab
```

`message/msg/DataBA.msg`:
```plaintext
int32 data_ba
```

`message/srv/SrvAB.msg`:
```plaintext
int32 data_ab
---
int32 response_data_ab
```

`message/srv/SrvBA.msg`:
```plaintext
int32 data_ba
---
int32 response_data_ba
```

패키지의 `CMakeLists.txt`와 `package.xml` 파일을 업데이트하여 메시지를 빌드하도록 설정합니다.

**message/CMakeLists.txt**:
```cmake
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/DataAB.msg"
  "msg/DataBA.msg"
  "srv/SrvAB.srv"
  "srv/SrvBA.srv"
)
```

**message_test/package.xml**:
```xml
  <buildtool_depend>rosidl_default_generators</buildtool_depend>
  <exec_depend>rosidl_default_runtime</exec_depend>
  <member_of_group>rosidl_interface_packages</member_of_group>
```
    

# 3. Add A, B source code
```
```

# 4. build & run
```
colcon build
source install/setup.bash
ros2 run nodeA node_a
ros2 run nodeb node_b
```