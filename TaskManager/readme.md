# ros2_humble_test
아래 같은 구조를 만들고 싶어서 만들어 봄. (topic 만을 이용해서 통신 샘플을 만듬)  

<img width="691" alt="image" src="https://github.com/user-attachments/assets/c68e5a14-6bed-4d4d-a629-5b8b1d23d599">

[메세지 참고](https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Custom-ROS2-Interfaces.html) 

message를 따로 만들고 메세지를 쓸 package들을 만들어서 써야함.
한번에 넣으니까 안된다 .ㅡ.ㅡ;
```
1. message관련 package 만든다.
   ros2 pkg create message_test
   rm -r include src
   mkdir msg
   touch msg/DataAB.msg

  CMakeList.txt 아래 내용 추가
  find_package(rosidl_default_generators REQUIRED)

  rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/DataAB.msg"
  "msg/DataBA.msg"
  "msg/DataBC.msg"
  "msg/DataCB.msg"
  )

  package.xml 아래 내용 추가
   
  <buildtool_depend>rosidl_default_generators</buildtool_depend>
  <exec_depend>rosidl_default_runtime</exec_depend>
  <member_of_group>rosidl_interface_packages</member_of_group>

빌드 된거 확인
   ros2 interface show interface_msg/msg/DataAB 로 확인가능. 
   
2. 각 process에서 mseeage 사용
    ros2 create pkg amend_python process_xx    
   
    package.xml 에 아래 내용(message관련 dependent추가)    
     <depend>message_test</depend>   

   python code에 아래 내용 추가(interface_msg/msg/DataBA 파일이 있다.)    
   from message_test.msg import DataBA, DataBC    
     
    setup.py에 아래 내용 추가.    
    'console_scripts': [    
         'ros2_layer_b = process_b.ros2_layer_b:main',   
    ],   
```

Python으로 작성된 ROS2 Humble에서 A, B, C 프로세스 간의 통신을 구현한 샘플 코드를 단계별로 설명드리겠습니다. 각 프로세스는 `process_layer`, `intermediate_layer`, `ros2_layer`로 구성되며, 메시지 송수신을 통해 통신합니다.

### 1. 프로젝트 구조
먼저 각 프로세스에 대한 ROS2 패키지를 생성합니다:

```bash
ros2 pkg create message_test
ros2 pkg create --build-type ament_python process_a
ros2 pkg create --build-type ament_python process_b
ros2 pkg create --build-type ament_python process_c
```

message_test 디렉토리에 `msg` 디렉토리를 생성하고, 통신할 메시지 타입을 정의합니다.

### 2. 메시지 정의

`message_test/msg/DataAB.msg`:
```plaintext
float32 value1
int32 value2
```

`message_test/msg/DataBA.msg`:
```plaintext
string text
bool flag
```

`message_test/msg/DataBC.msg`:
```plaintext
float32 value3
```

`message_test/msg/DataCB.msg`:
```plaintext
int64 value4
```

### 3. CMakeLists.txt 및 package.xml 설정
패키지의 `CMakeLists.txt`와 `package.xml` 파일을 업데이트하여 메시지를 빌드하도록 설정합니다.

**message_test/CMakeLists.txt**:
```cmake
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/DataAB.msg"
  "msg/DataBA.msg"
  "msg/DataBC.msg"
  "msg/DataCB.msg"
)
```

**message_test/package.xml**:
```xml
  <buildtool_depend>rosidl_default_generators</buildtool_depend>
  <exec_depend>rosidl_default_runtime</exec_depend>
  <member_of_group>rosidl_interface_packages</member_of_group>
```

각 패키지마다 이와 유사하게 설정합니다.

### 4. 각 프로세스 구현

#### A 프로세스

**process_a/process_a/process_layer_a.py**:
```python
class ProcessLayerA:
    def process_data(self, data):
        # 예제: 데이터 처리 로직
        processed_data = data.value1 * 2
        return processed_data
```

**process_a/process_a/intermediate_layer_a.py**:
```python
from process_a.process_layer_a import ProcessLayerA

class IntermediateLayerA:
    def __init__(self):
        self.process_layer = ProcessLayerA()

    def handle_ros_data(self, data):
        return self.process_layer.process_data(data)
```

