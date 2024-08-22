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
