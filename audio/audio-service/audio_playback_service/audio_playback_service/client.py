import rclpy
from rclpy.node import Node
from audio_msgs.srv import AudioControl

class AudioClient(Node):
    def __init__(self):
        super().__init__('audio_client')
        self.client = self.create_client(AudioControl, 'audio_control')
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Waiting for service...')

    def send_request(self, command, track=0, volume=0.0):
        request = AudioControl.Request()
        request.command = command
        request.track = track
        request.volume = volume

        future = self.client.call_async(request)
        rclpy.spin_until_future_complete(self, future)

        if future.result() is not None:
            self.get_logger().info(f"Response: {future.result().message}")
        else:
            self.get_logger().error("Service call failed.")

def main(args=None):
    rclpy.init(args=args)
    client = AudioClient()

    # Example: Play track 1.
    client.send_request("play", track=3)

    # Example: Set volume to 50%.
    client.send_request("set_volume", volume=0.5)

    client.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
