# Question 

```
ros2 humble을 이용해서 topic 과 service에 대한 샘플 코드를 만들고 있어. 
message, nodeA, nodeB를 아래와 같이 만들었고 
# 1. create message package, node A, node B
ros2 pkg create message
ros2 pkg create --build-type ament_python nodeA
ros2 pkg create --build-type ament_python nodeB

# 2. add msg and srv
in message 
.
ââ€â€ CMakeLists.txt
ââ€â€ msg
â   ââ€â€ DataAB.msg
â   ââ€â€ DataBA.msg
ââ€â€ package.xml
ââ€â€ srv
    ââ€â€ srvDataAB.srv
    ââ€â€ srvDataBA.srv
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

í¨í¤ì§€ì `CMakeLists.txt`ì€ `package.xml` íì¼ì ìë°ì´í¸íì¬ ë©ìì§€ë¥¼ ë¹ëíëë¡ ì¤ì í©ëë¤.

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
```
이제 nodeA와 nodeB쪽 코드를 작성해야 하는데. 관련해서 아래와 같은 기능을 하는 sample code 및 패키지에 수정해야 하는것들을 알려줘. 

nodeA는 1초 주기로 DataAB에 1~10까지의 랜덤한 수를 퍼블리쉬 하고 nodeB는 dataAB를 서브스크라이브해서 읽어오는 데이터중 2의 배수만 계속 더하고 10의 배수가 될 때 마다 nodeA로 serBA를 사용해서 10의 배수가 된걸 nodeA에 알려주면  nodeA는 10초간 topic 퍼블리쉬를 멈췄다가 다시 보내는 코드를 파이썬으로 작성해줘. 

각 퍼블리쉬와 서브스크라이브, 서비스 콜을 할때마다 확인할수 있게 프린트로 알려줘. 

마지막으로 nodeA, nodeB 의 소스코드 구조를 아래처럼 3개 레이어로 나눠서 작성해줘. 
process_layer.py
intermediate_layer.py
ros2_layer.py
```


# Answer
이제 ROS 2 Humble을 이용한 `nodeA`와 `nodeB`의 파이썬 샘플 코드를 작성하겠습니다. 요구 사항에 따라 코드를 3개의 레이어로 나누어 구성할 것입니다. 각 노드는 메시지와 서비스를 주고받으며 동작하게 됩니다.

### 1. 패키지 및 메시지/서비스 설정
패키지 `message`는 이미 설정이 되어있습니다. `nodeA`와 `nodeB` 패키지에서는 이 메시지와 서비스를 이용하게 됩니다.

### 2. `nodeA` 코드
`nodeA`는 1초 주기로 `DataAB` 메시지를 퍼블리시하고, `nodeB`로부터 `SrvBA` 서비스를 호출받으면 10초 동안 퍼블리시를 멈췄다가 다시 시작합니다.

#### **nodeA/src/nodeA/process_layer.py**
```python
class NodeAProcess:
    def __init__(self):
        self.publish_enabled = True

    def process_data_ab(self, value):
        # 1~10 사이의 랜덤 수를 반환
        return value

    def handle_srv_ba(self, response_data_ba):
        if response_data_ba == 10:
            self.publish_enabled = False
            print("Received notification from NodeB: 10's multiple found. Stopping publishing for 10 seconds.")
            return True
        return False
```

#### **nodeA/src/nodeA/intermediate_layer.py**
```python
import random

class NodeAIntermediate:
    def __init__(self, process_layer):
        self.process_layer = process_layer

    def generate_random_number(self):
        return random.randint(1, 10)

    def handle_service_response(self, response_data_ba):
        return self.process_layer.handle_srv_ba(response_data_ba)
```

#### **nodeA/src/nodeA/ros2_layer.py**
```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
from message.srv import SrvBA

from nodeA.intermediate_layer import NodeAIntermediate
from nodeA.process_layer import NodeAProcess

class NodeARos2(Node):
    def __init__(self):
        super().__init__('node_a')
        self.publisher_ = self.create_publisher(Int32, 'data_ab', 10)
        self.srv_client = self.create_client(SrvBA, 'srv_ba')
        self.timer = self.create_timer(1.0, self.publish_data_ab)
        self.intermediate_layer = NodeAIntermediate(NodeAProcess())

    def publish_data_ab(self):
        if self.intermediate_layer.process_layer.publish_enabled:
            msg = Int32()
            msg.data = self.intermediate_layer.generate_random_number()
            self.publisher_.publish(msg)
            print(f"Publishing: {msg.data}")
        else:
            print("Publishing paused.")
            self.create_timer(10.0, self.resume_publishing, one_shot=True)

    def resume_publishing(self):
        self.intermediate_layer.process_layer.publish_enabled = True
        print("Resumed publishing.")

    def call_srv_ba(self, data):
        request = SrvBA.Request()
        request.data_ba = data
        self.srv_client.call_async(request).add_done_callback(self.handle_srv_ba_response)

    def handle_srv_ba_response(self, future):
        response = future.result()
        self.intermediate_layer.handle_service_response(response.response_data_ba)

