from process_a.process_layer_a import ProcessLayerA

class IntermediateLayerA:
    def __init__(self):
        self.process_layer = ProcessLayerA()

    def handle_ros_data(self, data):
        return self.process_layer.process_data(data)