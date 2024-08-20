# Question 
nodeA는 1초 주기로 nodeB로 1의 값을 보냄
nodeB는 nodeB에서 보내는 데이터를 받아서 더하고 10의 배수가 될때 마다 nodeA로 service call로 더해진 값을 보낸다. 

# Answer 

이제 `nodeA`와 `nodeB`를 구현하여 위에서 설정한 ROS2 메시지와 서비스에 따라 동작하게 해보겠습니다. `nodeA`는 데이터를 퍼블리시하고, `nodeB`는 데이터를 받아서 처리하며, 특정 조건에서 `DataBA` 서비스를 통해 `nodeA`로 요청을 보냅니다. 각 노드의 구현을 아래에 설명하겠습니다.

### 1. NodeA

NodeA는 1초마다 `DataAB` 메시지를 퍼블리시하고, `DataBA` 서비스 요청을 수신할 수 있습니다.

**nodeA/nodeA/nodeA.py**:
```python
import rclpy
from rclpy.node import Node
from message.msg import DataAB
from message.srv import DataBA
from std_msgs.msg import String

class NodeA(Node):
    def __init__(self):
        super().__init__('node_a')
        
        # Publisher for DataAB
        self.publisher_ = self.create_publisher(DataAB, 'data_ab_topic', 10)
        
        # Create a timer to publish DataAB message every second
        self.timer = self.create_timer(1.0, self.publish_data_ab)
        self.data_ab_value = 1

        # Create a service to handle DataBA requests
        self.srv = self.create_service(DataBA, 'data_ba_service', self.handle_data_ba_request)

    def publish_data_ab(self):
        msg = DataAB()
        msg.data_ab = self.data_ab_value
        self.publisher_.publish(msg)
        self.get_logger().info(f'Publishing DataAB: {msg.data_ab}')
        self.data_ab_value += 1  # Increment data_ab for next message

    def handle_data_ba_request(self, request, response):
        response.response_data_ba = request.data_ba
        self.get_logger().info(f'Service request: {request.data_ba}, sending response: {response.response_data_ba}')
        return response

def main(args=None):
    rclpy.init(args=args)
    node = NodeA()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

**nodeA/setup.py**:
```python
from setuptools import setup

package_name = 'nodeA'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='your_name',
    maintainer_email='your_email@example.com',
    description='NodeA that publishes DataAB and provides DataBA service',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'node_a = nodeA.nodeA:main',
        ],
    },
)
```

### 2. NodeB

NodeB는 `DataAB` 메시지를 구독하고, 받은 값을 더하여 10의 배수가 될 때마다 `DataBA` 서비스를 호출하여 `nodeA`에 요청을 보냅니다.

**nodeB/nodeB/nodeB.py**:
```python
import rclpy
from rclpy.node import Node
from message.msg import DataAB
from message.srv import DataBA

class NodeB(Node):
    def __init__(self):
        super().__init__('node_b')
        
        # Subscriber for DataAB
        self.subscription = self.create_subscription(
            DataAB,
            'data_ab_topic',
            self.listener_callback,
            10)
        self.total_sum = 0

        # Create a client to call DataBA service
        self.client = self.create_client(DataBA, 'data_ba_service')
        self.timer = self.create_timer(1.0, self.check_and_send_request)  # Check every second

    def listener_callback(self, msg):
        self.total_sum += msg.data_ab
        self.get_logger().info(f'Received DataAB: {msg.data_ab}, Total Sum: {self.total_sum}')

    def check_and_send_request(self):
        if self.total_sum >= 10 and self.total_sum % 10 == 0:
            if not self.client.service_is_ready():
                self.get_logger().info('Service not available, waiting again...')
                return
            
            req = DataBA.Request()
            req.data_ba = self.total_sum
            future = self.client.call_async(req)
            rclpy.spin_until_future_complete(self, future)
            if future.result() is not None:
                response = future.result()
                self.get_logger().info(f'Received response from service: {response.response_data_ba}')
                self.total_sum = 0  # Reset sum after service call
            else:
                self.get_logger().error('Service call failed')

def main(args=None):
    rclpy.init(args=args)
    node = NodeB()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

**nodeB/setup.py**:
```python
from setuptools import setup

package_name = 'nodeB'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='your_name',
    maintainer_email='your_email@example.com',
    description='NodeB that subscribes to DataAB and sends DataBA service requests',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'node_b = nodeB.nodeB:main',
        ],
    },
)
```

### 3. Build and Run

1. **Build**:
    ```bash
    cd <workspace_root>
    colcon build
    ```

2. **Source the setup files**:
    ```bash
    source install/setup.bash
    ```

3. **Run NodeA**:
    ```bash
    ros2 run nodeA node_a
    ```

4. **Run NodeB**:
    ```bash
    ros2 run nodeB node_b
    ```

이제 `nodeA`는 1초마다 `DataAB` 메시지를 퍼블리시하고, `nodeB`는 이 메시지를 구독하여 총합을 계산하고 10의 배수일 때마다 `DataBA` 서비스를 호출하여 `nodeA`에 요청을 보냅니다.