**process_a/process_a/ros2_layer_a.py**:
```python
import rclpy
from rclpy.node import Node
from message_test.msg import DataAB
from process_a.intermediate_layer_a import IntermediateLayerA

class ROS2LayerA(Node):
    def __init__(self):
        super().__init__('ros2_layer_a')
        self.publisher_ = self.create_publisher(DataAB, 'topic_ab', 10)
        self.subscription = self.create_subscription(DataAB, 'topic_ba', self.listener_callback, 10)
        self.intermediate_layer = IntermediateLayerA()

    def listener_callback(self, msg):
        processed_data = self.intermediate_layer.handle_ros_data(msg)
        self.publish_data(processed_data)

    def publish_data(self, data):
        msg = DataAB()
        msg.value1 = data
        msg.value2 = 42
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = ROS2LayerA()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```
#### 추가 수정
```
    package.xml 에 아래 내용(message관련 dependent추가)    
     <depend>message_test</depend>   

   python code에 아래 내용 추가(interface_msg/msg/DataBA 파일이 있다.)    
   from message_test.msg import DataBA, DataBC    
     
    setup.py에 아래 내용 추가.    
    'console_scripts': [    
         'ros2_layer_a = process_a.ros2_layer_a:main',   
    ],
```
 
#### B 프로세스

**process_b/process_b/process_layer_b.py**:
```python
class ProcessLayerB:
    def process_data(self, data):
        # 예제: 데이터 처리 로직
        return f"Received: {data.value1}"
```

**process_b/process_b/intermediate_layer_b.py**:
```python
from process_b.process_layer_b import ProcessLayerB

class IntermediateLayerB:
    def __init__(self):
        self.process_layer = ProcessLayerB()

    def handle_ros_data(self, data):
        return self.process_layer.process_data(data)
```

**process_b/process_b/ros2_layer_b.py**:
```python
import rclpy
from rclpy.node import Node
from message_test.msg import DataBA, DataBC
from process_b.intermediate_layer_b import IntermediateLayerB

class ROS2LayerB(Node):
    def __init__(self):
        super().__init__('ros2_layer_b')
        self.publisher_ab = self.create_publisher(DataBA, 'topic_ba', 10)
        self.subscription_ab = self.create_subscription(DataBA, 'topic_ab', self.listener_callback_ab, 10)
        
        self.publisher_bc = self.create_publisher(DataBC, 'topic_bc', 10)
        self.subscription_bc = self.create_subscription(DataBC, 'topic_cb', self.listener_callback_bc, 10)
        
        self.intermediate_layer = IntermediateLayerB()

    def listener_callback_ab(self, msg):
        processed_data = self.intermediate_layer.handle_ros_data(msg)
        self.publish_data_ab(processed_data)

    def listener_callback_bc(self, msg):
        processed_data = self.intermediate_layer.handle_ros_data(msg)
        self.publish_data_bc(processed_data)

    def publish_data_ab(self, data):
        msg = DataBA()
        msg.text = data
        msg.flag = True
        self.publisher_ab.publish(msg)

    def publish_data_bc(self, data):
        msg = DataBC()
        msg.value3 = 1.23
        self.publisher_bc.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = ROS2LayerB()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

#### C 프로세스

**process_c/process_c/process_layer_c.py**:
```python
class ProcessLayerC:
    def process_data(self, data):
        # 예제: 데이터 처리 로직
        return data.value3 + 10
```

**process_c/process_c/intermediate_layer_c.py**:
```python
from process_c.process_layer_c import ProcessLayerC

class IntermediateLayerC:
    def __init__(self):
        self.process_layer = ProcessLayerC()

    def handle_ros_data(self, data):
        return self.process_layer.process_data(data)
```

**process_c/process_c/ros2_layer_c.py**:
```python
import rclpy
from rclpy.node import Node
from message_test.msg import DataBC, DataCB
from process_c.intermediate_layer_c import IntermediateLayerC

class ROS2LayerC(Node):
    def __init__(self):
        super().__init__('ros2_layer_c')
        self.publisher_ = self.create_publisher(DataCB, 'topic_cb', 10)
        self.subscription = self.create_subscription(DataBC, 'topic_bc', self.listener_callback, 10)
        self.intermediate_layer = IntermediateLayerC()

    def listener_callback(self, msg):
        processed_data = self.intermediate_layer.handle_ros_data(msg)
        self.publish_data(processed_data)

    def publish_data(self, data):
        msg = DataCB()
        msg.value4 = data
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = ROS2LayerC()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

### 5. 빌드 및 실행

각 패키지 디렉토리에서 다음 명령어를 실행하여 빌드를 수행합니다:

```bash
colcon build --packages-select process_a process_b process_c
```

빌드가 완료된 후, 각 프로세스를 실행합니다:

```bash
# 터미널 1: A 프로세스 실행
ros2 run process_a ros2_layer_a

# 터미널 2: B 프로세스 실행
ros2 run process_b ros2_layer_b

# 터미널 3: C 프로세스 실행
ros2 run process_c ros2_layer_c
```

이제 A, B, C 프로세스가 ROS2를 통해 통신하며 데이터를 주고받습니다.

### 추가 제안
**a.** 각 프로세스에서 발생할 수 있는 오류를 처리하기 위해 예외 처리를 추가해보세요.

**b.** 각 메시지 전송과 수신에 대한 로그를 추가하여 디버깅과 모니터링을 용이하게 해보세요.
