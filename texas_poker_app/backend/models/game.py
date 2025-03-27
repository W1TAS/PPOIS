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
        self.pots = []  # Инициализируем пустой список банков
        self.current_bet = 0
        self.stage = "preflop"
        self.actions_taken = 0

        # Сбрасываем ставки игроков
        for player in self.players:
            player.reset()

        # Перемещаем дилера
        self.dealer_index = (self.dealer_index + 1) % len(self.players) if self.dealer_index >= 0 else 0
        print(f"New dealer_index: {self.dealer_index}")

        # Определяем Small Blind и Big Blind
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
            player.receive_cards(cards)

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

        # Добавляем ставку игрока
        additional_bet = amount
        if additional_bet <= 0:
            print(f"No additional bet needed for player {player.name}, returning")
            return

        # Собираем всех игроков (включая тех, кто сбросил карты, так как их вклад уже в банке)
        all_players = [(p.name, p.total_bet, p.folded) for p in self.players]
        all_players.sort(key=lambda x: x[1])  # Сортируем по total_bet
        print(f"All players sorted by total_bet: {all_players}")

        # Создаём новые банки
        new_pots = []
        previous_bet = 0
        eligible = [p.name for p in self.players]  # Изначально все игроки

        for player_name, total_bet, folded in all_players:
            if total_bet <= previous_bet:
                continue

            # Обновляем eligible_players: исключаем игроков, чей total_bet меньше текущего уровня
            eligible = [p for p in eligible if
                        self.players[[p2.name for p2 in self.players].index(p)].total_bet >= total_bet]

            # Рассчитываем сумму для текущего банка на основе реальных ставок
            amount_for_pot = 0
            pot_eligible_players = []  # Игроки, которые реально внесли вклад в этот банк
            for p_name in eligible:
                player_obj = self.players[[p2.name for p2 in self.players].index(p_name)]
                # Учитываем только реальный вклад игрока на этом уровне
                contribution = min(total_bet, player_obj.total_bet) - min(previous_bet, player_obj.total_bet)
                if contribution > 0:
                    amount_for_pot += contribution
                    pot_eligible_players.append(p_name)

            if amount_for_pot > 0:
                new_pots.append({"amount": amount_for_pot, "eligible_players": pot_eligible_players})
                print(f"Created new pot: amount={amount_for_pot}, eligible_players={pot_eligible_players}")

            previous_bet = total_bet

        # Обновляем self.pots
        self.pots = new_pots

        # Удаляем банки с нулевой суммой
        self.pots = [pot for pot in self.pots if pot["amount"] > 0]
        if not self.pots:
            self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]

        # Обновляем eligible_players, исключая тех, кто сбросил карты
        for pot in self.pots:
            pot["eligible_players"] = [p for p in pot["eligible_players"] if
                                       not self.players[[p2.name for p2 in self.players].index(p)].folded]

        print(f"Updated pots: {self.pots}")

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
                    "total_bet": p.total_bet,
                    "balance": p.balance,
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

        # Если остался только один игрок (например, из-за фолда)
        if len(players_in_game) == 1:
            winner = players_in_game[0]
            total_net_winnings = 0
            for pot in self.pots:
                if pot["amount"] == 0:
                    continue
                # Игроки, которые имеют право на этот банк (до фолда)
                eligible_players = [p for p in self.players if p.name in pot["eligible_players"]]
                if not eligible_players:
                    continue

                # Рассчитываем вклад каждого игрока в этот банк
                contributions = {}
                previous_pots_amount = sum(prev_pot["amount"] for prev_pot in self.pots[:self.pots.index(pot)])
                for p in self.players:
                    contribution = min(pot["amount"], max(0, min(p.total_bet, previous_pots_amount + pot[
                        "amount"]) - previous_pots_amount))
                    contributions[p.name] = contribution

                # Если в банке остался только один участник, который не сбросил карты
                eligible_active_players = [p for p in eligible_players if not p.folded]
                if len(eligible_active_players) == 1 and eligible_active_players[0] == winner:
                    winner_contribution = contributions[winner.name]
                    net_winnings_for_pot = pot["amount"] - winner_contribution
                    total_net_winnings += net_winnings_for_pot
                    self.players[self.players.index(winner)].balance += pot["amount"]
                    print(
                        f"Winner {winner.name} receives from pot (single eligible player): pot amount={pot['amount']}, contribution={winner_contribution}, net winnings={net_winnings_for_pot}, total pot added={pot['amount']}, new balance: {self.players[self.players.index(winner)].balance}"
                    )
                else:
                    # Победитель получает весь банк
                    winner_contribution = contributions[winner.name]
                    net_winnings_for_pot = pot["amount"] - winner_contribution
                    total_net_winnings += net_winnings_for_pot
                    self.players[self.players.index(winner)].balance += pot["amount"]
                    print(
                        f"Winner {winner.name} receives from pot: pot amount={pot['amount']}, contribution={winner_contribution}, net winnings={net_winnings_for_pot}, total pot added={pot['amount']}, new balance: {self.players[self.players.index(winner)].balance}"
                    )

                # Возвращаем сайд-пот игрокам, которые не участвуют в раздаче
                for p in self.players:
                    if p.folded and p.name in pot["eligible_players"]:
                        player_contribution = contributions[p.name]
                        if player_contribution > 0:
                            self.players[self.players.index(p)].balance += player_contribution
                            print(
                                f"Returning side pot to {p.name}: amount={player_contribution}, new balance: {self.players[self.players.index(p)].balance}"
                            )

            print(
                f"Winner (single player): {winner.name}, total net winnings: {total_net_winnings}, final balance: {self.players[self.players.index(winner)].balance}"
            )
            result = {"player": winner.name, "hand": [str(card) for card in winner.hand],
                      "winnings": total_net_winnings}
            self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]
            return result

        # Шоудаун с несколькими игроками
        winners_by_pot = []
        total_net_winnings_by_player = {p.name: 0 for p in
                                        self.players}  # Отслеживаем чистый выигрыш для итогового результата

        for pot in self.pots:
            if pot["amount"] == 0:
                continue
            # Учитываем только игроков, которые не сбросили карты и участвуют в этом банке
            eligible_players = [p for p in self.players if p.name in pot["eligible_players"] and not p.folded]
            if not eligible_players:
                # Если нет игроков, которые не сбросили карты, возвращаем банк тем, кто внёс вклад
                contributions = {}
                previous_pots_amount = sum(prev_pot["amount"] for prev_pot in self.pots[:self.pots.index(pot)])
                for p in self.players:
                    contribution = min(pot["amount"], max(0, min(p.total_bet, previous_pots_amount + pot[
                        "amount"]) - previous_pots_amount))
                    contributions[p.name] = contribution
                    if contribution > 0 and p.name in pot["eligible_players"]:
                        self.players[self.players.index(p)].balance += contribution
                        print(
                            f"Returning pot to {p.name} (no eligible players): amount={contribution}, new balance: {self.players[self.players.index(p)].balance}"
                        )
                continue

            # Если в банке остался только один участник, который не сбросил карты
            if len(eligible_players) == 1:
                winner = eligible_players[0]
                contributions = {}
                previous_pots_amount = sum(prev_pot["amount"] for prev_pot in self.pots[:self.pots.index(pot)])
                for p in self.players:
                    contribution = min(pot["amount"], max(0, min(p.total_bet, previous_pots_amount + pot[
                        "amount"]) - previous_pots_amount))
                    contributions[p.name] = contribution

                winner_contribution = contributions[winner.name]
                net_winnings_for_pot = pot["amount"] - winner_contribution
                self.players[self.players.index(winner)].balance += pot["amount"]
                total_net_winnings_by_player[winner.name] += net_winnings_for_pot
                print(
                    f"Returning pot to {winner.name} (single eligible player): pot amount={pot['amount']}, contribution={winner_contribution}, net winnings={net_winnings_for_pot}, total pot added={pot['amount']}, new balance: {self.players[self.players.index(winner)].balance}"
                )
                winners_by_pot.append({
                    "pot_amount": pot["amount"],
                    "winners": [winner.name],
                    "winnings_per_player": pot["amount"]
                })
                continue

            # Находим победителей для этого банка
            best_hand = None
            pot_winners = []
            for player in eligible_players:
                hand = player.hand + self.revealed_cards
                hand_value = self.get_best_hand(player)
                print(f"Evaluating hand for {player.name}: {hand}, hand_value: {hand_value}")
                if best_hand is None or hand_value > best_hand:
                    best_hand = hand_value
                    pot_winners = [player]
                elif hand_value == best_hand:
                    pot_winners.append(player)

            # Рассчитываем вклад каждого игрока в этот банк
            contributions = {}
            previous_pots_amount = sum(prev_pot["amount"] for prev_pot in self.pots[:self.pots.index(pot)])
            for p in self.players:
                contribution = min(pot["amount"], max(0, min(p.total_bet, previous_pots_amount + pot[
                    "amount"]) - previous_pots_amount))
                contributions[p.name] = contribution

            # Делим банк между победителями
            winnings_per_player = pot["amount"] // len(pot_winners)
            for winner in pot_winners:
                winner_contribution = contributions[winner.name]
                net_winnings_for_pot = winnings_per_player - winner_contribution
                self.players[self.players.index(winner)].balance += winnings_per_player  # Начисляем весь банк
                total_net_winnings_by_player[
                    winner.name] += net_winnings_for_pot  # Чистый выигрыш для итогового результата
                print(
                    f"Adding winnings to {winner.name}: pot amount={pot['amount']}, contribution={winner_contribution}, net winnings={net_winnings_for_pot}, total pot added={winnings_per_player}, new balance: {self.players[self.players.index(winner)].balance}"
                )
            winners_by_pot.append({
                "pot_amount": pot["amount"],
                "winners": [w.name for w in pot_winners],
                "winnings_per_player": winnings_per_player
            })

        # Формируем результат
        total_net_winnings = sum(total_net_winnings_by_player.values())
        result = {
            "player": winners_by_pot[0]["winners"] if winners_by_pot else [],
            "hand": [str(card) for card in pot_winners[0].hand] if winners_by_pot and pot_winners else [],
            "winnings": total_net_winnings
        }
        print(f"Winners by pot: {winners_by_pot}, total net winnings: {total_net_winnings}")
        self.pots = [{"amount": 0, "eligible_players": [p.name for p in self.players]}]
        return result