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