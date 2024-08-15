from process_c.process_layer_c import ProcessLayerC

class IntermediateLayerC:
    def __init__(self):
        self.process_layer = ProcessLayerC()

    def handle_ros_data(self, data):
        return self.process_layer.process_data(data)