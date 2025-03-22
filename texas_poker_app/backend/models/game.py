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
        self.total_bet = 0
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
        self.total_bet += amount

    def fold(self):
        self.folded = True
        self.hand = []  # Очищаем руку при сбросе карт

    def reset_bet(self):
        self.current_bet = 0


    # Добавляем новые методы из player.py
    def receive_cards(self, cards: list):
        """Получение списка карт в руку."""
        self.hand.extend(cards)

    def reset(self):
        """Сброс состояния игрока для новой раздачи."""
        self.hand = []
        self.folded = False
        self.current_bet = 0
        self.total_bet = 0

    def __str__(self):
        """Строковое представление игрока."""
        hand_str = ", ".join(str(card) for card in self.hand) if self.hand else "No cards"
        return f"{self.name}: {hand_str}, Balance: {self.balance}"

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
        self.dealer_index = -1  # Добавляем позицию дилера
        self.small_blind = 10   # Размер Small Blind
        self.big_blind = 20     # Размер Big Blind

    def start_new_round(self):
        self.deck = Deck()
        self.deck.shuffle()
        self.community_cards = [self.deck.deal_card() for _ in range(5)]
        self.revealed_cards = []
        self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]
        self.current_bet = 0
        self.stage = "preflop"
        self.actions_taken = 0

        # Сбрасываем ставки игроков
        for player in self.players:
            player.reset()  # Используем новый метод reset вместо отдельных сбросов

        # Перемещаем дилера
        self.dealer_index = (self.dealer_index + 1) % len(self.players) if self.dealer_index >= 0 else 0
        print(f"New dealer_index: {self.dealer_index}")  # Лог для отладки

        # Определяем Small Blind и Big Blind на основе dealer_index
        small_blind_index = (self.dealer_index + 1) % len(self.players)
        big_blind_index = (self.dealer_index + 2) % len(self.players)

        # Снимаем Small Blind
        if len(self.players) > 1:
            try:
                small_blind_amount = min(self.small_blind, self.players[small_blind_index].balance)
                if small_blind_amount == 0:
                    raise ValueError(
                        f"Player {self.players[small_blind_index].name} (Small Blind) has no chips, cannot start round")
                self.players[small_blind_index].place_bet(small_blind_amount)
                self.update_pots(small_blind_amount, self.players[small_blind_index])
            except ValueError as e:
                print(f"Error placing Small Blind: {e}")
                raise

        # Снимаем Big Blind
        if len(self.players) > 1:
            try:
                big_blind_amount = min(self.big_blind, self.players[big_blind_index].balance)
                if big_blind_amount == 0:
                    raise ValueError(
                        f"Player {self.players[big_blind_index].name} (Big Blind) has no chips, cannot start round")
                self.players[big_blind_index].place_bet(big_blind_amount)
                self.update_pots(big_blind_amount, self.players[big_blind_index])
            except ValueError as e:
                print(f"Error placing Big Blind: {e}")
                raise

        # Устанавливаем текущего игрока (после Big Blind)
        self.current_player_index = (big_blind_index + 1) % len(self.players) if len(self.players) > 1 else 0

        # Раздаём карты
        for player in self.players:
            cards = [self.deck.deal_card(), self.deck.deal_card()]
            player.receive_cards(cards)  # Используем receive_cards вместо отдельных receive_card

        # Обновляем self.current_bet после блайндов
        self.current_bet = max(p.current_bet for p in self.players)

    def can_advance_stage(self, balances=None):
        if self.stage == "showdown":
            return False

        # Игроки, которые могут действовать
        active_players = [p for p in self.players if not p.folded and (balances[p.name] if balances else p.balance) > 0]
        # Игроки, которые всё ещё в игре (включая all-in)
        players_in_game = [p for p in self.players if not p.folded]

        if len(players_in_game) <= 1:
            return True

        # Находим максимальную ставку среди игроков в игре
        max_bet = max(p.current_bet for p in players_in_game)
        # Проверяем, уравняли ли все игроки максимальную ставку (или у них all-in)
        bets_settled = all(
            p.current_bet == max_bet or (balances[p.name] if balances else p.balance) == 0 for p in players_in_game
        )
        # Проверяем, сделал ли каждый активный игрок хотя бы одно действие
        round_completed = self.actions_taken >= len(active_players) if active_players else True

        print(f"can_advance_stage: bets_settled={bets_settled}, round_completed={round_completed}, max_bet={max_bet}")
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

            if self.stage in ["flop", "turn", "river"]:
                small_blind_index = (self.dealer_index + 1) % len(self.players)
                self.current_player_index = self.next_active_player(small_blind_index - 1)
                if self.current_player_index == -1:
                    self.stage = self.STAGES[current_index + 2] if current_index + 2 < len(self.STAGES) else "showdown"
                    if self.stage == "turn":
                        self.revealed_cards = self.community_cards[:4]
                    elif self.stage == "river" or self.stage == "showdown":
                        self.revealed_cards = self.community_cards[:5]
                self.actions_taken = 0
                self.current_bet = 0
                for player in self.players:
                    if not player.folded:
                        player.current_bet = 0
                return True
        elif self.stage == "showdown":
            return True  # Возвращаем True, чтобы routes/game.py вызвал broadcast_winner
        return False

    def next_active_player(self, start_index):
        active_players = [i for i, p in enumerate(self.players) if not p.folded and p.balance > 0]
        if not active_players:
            return -1  # Возвращаем -1, если нет активных игроков
        next_index = (start_index + 1) % len(self.players)
        # Проверяем, чтобы не зациклиться
        start = next_index
        while next_index not in active_players:
            next_index = (next_index + 1) % len(self.players)
            if next_index == start:  # Если сделали полный круг
                return -1
        return next_index

    def place_bet(self, player_index, amount):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index:
            return False
        # Корректируем amount до проверки total_bet
        if amount > player.balance:
            amount = player.balance
        total_bet = player.current_bet + amount
        if total_bet < self.current_bet:
            raise ValueError("Bet must match or exceed current bet")
        player.place_bet(amount)
        self.update_pots(amount, player)
        self.current_bet = max(p.current_bet for p in self.players)  # Обновляем текущую ставку
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def call_or_pass(self, player_index):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index:
            return False
        max_bet = max(p.current_bet for p in self.players)
        difference = max_bet - player.current_bet
        if difference > 0:
            if difference > player.balance:
                amount = player.balance
                player.place_bet(amount)
                self.update_pots(amount, player)
            else:
                player.place_bet(difference)
                self.update_pots(difference, player)
        self.current_bet = max(p.current_bet for p in self.players)
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def fold(self, player_index):
        if self.players[player_index].folded:
            return False
        self.players[player_index].folded = True
        self.actions_taken += 1

        # Обновляем eligible_players в банках
        for pot in self.pots:
            if self.players[player_index].name in pot["eligible_players"]:
                pot["eligible_players"].remove(self.players[player_index].name)

        self.current_player_index = self.next_active_player(self.current_player_index)
        return True

    def all_in(self, player_index):
        player = self.players[player_index]
        if player.folded or player_index != self.current_player_index or player.balance == 0:
            return False
        amount = player.balance
        player.place_bet(amount)
        self.update_pots(amount, player)
        self.current_bet = max(p.current_bet for p in self.players)
        self.current_player_index = self.next_active_player(player_index)
        self.actions_taken += 1
        return True

    def update_pots(self, amount, player):
        print(f"Updating pots for player {player.name}, amount={amount}, current_bet={player.current_bet}")
        # Убедимся, что у нас есть хотя бы один банк
        if not self.pots:
            print("No pots exist, creating a new one")
            self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players if not p.folded]}]

        # Вычисляем, сколько нужно добавить
        additional_bet = amount
        if additional_bet <= 0:
            print(f"No additional bet needed for player {player.name}, returning")
            return

        # Удаляем проверку баланса, так как она уже выполнена в place_bet
        # print(f"Checking balance: additional_bet={additional_bet}, player.balance={player.balance}")
        # if additional_bet > player.balance:
        #     print(f"Player {player.name} does not have enough chips to bet {additional_bet}")
        #     raise ValueError(f"Player {player.name} does not have enough chips to bet {additional_bet}")

        # Находим текущую максимальную ставку среди всех игроков (для отладки)
        print("Calculating max_bet")
        try:
            max_bet = max(p.current_bet for p in self.players)
        except Exception as e:
            print(f"Error calculating max_bet: {e}")
            raise
        print(f"max_bet={max_bet}")

        # Определяем активных игроков
        print("Determining active players")
        active_players = [p.name for p in self.players if not p.folded]
        print(f"Active players: {active_players}")
        if not active_players:
            print("No active players, returning")
            return

        # Проверяем, нужно ли создавать новый банк
        print(f"Current pots: {self.pots}")
        if not self.pots or self.pots[-1]["eligible_players"] != active_players:
            print("Creating new pot due to change in eligible players")
            self.pots.append({"amount": 0, "eligible_players": active_players})

        # Добавляем ставку к последнему банку
        print(f"Adding {additional_bet} to last pot")
        self.pots[-1]["amount"] += additional_bet

        print(f"Updated pots: {self.pots}, max_bet: {max_bet}, current_bet: {self.current_bet}")

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
                {
                    "name": p.name,
                    "folded": p.folded,
                    "bet": p.current_bet,
                    "total_bet": p.total_bet,  # Убедимся, что это поле есть
                    "is_dealer": i == self.dealer_index,
                    "is_small_blind": i == (self.dealer_index + 1) % len(self.players),
                    "is_big_blind": i == (self.dealer_index + 2) % len(self.players),
                }
                for i, p in enumerate(self.players)
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
        print(f"Calling get_winner for stage: {self.stage}")
        players_in_game = [p for p in self.players if not p.folded]
        if len(players_in_game) == 1:
            winner = players_in_game[0]
            total_winnings = sum(pot["amount"] for pot in self.pots if winner.name in pot["eligible_players"])
            self.players[self.players.index(winner)].balance += total_winnings
            print(
                f"Winner (single player): {winner.name}, winnings: {total_winnings}, new balance: {self.players[self.players.index(winner)].balance}")
            result = {"player": winner.name, "hand": [str(card) for card in winner.hand], "winnings": total_winnings}
            # Сбрасываем банк
            self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]
            return result

        best_hand = None
        winners = []
        for player in players_in_game:
            hand = player.hand + self.revealed_cards
            hand_value = self.get_best_hand(player)
            print(f"Evaluating hand for {player.name}: {hand}, hand_value: {hand_value}")
            if best_hand is None or hand_value > best_hand:
                best_hand = hand_value
                winners = [player]
            elif hand_value == best_hand:
                winners.append(player)

        total_winnings = sum(
            pot["amount"] for pot in self.pots if any(w.name in pot["eligible_players"] for w in winners))
        winnings_per_player = total_winnings // len(winners)
        for winner in winners:
            self.players[self.players.index(winner)].balance += winnings_per_player
            print(
                f"Adding winnings to {winner.name}: {winnings_per_player}, new balance: {self.players[self.players.index(winner)].balance}")
        print(
            f"Winners: {[w.name for w in winners]}, total winnings: {total_winnings}, per player: {winnings_per_player}")
        result = {
            "player": [w.name for w in winners],
            "hand": [str(card) for card in winners[0].hand] if winners else [],
            "winnings": total_winnings
        }
        # Сбрасываем банк
        self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]
        return result