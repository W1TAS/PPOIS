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
        self.players = [Player(name) for name in player_names] if player_names else []
        self.deck = Deck()
        self.community_cards = []
        self.revealed_cards = []
        self.pots = []  # Список банков: [{"amount": int, "eligible_players": [str]}]
        self.current_bet = 0
        self.stage = "preflop"
        self.current_player_index = 0
        self.actions_taken = 0

    def start_new_round(self):
        self.deck = Deck()
        self.deck.shuffle()
        self.community_cards = [self.deck.deal_card() for _ in range(5)]
        self.revealed_cards = []
        self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]
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
        bets_settled = all(p.current_bet == self.current_bet or p.balance == 0 or p.folded for p in self.players)
        round_completed = self.actions_taken >= len(active_players)
        return bets_settled and round_completed

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
            elif self.stage == "river" or self.stage == "showdown":
                self.revealed_cards = self.community_cards[:5]
            self.current_player_index = self.next_active_player(-1)
            self.actions_taken = 0
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
        if total_bet < self.current_bet:
            raise ValueError("Bet must match or exceed current bet")
        if amount > player.balance:
            amount = player.balance
            total_bet = player.current_bet + amount
        player.place_bet(amount)
        self.update_pots(amount, player)
        self.current_bet = max(self.current_bet, total_bet)
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def call_or_pass(self, player_index):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index:
            return False
        difference = self.current_bet - player.current_bet
        if difference > 0:
            if difference > player.balance:
                amount = player.balance
                player.place_bet(amount)
                self.update_pots(amount, player)
            else:
                player.place_bet(difference)
                self.update_pots(difference, player)
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def fold(self, player_index):
        if player_index != self.current_player_index:
            return False
        self.players[player_index].fold()
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def all_in(self, player_index):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index or player.balance == 0:
            return False
        amount = player.balance
        player.place_bet(amount)
        self.update_pots(amount, player)
        self.current_bet = max(self.current_bet, player.current_bet)
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def update_pots(self, amount, player):
        bets = sorted(set(p.current_bet for p in self.players if p.current_bet > 0))
        new_pots = []
        previous_cap = 0

        for bet in bets:
            pot_amount = 0
            eligible_players = []
            for p in self.players:
                if p.current_bet >= bet:
                    contribution = min(bet, p.current_bet) - previous_cap
                    pot_amount += contribution
                    if not p.folded:
                        eligible_players.append(p.name)
            if pot_amount > 0 and eligible_players:
                new_pots.append({"amount": pot_amount, "eligible_players": eligible_players})
            previous_cap = bet

        if not new_pots:
            new_pots = [{"amount": 0, "eligible_players": [p.name for p in self.players if not p.folded]}]
        self.pots = new_pots

    def get_game_state(self, player_index):
        player = self.players[player_index]
        return {
            "stage": self.stage,
            "hand": [str(card) for card in player.hand],
            "community_cards": [str(card) for card in self.revealed_cards],
            "balance": player.balance,
            "current_bet": player.current_bet,
            "pots": [{"amount": pot["amount"], "eligible_players": pot["eligible_players"]} for pot in self.pots],
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
        if not active_players:
            return None
        # Определяем победителя по лучшей комбинации
        winner = max(active_players, key=lambda p: self.get_best_hand(p)) if len(active_players) > 1 else \
        active_players[0]
        # Распределяем банки и возвращаем лишние фишки
        total_won = 0
        for pot in reversed(self.pots):  # Начинаем с последнего банка
            if winner.name in pot["eligible_players"]:
                winner.balance += pot["amount"]
                total_won += pot["amount"]
            else:
                # Если победитель не участвует в этом банке, возвращаем фишки остальным игрокам
                eligible_players = [p for p in self.players if p.name in pot["eligible_players"]]
                if eligible_players:
                    refund_per_player = pot["amount"] // len(eligible_players)
                    for p in eligible_players:
                        if p != winner:  # Не возвращаем победителю
                            p.balance += refund_per_player
                            print(f"Refunded {refund_per_player} to {p.name} from pot {pot['amount']}")
        print(f"Winner {winner.name} won {total_won} from pots: {self.pots}")
        # Возвращаем объект с именем и рукой
        return {
            "name": winner.name,
            "hand": [str(card) for card in winner.hand]  # Преобразуем карты в строки
        }