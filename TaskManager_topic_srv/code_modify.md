`nodeA`에서 토픽을 퍼블리시할 때, 메시지 타입을 `message.msg.DataAB`로 사용하도록 코드를 수정하겠습니다. 이는 `std_msgs.msg.Int32` 대신 `message.msg.DataAB` 메시지 타입을 이용하여 데이터를 퍼블리시하는 것을 의미합니다.

### 1. `nodeA` 코드 수정

아래는 `nodeA`의 `ros2_layer.py` 파일을 `message.msg.DataAB` 메시지를 사용하도록 수정한 코드입니다.

#### **nodeA/src/nodeA/ros2_layer.py**
```python
import rclpy
from rclpy.node import Node
from message.msg import DataAB  # Import the DataAB message type
from message.srv import SrvBA

# Import the necessary classes
from nodeA.intermediate_layer import NodeAIntermediate
from nodeA.process_layer import NodeAProcess

class NodeARos2(Node):
    def __init__(self):
        super().__init__('node_a')
        self.publisher_ = self.create_publisher(DataAB, 'data_ab', 10)  # Use DataAB message type
        self.timer = self.create_timer(1.0, self.publish_data_ab)
        self.intermediate_layer = NodeAIntermediate(NodeAProcess())
        self.srv_server = self.create_service(SrvBA, 'srv_ba', self.handle_srv_ba_request)

    def publish_data_ab(self):
        if self.intermediate_layer.process_layer.publish_enabled:
            msg = DataAB()
            msg.data_ab = self.intermediate_layer.generate_random_number()
            self.publisher_.publish(msg)
            print(f"Publishing DataAB: {msg.data_ab}")
        else:
            print("Publishing paused.")
            self.create_timer(10.0, self.resume_publishing, one_shot=True)

    def resume_publishing(self):
        self.intermediate_layer.process_layer.publish_enabled = True
        print("Resumed publishing.")

    def handle_srv_ba_request(self, request, response):
        print(f"Received service request: {request.data_ba}")
        if self.intermediate_layer.handle_service_response(request.data_ba):
            response.response_data_ba = 10
            print("Pausing publishing for 10 seconds.")
        else:
            response.response_data_ba = -1
        return response

def main(args=None):
    rclpy.init(args=args)
    node = NodeARos2()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

### 2. `nodeB` 코드 검토 (메시지 타입 확인)

`nodeB`에서 `DataAB` 메시지를 구독할 때도 `message.msg.DataAB` 타입을 사용해야 합니다. 이 부분은 이미 설정되어 있다면 수정할 필요가 없지만, 확인해보겠습니다.

#### **nodeB/src/nodeB/ros2_layer.py**
```python
import rclpy
from rclpy.node import Node
from message.msg import DataAB  # Use DataAB message type
from message.srv import SrvBA

# Import the necessary classes
from nodeB.intermediate_layer import NodeBIntermediate
from nodeB.process_layer import NodeBProcess

class NodeBRos2(Node):
    def __init__(self):
        super().__init__('node_b')
        self.subscription = self.create_subscription(DataAB, 'data_ab', self.listener_callback, 10)  # Use DataAB message type
        self.srv_client = self.create_client(SrvBA, 'srv_ba')
        self.intermediate_layer = NodeBIntermediate(NodeBProcess())

    def listener_callback(self, msg):
        print(f"Received DataAB: {msg.data_ab}")
        sum_ab = self.intermediate_layer.handle_data_ab(msg.data_ab)
        if sum_ab is not None:
            self.call_srv_ba(sum_ab)

    def call_srv_ba(self, data):
        request = SrvBA.Request()
        request.data_ba = data
        future = self.srv_client.call_async(request)
        future.add_done_callback(self.handle_srv_ba_response)

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

### 3. 패키지 재빌드

수정된 사항을 반영한 후 패키지를 다시 빌드합니다.

```bash
colcon build --packages-select nodeA nodeB
```

### 4. 노드 실행

패키지를 빌드한 후 `nodeA`와 `nodeB`를 각각 실행하여, 수정된 메시지 타입을 사용하는 동작을 확인합니다.

### 다음 작업
**a.** `nodeA`와 `nodeB`의 메시지 타입 변경 후 동작 확인  
**b.** 메시지 및 서비스의 데이터 흐름을 디버깅 로그로 확인
