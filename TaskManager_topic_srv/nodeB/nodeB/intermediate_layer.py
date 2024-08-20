class NodeBIntermediate:
    def __init__(self, process_layer):
        self.process_layer = process_layer

    def handle_data_ab(self, value):
        sum_ab = self.process_layer.process_data_ab(value)
        if self.process_layer.check_multiple_of_ten():
            self.process_layer.reset_sum()
            return sum_ab
        return None