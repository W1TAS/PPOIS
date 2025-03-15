class Card:
    def __init__(self, suit: str, rank: str):
        self.suit = suit  # Масть (hearts, diamonds, clubs, spades)
        self.rank = rank  # Значение (2, 3, ..., 10, J, Q, K, A)

    def __str__(self):
        return f"{self.rank} of {self.suit}"