def main(args=None):
    rclpy.init(args=args)
    node = NodeARos2()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

### 3. `nodeB` 코드
`nodeB`는 `DataAB` 메시지를 구독하고, 2의 배수를 누적하여 10의 배수일 때 `SrvBA` 서비스를 호출합니다.

#### **nodeB/src/nodeB/process_layer.py**
```python
class NodeBProcess:
    def __init__(self):
        self.sum_ab = 0

    def process_data_ab(self, value):
        if value % 2 == 0:
            self.sum_ab += value
            print(f"Current sum: {self.sum_ab}")
        return self.sum_ab

    def check_multiple_of_ten(self):
        if self.sum_ab > 0 and self.sum_ab % 10 == 0:
            print("Sum is a multiple of 10. Notifying NodeA.")
            return True
        return False

    def reset_sum(self):
        self.sum_ab = 0
```

#### **nodeB/src/nodeB/intermediate_layer.py**
```python
class NodeBIntermediate:
    def __init__(self, process_layer):
        self.process_layer = process_layer

    def handle_data_ab(self, value):
        sum_ab = self.process_layer.process_data_ab(value)
        if self.process_layer.check_multiple_of_ten():
            self.process_layer.reset_sum()
            return sum_ab
        return None
```

#### **nodeB/src/nodeB/ros2_layer.py**
```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
from message.srv import SrvBA

from nodeB.intermediate_layer import NodeBIntermediate
from nodeB.process_layer import NodeBProcess

class NodeBRos2(Node):
    def __init__(self):
        super().__init__('node_b')
        self.subscription = self.create_subscription(Int32, 'data_ab', self.listener_callback, 10)
        self.srv_client = self.create_client(SrvBA, 'srv_ba')
        self.intermediate_layer = NodeBIntermediate(NodeBProcess())

    def listener_callback(self, msg):
        print(f"Received: {msg.data}")
        sum_ab = self.intermediate_layer.handle_data_ab(msg.data)
        if sum_ab is not None:
            self.call_srv_ba(sum_ab)

    def call_srv_ba(self, data):
        request = SrvBA.Request()
        request.data_ba = data
        self.srv_client.call_async(request).add_done_callback(self.handle_srv_ba_response)

    def handle_srv_ba_response(self, future):
        response = future.result()
        print(f"NodeA responded with: {response.response_data_ba}")

def main(args=None):
    rclpy.init(args=args)
    node = NodeBRos2()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

### 4. Node 패키지 구성
`nodeA`와 `nodeB` 패키지의 디렉토리 구조는 다음과 같습니다.

#### **nodeA 패키지 디렉토리**
```
nodeA/
├── src/
│   ├── nodeA/
│   │   ├── __init__.py
│   │   ├── process_layer.py
│   │   ├── intermediate_layer.py
│   │   └── ros2_layer.py
└── setup.py
```

#### **nodeB 패키지 디렉토리**
```
nodeB/
├── src/
│   ├── nodeB/
│   │   ├── __init__.py
│   │   ├── process_layer.py
│   │   ├── intermediate_layer.py
│   │   └── ros2_layer.py
└── setup.py
```

### 5. `setup.py` 파일 설정
각 패키지의 `setup.py` 파일을 수정하여 패키지 구성 요소를 정의합니다.

#### **nodeA/setup.py**
```python
from setuptools import setup

package_name = 'nodeA'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    py_modules=[],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='your_name',
    maintainer_email='your_email@example.com',
    description='nodeA package for ROS2',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'node_a = nodeA.ros2_layer:main',
        ],
    },
)
```

#### **nodeB/setup.py**
```python
from setuptools import setup

package_name = 'nodeB'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    py_modules=[],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='your_name',
    maintainer_email='your_email@example.com',
    description='nodeB package for ROS2',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'node_b = nodeB.ros2_layer:main',
        ],
    },
)
```

### 다음 작업
**a.** 노드 실행 후 동작 확인 (예: 터미널에서 `nodeA`와 `nodeB` 실행)  
**b.** 노드 테스트를 위한 pytest 기반의 단위 테스트 작성
