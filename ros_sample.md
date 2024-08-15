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
)
```

**message_test/package.xml**:
```xml
<build_depend>rosidl_default_generators</build_depend>
<exec_depend>rosidl_default_runtime</exec_depend>
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
from process_a.msg import DataAB
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
from process_b.msg import DataBA, DataBC
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
from process_c.msg import DataBC, DataCB
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
