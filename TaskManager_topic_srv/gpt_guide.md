ROS2에서 서비스와 토픽을 사용하여 A와 B 노드를 통신하게 만드는 예제를 작성해보겠습니다. 우선, 필요한 메시지와 서비스 파일을 정의하고, 그런 다음 A 노드와 B 노드의 Python 코드를 작성하겠습니다.

### 1. 메시지 파일 정의

먼저, 두 개의 메시지 타입을 정의합니다.

**`DataAB.msg`**
```plaintext
int32 data
```

**`DataBA.msg`**
```plaintext
int32 data
```

### 2. 서비스 파일 정의

다음으로, 서비스 파일을 정의합니다.

**`SrvAB.srv`**
```plaintext
---  # request와 response를 구분하기 위한 구분선
int32 result
```

### 3. 노드 코드 작성

다음은 A와 B 노드의 코드를 작성합니다.

**A 노드 (`node_a.py`)**

```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
from your_package_name.msg import DataAB, DataBA
from your_package_name.srv import SrvAB

class NodeA(Node):
    def __init__(self):
        super().__init__('node_a')
        self.data_ab_pub = self.create_publisher(DataAB, 'data_ab', 10)
        self.data_ab_counter = 0
        self.timer = self.create_timer(10.0, self.publish_data_ab)
        self.srv_client = self.create_client(SrvAB, 'srv_ab')
        
        self.data_ba_sub = self.create_subscription(DataBA, 'data_ba', self.data_ba_callback, 10)
        self.received_data_ba = 0

    def publish_data_ab(self):
        msg = DataAB()
        msg.data = 1
        self.data_ab_pub.publish(msg)
        self.data_ab_counter += 1
        self.get_logger().info(f'Published data_ab: {msg.data} (count: {self.data_ab_counter})')

        if self.data_ab_counter == 10:
            self.call_srv_ab()

    def call_srv_ab(self):
        req = SrvAB.Request()
        self.get_logger().info(f'Calling service SrvAB...')
        while not self.srv_client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting again...')
        future = self.srv_client.call_async(req)
        future.add_done_callback(self.handle_service_response)

    def handle_service_response(self, future):
        try:
            response = future.result()
            self.get_logger().info(f'Service response: {response.result}')
        except Exception as e:
            self.get_logger().error(f'Service call failed {str(e)}')

    def data_ba_callback(self, msg):
        self.received_data_ba = msg.data
        self.get_logger().info(f'Received data_ba: {self.received_data_ba}')

def main(args=None):
    rclpy.init(args=args)
    node_a = NodeA()
    rclpy.spin(node_a)
    node_a.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

**B 노드 (`node_b.py`)**

```python
import rclpy
from rclpy.node import Node
from your_package_name.msg import DataAB, DataBA
from your_package_name.srv import SrvAB

class NodeB(Node):
    def __init__(self):
        super().__init__('node_b')
        self.data_ba_pub = self.create_publisher(DataBA, 'data_ba', 10)
        self.data_ab_sub = self.create_subscription(DataAB, 'data_ab', self.data_ab_callback, 10)
        self.srv_ab = self.create_service(SrvAB, 'srv_ab', self.srv_ab_callback)

        self.data_ab_count = 0
        self.data_ba_total = 0

    def data_ab_callback(self, msg):
        self.data_ab_count += 1
        self.data_ba_total += msg.data

        data_ba_msg = DataBA()
        data_ba_msg.data = self.data_ba_total
        self.data_ba_pub.publish(data_ba_msg)
        self.get_logger().info(f'Received data_ab: {msg.data}, Published data_ba: {data_ba_msg.data}')

    def srv_ab_callback(self, request, response):
        self.get_logger().info(f'Service SrvAB called, comparing counts...')
        if self.data_ab_count * 1 == self.data_ba_total:
            response.result = 1
        else:
            response.result = 0
        return response

def main(args=None):
    rclpy.init(args=args)
    node_b = NodeB()
    rclpy.spin(node_b)
    node_b.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

### 4. CMakeLists.txt 및 package.xml 설정

패키지에 필요한 메시지와 서비스를 빌드하도록 CMakeLists.txt와 package.xml에 설정을 추가해야 합니다.

**CMakeLists.txt**
```cmake
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/DataAB.msg"
  "msg/DataBA.msg"
  "srv/SrvAB.srv"
)

ament_export_dependencies(rosidl_default_runtime)
```

**package.xml**
```xml
<depend>rclpy</depend>
<depend>std_msgs</depend>
<depend>rosidl_default_runtime</depend>
```

### 5. 빌드 및 실행

위의 모든 설정을 완료했다면 패키지를 빌드하고 노드를 실행할 수 있습니다.

```bash
colcon build --packages-select your_package_name
source install/setup.bash
ros2 run your_package_name node_a
ros2 run your_package_name node_b
```

이제 A 노드와 B 노드가 정상적으로 통신하면서 요구된 동작을 수행할 것입니다. A 노드는 `DataAB` 토픽을 주기적으로 퍼블리시하고, B 노드는 이를 받아 `DataBA` 토픽으로 응답합니다. 10번의 `DataAB` 메시지가 보내지면, A 노드는 B 노드에 서비스 요청을 보내고, B 노드는 현재까지 받은 데이터를 비교하여 응답합니다.
