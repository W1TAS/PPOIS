import random
from .card import Card

class Deck:
    def __init__(self):
        """Инициализация стандартной колоды из 52 карт."""
        suits = ["hearts", "diamonds", "clubs", "spades"]  # Масти
        ranks = ["2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"]  # Значения
        self.cards = [Card(suit, rank) for suit in suits for rank in ranks]  # Создаем все комбинации
        self.shuffle()  # Перемешиваем колоду при создании

    def shuffle(self):
        """Перемешивание колоды."""
        random.shuffle(self.cards)

    def draw(self):
        """Извлечение одной карты из колоды."""
        if len(self.cards) > 0:
            return self.cards.pop()  # Удаляем и возвращаем верхнюю карту
        else:
            raise ValueError("Колода пуста, нельзя взять карту")

    def remaining_cards(self):
        """Возвращает количество оставшихся карт в колоде."""
        return len(self.cards)

    def __str__(self):
        """Строковое представление колоды."""
        return f"Deck with {self.remaining_cards()} cards remaining"