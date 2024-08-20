class NodeBProcess:
    def __init__(self):
        self.sum_ab = 0

    def process_data_ab(self, value):
        if value % 2 == 0:
            self.sum_ab += value
            print(f"Current sum: {self.sum_ab}")
        return self.sum_ab

    def check_multiple_of_ten(self):
        if self.sum_ab > 0 and self.sum_ab % 10 == 0:
            print("Sum is a multiple of 10. Notifying NodeA.")
            return True
        return False

    def reset_sum(self):
        self.sum_ab = 0