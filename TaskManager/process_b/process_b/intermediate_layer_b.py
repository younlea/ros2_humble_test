from process_b.process_layer_b import ProcessLayerB

class IntermediateLayerB:
    def __init__(self):
        self.process_layer = ProcessLayerB()

    def handle_ros_data(self, data):
        return self.process_layer.process_data(data)