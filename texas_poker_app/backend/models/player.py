class Player:
    def __init__(self, name: str, chips: int = 1000):
        """Инициализация игрока.

        Args:
            name (str): Имя игрока.
            chips (int): Начальное количество фишек (по умолчанию 1000).
        """
        self.name = name
        self.hand = []  # Карты в руке игрока (изначально пустая)
        self.chips = chips  # Количество фишек
        self.is_active = True  # Статус активности (например, не сбросил ли карты)

    def receive_cards(self, cards: list):
        """Получение карт в руку."""
        self.hand.extend(cards)

    def place_bet(self, amount: int) -> bool:
        """Ставка фишек.

        Args:
            amount (int): Количество фишек для ставки.

        Returns:
            bool: Успешность ставки (True, если хватает фишек).
        """
        if amount <= self.chips:
            self.chips -= amount
            return True
        return False

    def fold(self):
        """Сброс карт (игрок выходит из текущего раунда)."""
        self.hand = []
        self.is_active = False

    def reset(self):
        """Сброс состояния игрока для новой раздачи."""
        self.hand = []
        self.is_active = True

    def __str__(self):
        """Строковое представление игрока."""
        hand_str = ", ".join(str(card) for card in self.hand) if self.hand else "No cards"
        return f"{self.name}: {hand_str}, Chips: {self.chips}"