class NodeAProcess:
    def __init__(self):
        self.publish_enabled = True

    def process_data_ab(self, value):
        # 1~10 ??? ?? ?? ??
        return value

    def handle_srv_ba(self, response_data_ba):
        if response_data_ba == 10:
            self.publish_enabled = False
            print("Received notification from NodeB: 10's multiple found. Stopping publishing for 10 seconds.")
            return True
        return False