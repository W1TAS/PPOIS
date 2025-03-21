import unittest
from backend.models.game import PokerGame, Player, Card
from itertools import combinations

class TestPokerGame(unittest.TestCase):
    def setUp(self):
        self.game = PokerGame([])  # Передаём пустой список
        self.player = Player("TestPlayer")

    def test_evaluate_hand(self):
        # Роял-флеш
        royal_flush = [
            Card("A♠"),
            Card("K♠"),
            Card("Q♠"),
            Card("J♠"),
            Card("10♠")
        ]
        self.assertEqual(self.game.evaluate_hand(royal_flush), (10, [14, 13, 12, 11, 10]))

        # Стрит-флеш
        straight_flush = [
            Card("9♥"),
            Card("8♥"),
            Card("7♥"),
            Card("6♥"),
            Card("5♥")
        ]
        self.assertEqual(self.game.evaluate_hand(straight_flush), (9, [9, 8, 7, 6, 5]))

        # Каре
        four_of_a_kind = [
            Card("A♠"),
            Card("A♥"),
            Card("A♦"),
            Card("A♣"),
            Card("K♠")
        ]
        self.assertEqual(self.game.evaluate_hand(four_of_a_kind), (8, [14, 13]))

        # Фулл-хаус
        full_house = [
            Card("K♠"),
            Card("K♥"),
            Card("K♦"),
            Card("Q♠"),
            Card("Q♥")
        ]
        self.assertEqual(self.game.evaluate_hand(full_house), (7, [13, 12]))

        # Флеш
        flush = [
            Card("K♠"),
            Card("J♠"),
            Card("7♠"),
            Card("6♠"),
            Card("5♠")
        ]
        self.assertEqual(self.game.evaluate_hand(flush), (6, [13, 11, 7, 6, 5]))

        # Стрит
        straight = [
            Card("9♠"),
            Card("8♥"),
            Card("7♦"),
            Card("6♣"),
            Card("5♠")
        ]
        self.assertEqual(self.game.evaluate_hand(straight), (5, [9, 8, 7, 6, 5]))

        # Сет
        three_of_a_kind = [
            Card("Q♠"),
            Card("Q♥"),
            Card("Q♦"),
            Card("K♠"),
            Card("J♥")
        ]
        self.assertEqual(self.game.evaluate_hand(three_of_a_kind), (4, [12, 13, 11]))

        # Две пары
        two_pairs = [
            Card("J♠"),
            Card("J♥"),
            Card("10♦"),
            Card("10♣"),
            Card("K♠")
        ]
        self.assertEqual(self.game.evaluate_hand(two_pairs), (3, [11, 10, 13]))

        # Пара
        one_pair = [
            Card("10♠"),
            Card("10♥"),
            Card("K♦"),
            Card("Q♣"),
            Card("J♠")
        ]
        self.assertEqual(self.game.evaluate_hand(one_pair), (2, [10, 13, 12, 11]))

        # Старшая карта
        high_card = [
            Card("A♠"),
            Card("K♥"),
            Card("Q♦"),
            Card("J♣"),
            Card("9♠")
        ]
        self.assertEqual(self.game.evaluate_hand(high_card), (1, [14, 13, 12, 11, 9]))

    def test_get_best_hand(self):
        # Флеш
        self.player.hand = [Card("K♠"), Card("10♥")]
        self.game.revealed_cards = [
            Card("5♠"),
            Card("2♣"),
            Card("7♠"),
            Card("J♠"),
            Card("6♠")
        ]
        best_hand = self.game.get_best_hand(self.player)
        self.assertEqual(best_hand, (6, [13, 11, 7, 6, 5]))

        # Две пары
        self.player.hand = [Card("2♥"), Card("J♥")]
        self.game.revealed_cards = [
            Card("5♠"),
            Card("2♣"),
            Card("7♠"),
            Card("J♠"),
            Card("6♠")
        ]
        best_hand = self.game.get_best_hand(self.player)
        self.assertEqual(best_hand, (3, [11, 2, 7]))

        # Стрит
        self.player.hand = [Card("8♠"), Card("9♥")]
        self.game.revealed_cards = [
            Card("5♠"),
            Card("6♣"),
            Card("7♦"),
            Card("J♠"),
            Card("Q♠")
        ]
        best_hand = self.game.get_best_hand(self.player)
        self.assertEqual(best_hand, (5, [9, 8, 7, 6, 5]))

    def test_get_winner(self):
        # Создаём игру с тремя игроками
        self.game = PokerGame(["1", "2", "3"])  # Переопределяем self.game с игроками
        self.game.players[0].hand = [Card("2♥"), Card("J♥")]
        self.game.players[1].hand = [Card("Q♥"), Card("8♦")]
        self.game.players[2].hand = [Card("K♠"), Card("10♥")]
        self.game.revealed_cards = [
            Card("5♠"),
            Card("2♣"),
            Card("7♠"),
            Card("J♠"),
            Card("6♠")
        ]
        self.game.pots = [{"amount": 2520, "eligible_players": ["1", "2", "3"]}]
        for player in self.game.players:
            player.folded = False

        winner_data = self.game.get_winner()
        self.assertEqual(winner_data["player"], ["3"])
        self.assertEqual(winner_data["winnings"], 2520)

if __name__ == "__main__":
    unittest.main()