import random

class NodeAIntermediate:
    def __init__(self, process_layer):
        self.process_layer = process_layer

    def generate_random_number(self):
        return random.randint(1, 10)

    def handle_service_response(self, response_data_ba):
        return self.process_layer.handle_srv_ba(response_data_ba)