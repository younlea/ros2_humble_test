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