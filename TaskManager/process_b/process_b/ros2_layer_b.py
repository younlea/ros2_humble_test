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