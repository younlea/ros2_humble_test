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