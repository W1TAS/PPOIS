import random
from itertools import combinations

class Card:
    RANKS = {'2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9,
             '10': 10, 'J': 11, 'Q': 12, 'K': 13, 'A': 14}

    def __init__(self, notation):
        self.rank = notation[:-1]
        self.suit = notation[-1]

    def get_value(self):
        return self.RANKS[self.rank]

    def __repr__(self):
        return f"{self.rank}{self.suit}"

    def __str__(self):
        return self.__repr__()

class Deck:
    def __init__(self):
        suits = ["♠", "♥", "♦", "♣"]
        ranks = ["2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"]
        self.cards = [Card(f"{rank}{suit}") for suit in suits for rank in ranks]

    def shuffle(self):
        random.shuffle(self.cards)

    def deal_card(self):
        return self.cards.pop() if self.cards else None

class Player:
    def __init__(self, name, balance=1000):
        self.name = name
        self.hand = []
        self.balance = balance
        self.current_bet = 0
        self.folded = False

    def receive_card(self, card):
        if card:
            self.hand.append(card)

    def place_bet(self, amount):
        if amount > self.balance:
            raise ValueError("Not enough balance")
        self.balance -= amount
        self.current_bet += amount

    def fold(self):
        self.folded = True

    def reset_bet(self):
        self.current_bet = 0

class PokerGame:
    STAGES = ["preflop", "flop", "turn", "river", "showdown"]

    def __init__(self, player_names):
        self.players = [Player(name) for name in player_names]
        self.deck = Deck()
        self.community_cards = []
        self.revealed_cards = []
        self.pot = 0
        self.current_bet = 0  # Максимальная ставка в текущем круге
        self.stage = "preflop"
        self.current_player_index = 0
        self.actions_taken = 0  # Счётчик действий на текущей стадии

    def start_new_round(self):
        self.deck = Deck()
        self.deck.shuffle()
        self.community_cards = [self.deck.deal_card() for _ in range(5)]
        self.revealed_cards = []
        self.pot = 0
        self.current_bet = 0
        self.stage = "preflop"
        self.current_player_index = 0
        self.actions_taken = 0
        for player in self.players:
            player.hand = [self.deck.deal_card(), self.deck.deal_card()]
            player.folded = False
            player.reset_bet()

    def can_advance_stage(self):
        active_players = [p for p in self.players if not p.folded]
        if len(active_players) <= 1:
            return True
        # Проверяем, уравнены ли ставки
        bets_equal = all(p.current_bet == self.current_bet for p in active_players)
        # Проверяем, сделал ли каждый активный игрок хотя бы одно действие
        round_completed = self.actions_taken >= len(active_players)
        return bets_equal and round_completed

    def advance_stage(self):
        if not self.can_advance_stage():
            return False
        current_index = self.STAGES.index(self.stage)
        if current_index < len(self.STAGES) - 1:
            self.stage = self.STAGES[current_index + 1]
            if self.stage == "flop":
                self.revealed_cards = self.community_cards[:3]
            elif self.stage == "turn":
                self.revealed_cards = self.community_cards[:4]
            elif self.stage == "river":
                self.revealed_cards = self.community_cards[:5]
            elif self.stage == "showdown":
                self.revealed_cards = self.community_cards[:5]
            self.current_bet = 0
            for player in self.players:
                player.reset_bet()
            self.current_player_index = self.next_active_player(-1)  # Начинаем с первого активного игрока
            self.actions_taken = 0  # Сбрасываем счётчик действий
            return True
        return False

    def next_active_player(self, start_index):
        active_players = [i for i, p in enumerate(self.players) if not p.folded]
        if not active_players:
            return 0
        next_index = (start_index + 1) % len(self.players)
        while next_index not in active_players:
            next_index = (next_index + 1) % len(self.players)
        return next_index

    def place_bet(self, player_index, amount):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index:
            return False
        total_bet = player.current_bet + amount
        if total_bet <= self.current_bet:
            raise ValueError("Bet must be higher than current bet")
        if amount > player.balance:
            raise ValueError("Bet exceeds balance")
        player.place_bet(amount)
        self.pot += amount
        self.current_bet = total_bet
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1  # Увеличиваем счётчик действий
        return True

    def call_or_pass(self, player_index):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index:
            return False
        difference = self.current_bet - player.current_bet
        if difference > 0:
            player.place_bet(difference)
            self.pot += difference
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1  # Увеличиваем счётчик действий
        return True

    def fold(self, player_index):
        if player_index != self.current_player_index:
            return False
        self.players[player_index].fold()
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1  # Увеличиваем счётчик действий
        return True

    def all_in(self, player_index):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index:
            return False
        amount = player.balance
        player.place_bet(amount)
        self.pot += amount
        self.current_bet = max(self.current_bet, player.current_bet)
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1  # Увеличиваем счётчик действий
        return True

    def get_game_state(self, player_index):
        player = self.players[player_index]
        return {
            "stage": self.stage,
            "hand": [str(card) for card in player.hand],
            "community_cards": [str(card) for card in self.revealed_cards],
            "balance": player.balance,
            "current_bet": player.current_bet,
            "pot": self.pot,
            "players": [
                {"name": p.name, "folded": p.folded, "bet": p.current_bet}
                for p in self.players
            ],
            "current_player": self.players[self.current_player_index].name,
            "can_advance": self.can_advance_stage()
        }

    def get_best_hand(self, player):
        all_cards = player.hand + self.revealed_cards
        best_hand = None
        for combo in combinations(all_cards, 5):
            hand_rank = self.evaluate_hand(combo)
            if not best_hand or hand_rank > best_hand:
                best_hand = hand_rank
        return best_hand

    def evaluate_hand(self, cards):
        values = sorted([card.get_value() for card in cards], reverse=True)
        suits = [card.suit for card in cards]
        is_flush = len(set(suits)) == 1
        is_straight = all(values[i] - 1 == values[i + 1] for i in range(len(values) - 1))

        if is_flush and is_straight and values[0] == 14:
            return (10, values)
        if is_flush and is_straight:
            return (9, values)
        if self.has_n_of_a_kind(values, 4):
            return (8, self.has_n_of_a_kind(values, 4))
        if self.has_full_house(values):
            return (7, self.has_full_house(values))
        if is_flush:
            return (6, values)
        if is_straight:
            return (5, values)
        if self.has_n_of_a_kind(values, 3):
            return (4, self.has_n_of_a_kind(values, 3))
        if self.has_two_pair(values):
            return (3, self.has_two_pair(values))
        if self.has_n_of_a_kind(values, 2):
            return (2, self.has_n_of_a_kind(values, 2))
        return (1, values)

    def has_n_of_a_kind(self, values, n):
        for value in set(values):
            if values.count(value) == n:
                return [value] + sorted([v for v in values if v != value], reverse=True)
        return None

    def has_full_house(self, values):
        three = self.has_n_of_a_kind(values, 3)
        pair = self.has_n_of_a_kind(values, 2)
        if three and pair:
            return three[:1] + pair[:1]
        return None

    def has_two_pair(self, values):
        pairs = [v for v in set(values) if values.count(v) == 2]
        if len(pairs) >= 2:
            pairs.sort(reverse=True)
            kicker = max(v for v in values if v not in pairs)
            return pairs[:2] + [kicker]
        return None

    def get_winner(self):
        active_players = [p for p in self.players if not p.folded]
        if len(active_players) == 1:
            winner = active_players[0]
        else:
            winner = max(active_players, key=lambda p: self.get_best_hand(p))
        winner.balance += self.pot
        return winner.name