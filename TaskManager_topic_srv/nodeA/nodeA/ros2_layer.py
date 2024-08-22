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
