class Garden:
    def __init__(self):
        self.plants = {}

    def plant(self, name: str):
        self.plants[name] = {"watered": False, "fertilized": False}

    def water(self, name: str):
        if name in self.plants:
            self.plants[name]["watered"] = True

    def fertilize(self, name: str):
        if name in self.plants:
            self.plants[name]["fertilized"] = True

    def __str__(self):
        return json.dumps(self.plants, indent=4)