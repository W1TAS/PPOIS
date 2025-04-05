document.addEventListener("DOMContentLoaded", () => {
    console.log("DOM fully loaded");

    document.addEventListener("wheel", (event) => {
        if (event.ctrlKey) {
            event.preventDefault();
        }
    }, { passive: false });

    document.addEventListener("keydown", (event) => {
        if (event.ctrlKey && (event.key === "+" || event.key === "-" || event.key === "0")) {
            event.preventDefault();
        }
    });

    let ws;
    let token = null;
    let username = null;
    let playerBalances = {};
    let playerHands = {};
    let currentWinner = null;
    let gameEnded = false;
    let currentPlayer = null;
    let lastCommunityCards = [];
    let lastStage = null;
    let isWaiting = false; // Флаг для состояния ожидания
    let cardTurnSound = new Audio('/static/sounds/card_turn.mp3');
    let lastVisibleCardCount = 0; // Для отслеживания количества видимых карт
    let oneChipSound = new Audio('/static/sounds/one_chip.mp3');
    let smallBetSound = new Audio('/static/sounds/small_bet.mp3');
    let normalBetSound = new Audio('/static/sounds/normal_bet.mp3');
    let bigAllInSound = new Audio('/static/sounds/big_all_in.mp3');
    let previousBets = {};
    let isNewGame = true; // Флаг для новой игры

    function showGame() {
        console.log("Switching to game view");
        document.getElementById("auth").style.display = "none";
        const gameTable = document.getElementById("game-table");
        if (gameTable) {
            gameTable.style.display = "block";
            console.log("Game table displayed");
        } else {
            console.error("Element #game-table not found");
        }
    }

    async function login() {
        const usernameInput = document.getElementById("username").value;
        const password = document.getElementById("password").value;
        const response = await fetch("/auth/token", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username: usernameInput, password })
        });
        if (response.ok) {
            const data = await response.json();
            token = data.access_token;
            username = usernameInput;
            connectWebSocket();
            showGame();
        } else {
            alert("Login failed");
        }
    }

    async function register() {
        const username = document.getElementById("username").value;
        const password = document.getElementById("password").value;
        const response = await fetch("/auth/register", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username, password })
        });
        if (response.ok) {
            alert("Registered successfully, now login");
        } else {
            alert("Registration failed");
        }
    }

    function connectWebSocket() {
        // Определяем протокол (ws или wss) и хост автоматически
        const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
        const host = window.location.host; // Например, "texas-poker-app.onrender.com" или "localhost:8000"
        const wsUrl = `${protocol}//${host}/game/ws?token=${token}`;
        console.log("Connecting to WebSocket at:", wsUrl);
        ws = new WebSocket(wsUrl);

        ws.onopen = () => {
            console.log("WebSocket connected");
        };

        // Остальной код функции остаётся без изменений
        ws.onmessage = function(event) {
            try {
                if (!event.data) {
                    console.error("Received empty WebSocket message");
                    return;
                }
                const data = JSON.parse(event.data);
                console.log("Received WebSocket message:", data);

                // Обработка пинг-понг сообщений
                if (data.type === "ping") {
                    if (ws.readyState === WebSocket.OPEN) {
                        ws.send(JSON.stringify({ type: "pong" }));
                        console.log("Sent pong response");
                    }
                    return;
                }

                // Обработка состояния ожидания
                if (data.message === "Waiting for the current round to finish") {
                    console.log("Player is waiting for the current round to finish");
                    isWaiting = true; // Устанавливаем флаг ожидания
                    gameEnded = false;

                    // Отображаем интерфейс, но без возможности взаимодействия
                    document.getElementById("table-center").style.display = "block";
                    document.getElementById("player-info").style.display = "flex";
                    document.getElementById("stage").textContent = "Waiting for the current round to finish...";
                    if (data.pots) {
                        const total_pot = data.pots.reduce((sum, pot) => sum + pot.amount, 0);
                        document.getElementById("pot").textContent = `Pot: ${total_pot} (${data.pots.map(p => p.amount).join(" | ")})`;
                    }
                    if (data.players) {
                        updatePlayerList(data.players, username, null, data.stage);
                    }
                    if (data.community_cards) {
                        updateCommunityCards(data.community_cards, data.stage);
                        lastCommunityCards = data.community_cards || [];
                        lastStage = data.stage;
                    }
                    document.getElementById("action-bar").style.display = "none";
                    document.getElementById("ready-button").style.display = "none"; // Скрываем кнопку "Ready"
                    toggleButtons(false);
                    return;
                }

                if (data.error) {
                    console.log("Error received:", data.error);
                    alert(data.error);
                    document.getElementById("table-center").style.display = "none";
                    document.getElementById("player-info").style.display = "none";
                    document.getElementById("stage").textContent = "";
                    document.getElementById("pot").textContent = "";
                    gameEnded = false;
                    lastCommunityCards = [];
                    lastStage = null;
                    currentWinner = null;
                    return;
                }

                if (data.players && !data.stage && !data.winner && !data.all_ready) {
                    console.log("Pre-game state without winner, stage, or all ready");
                    gameEnded = false;
                    isWaiting = false; // Сбрасываем isWaiting для всех игроков
                    data.players.forEach(player => {
                        playerBalances[player.player_id] = player.balance !== undefined ? player.balance : (playerBalances[player.player_id] || 0);
                        console.log(`Updated playerBalances[${player.player_id}] = ${playerBalances[player.player_id]}`);
                    });
                    try {
                        updatePlayerList(data.players, username, currentWinner, null);
                    } catch (error) {
                        console.error("Error in updatePlayerList:", error);
                    }
                    // Показываем кнопку "Ready" только если игрок не в ожидании
                    console.log(`isWaiting=${isWaiting}, showing ready button: ${!isWaiting}`);
                    if (!isWaiting) {
                        console.log("Displaying ready button");
                        document.getElementById("ready-button").style.display = "block";
                        document.getElementById("action-bar").style.display = "none"; // Явно скрываем кнопки действий
                        toggleButtons(false);
                    }
                    if (lastCommunityCards.length > 0) {
                        document.getElementById("table-center").style.display = "block";
                        document.getElementById("player-info").style.display = "flex";
                        updateCommunityCards(lastCommunityCards, lastStage);
                    } else {
                        document.getElementById("table-center").style.display = "none";
                        document.getElementById("player-info").style.display = "none";
                    }
                    document.getElementById("stage").textContent = "";
                    document.getElementById("pot").textContent = "";
                }

                if (data.all_ready) {
                    console.log("All players ready, resetting game state");
                    isWaiting = false;
                    gameEnded = false;
                    currentWinner = null;
                    currentPlayer = null;
                    playerHands = {};
                    lastCommunityCards = [];
                    lastStage = null;
                    lastVisibleCardCount = 0; // Сбрасываем счётчик видимых карт
                    isNewGame = true;
                    document.getElementById("table-center").style.display = "none";
                    document.getElementById("player-info").style.display = "none";
                    document.getElementById("action-bar").style.display = "none";
                    document.getElementById("ready-button").style.display = "block";
                    toggleButtons(false);
                    const playerCircle = document.getElementById("player-circle");
                    const oldBetLabels = playerCircle.querySelectorAll(".player-bet");
                    oldBetLabels.forEach(label => label.remove());
                    // Очищаем общие карты
                    const communityCardsContainer = document.getElementById("community-cards");
                    if (communityCardsContainer) {
                        communityCardsContainer.innerHTML = "";
                    }
                    if (data.players) {
                        data.players.forEach(player => {
                            playerBalances[player.player_id] = player.balance !== undefined ? player.balance : (playerBalances[player.player_id] || 0);
                            console.log(`Updated playerBalances[${player.player_id}] = ${playerBalances[player.player_id]}`);
                        });
                        try {
                            updatePlayerList(data.players, username, null, null);
                        } catch (error) {
                            console.error("Error in updatePlayerList (all_ready):", error);
                        }
                    }
                }

                if (data.stage) {
                    console.log("Game state");
                    if (data.stage === "preflop" && lastStage !== "preflop") {
                        console.log("New game started (preflop), setting isNewGame to true");
                        isNewGame = true;
                        lastVisibleCardCount = 0; // Сбрасываем счётчик видимых карт
                        const communityCardsContainer = document.getElementById("community-cards");
                        if (communityCardsContainer) {
                            communityCardsContainer.innerHTML = ""; // Очищаем общие карты
                        }
                    }

                    if (data.players.some(p => p.name === username)) {
                        isWaiting = false;
                    }
                    document.getElementById("table-center").style.display = "block";
                    document.getElementById("player-info").style.display = "flex";
                    document.getElementById("stage").textContent = `Stage: ${data.stage}`;

                    if (!gameEnded) {
                        if (data.hand) {
                            console.log(`Received hand for ${username}: ${JSON.stringify(data.hand)}`);
                            playerHands[username] = data.hand;
                            // Убрали сброс isNewGame здесь
                        } else {
                            console.log(`No hand received for ${username} in stage ${data.stage}`);
                        }
                        updateCommunityCards(data.community_cards, data.stage);
                        lastCommunityCards = data.community_cards || [];
                        lastStage = data.stage;
                        if (data.pots) {
                            console.log("Pots received:", data.pots);
                            const total_pot = data.pots.reduce((sum, pot) => sum + pot.amount, 0);
                            const potText = `Pot: ${total_pot} (${data.pots.map(p => p.amount).join(" | ")})`;
                            document.getElementById("pot").textContent = potText;
                            console.log("Pot displayed:", potText);
                        } else {
                            console.log("No pots received in game state");
                            document.getElementById("pot").textContent = "Pot: 0";
                        }
                    }

                    if (data.balance !== undefined) {
                        playerBalances[username] = data.balance;
                        console.log(`Updated playerBalances[${username}] = ${data.balance} from data.balance`);
                    }

                    if (data.players) {
                        data.players.forEach(player => {
                            playerBalances[player.name] = player.balance !== undefined ? player.balance : (playerBalances[player.name] || 0);
                            console.log(`Updated playerBalances[${player.name}] = ${playerBalances[player.name]} from data.players`);
                            if (player.name === username && data.hand) {
                                playerHands[player.name] = data.hand;
                            }
                        });
                    }

                    document.getElementById("balance").innerHTML = `${username} <span>${playerBalances[username]}</span>`;
                    currentPlayer = data.current_player;
                    try {
                        updatePlayerList(data.players, username, data.stage === "showdown" ? currentWinner : null, data.stage);
                    } catch (error) {
                        console.error("Error in updatePlayerList (stage):", error);
                    }
                    const isActivePlayer = data.current_player === username && !data.players.find(p => p.name === username)?.folded;
                    console.log(`Current player: ${data.current_player}, isActivePlayer: ${isActivePlayer}, gameEnded: ${gameEnded}, stage: ${data.stage}`);
                    if (data.stage === "showdown") {
                        gameEnded = true;
                        console.log("Showdown reached, resetting isNewGame to true for next game");
                        isNewGame = true;
                    }
                    if (data.stage && data.stage !== "showdown" && isActivePlayer && !isWaiting) {
                        document.getElementById("action-bar").style.display = "flex";
                        toggleButtons(true);
                        document.getElementById("ready-button").style.display = "none";
                    } else {
                        document.getElementById("action-bar").style.display = "none";
                        toggleButtons(false);
                        if (data.stage !== "showdown" && !isWaiting) {
                            document.getElementById("ready-button").style.display = "none";
                        }
                    }
                    const betAmount = document.getElementById("bet-amount");
                    if (betAmount) {
                        betAmount.max = data.balance;
                        betAmount.min = data.current_bet + 1 || 1;
                    }
                }

                // Состояние победителя
                if (data.winner) {
                    console.log("Winner state:", data.winner);
                    gameEnded = true;
                    const winner = data.winner;
                    const winnerName = typeof data.winner === "object"
                        ? (data.winner.name || (Array.isArray(data.winner.player) ? data.winner.player[0] : data.winner.player) || data.winner.player_id)
                        : data.winner;
                    console.log("Extracted winnerName:", winnerName);
                    currentWinner = winnerName;
                    currentPlayer = null;
                    if (data.players) {
                        data.players.forEach(player => {
                            playerBalances[player.name] = player.balance;
                            console.log(`Updated playerBalances[${player.name}] = ${playerBalances[player.name]} from winner state`);
                            if (player.hand) {
                                console.log(`${player.name}: ${player.hand.join(", ")}`);
                                playerHands[player.name] = player.hand;
                            }
                        });
                        try {
                            updatePlayerList(data.players, username, currentWinner, data.stage);
                        } catch (error) {
                            console.error("Error in updatePlayerList (winner):", error);
                        }
                    } else {
                        console.warn("No players data with winner message");
                    }
                    toggleButtons(false);
                    document.getElementById("action-bar").style.display = "none";
                    data.players.forEach(player => {
                        if (player.name === username && player.ready === false) {
                            document.getElementById("ready-button").style.display = "block";
                        } else if (player.name === username && player.ready === true) {
                            document.getElementById("ready-button").style.display = "none";
                        }
                    });
                    document.getElementById("table-center").style.display = "block";
                    document.getElementById("player-info").style.display = "flex";
                    updateCommunityCards(lastCommunityCards, lastStage);
                }

                if (data.type === "winner") {
                    console.log("Winner announced:", data.winner);
                    if (data.winner && data.winner.player) {
                        const winnerMessage = `Winner: ${data.winner.player} with hand ${data.winner.hand.join(", ")}`;
                        console.log(winnerMessage);
                    }
                }
            } catch (error) {
                console.error("Error parsing WebSocket message:", error, "Raw data:", event.data);
            }
        };

        ws.onclose = (event) => {
            console.log("WebSocket closed with code:", event.code, "reason:", event.reason);
            setTimeout(() => {
                connectWebSocket();
            }, 3000);
        };

        ws.onerror = (error) => {
            console.error("WebSocket error:", error);
        };
    }

    function normalizeCardName(card) {
        if (!card) return "card_back_black";

        const suitMap = {
            '♠': 'spades',
            '♥': 'hearts',
            '♦': 'diamonds',
            '♣': 'clubs'
        };

        const valueMap = {
            'A': 'ace',
            'K': 'king',
            'Q': 'queen',
            'J': 'jack'
        };

        const value = card.replace(/[♠♥♦♣]/g, '');
        const suit = card.match(/[♠♥♦♣]/)?.[0] || 'spades';

        // Преобразуем значение карты
        const normalizedValue = valueMap[value] || value.toLowerCase();

        return `${normalizedValue}_of_${suitMap[suit]}`;
    }

    function updatePlayerList(players, currentUsername, winnerName = null, stage = null) {
    console.log(`updatePlayerList called with currentUsername=${currentUsername}, winnerName=${winnerName}, stage=${stage}`);
    const gameContainer = document.getElementById("game-container");
    if (!gameContainer) {
        console.error("Element #game-container not found in DOM");
        return;
    }
    const playerCircle = document.getElementById("player-circle");
    if (!playerCircle) {
        console.error("Element #player-circle not found in DOM");
        return;
    }
    while (playerCircle.firstChild) {
        playerCircle.removeChild(playerCircle.firstChild);
    }

    const containerWidth = gameContainer.offsetWidth;
    const containerHeight = gameContainer.offsetHeight;
    const aspectRatio = containerWidth / containerHeight;
    const radius = Math.min(containerWidth, containerHeight) * 0.3;

    const centerX = containerWidth / 2;
    const centerY = containerHeight * 0.42;

    const currentPlayerIndex = players.findIndex(p => p.player_id === currentUsername || p.name === currentUsername);
    const totalPlayers = players.length;

    const reorderedPlayers = [];
    for (let i = 0; i < totalPlayers; i++) {
        const index = (currentPlayerIndex + i) % totalPlayers;
        reorderedPlayers.push(players[index]);
    }

    reorderedPlayers.forEach((player, index) => {
        console.log(`Player ${player.name || player.player_id}: is_dealer=${player.is_dealer}, is_small_blind=${player.is_small_blind}, is_big_blind=${player.is_big_blind}, ready=${player.ready}, balance=${player.balance}`);

        const angleDeg = 90 + (360 / totalPlayers) * index;
        const angleRad = (angleDeg * Math.PI) / 180;
        const xOffset = radius * Math.cos(angleRad) * aspectRatio;
        const yOffset = radius * Math.sin(angleRad);

        const xPercent = ((centerX + xOffset) / containerWidth) * 100;
        const yPercent = ((centerY + yOffset) / containerHeight) * 100;

        const playerDiv = document.createElement("div");
        playerDiv.className = "player";
        // Добавляем класс current-player для текущего игрока
        if (player.name === currentUsername || player.player_id === currentUsername) {
            playerDiv.classList.add("current-player");
        }
        playerDiv.style.left = `${xPercent}%`;
        playerDiv.style.top = `${yPercent}%`;
        playerDiv.setAttribute("data-position", angleDeg.toString());

        const playerId = player.player_id || player.name;
        const balance = player.balance !== undefined ? player.balance : (playerBalances[playerId] || 0);
        console.log(`Displaying balance for ${playerId}: ${balance}`);
        const cardContainer = document.createElement("div");
        cardContainer.className = "card-container";

        const betLabel = document.createElement("div");
        betLabel.className = "player-bet";
        betLabel.setAttribute("data-player-id", playerId);
        const totalBet = player.total_bet || 0;
        betLabel.innerHTML = "";

        const betText = document.createElement("span");
        betText.textContent = totalBet;
        betLabel.appendChild(betText);

        const chipsImg = document.createElement("img");
        if (totalBet <= 50) {
            chipsImg.src = "/static/images/chips1.png";
            chipsImg.className = "bet-chips bet-chips-1";
        } else if (totalBet <= 200) {
            chipsImg.src = "/static/images/chips2.png";
            chipsImg.className = "bet-chips bet-chips-2";
        } else if (totalBet <= 500) {
            chipsImg.src = "/static/images/chips3.png";
            chipsImg.className = "bet-chips bet-chips-3";
        } else {
            chipsImg.src = "/static/images/chips4.png";
            chipsImg.className = "bet-chips bet-chips-4";
        }
        chipsImg.alt = "Chips";
        betLabel.appendChild(chipsImg);

        if (totalBet > 0) {
            betLabel.style.display = "block";
            betLabel.classList.add("visible");
            if (totalBet <= 50) {
                betLabel.classList.add("bet-low");
            } else if (totalBet <= 200) {
                betLabel.classList.add("bet-medium");
            } else if (totalBet <= 500) {
                betLabel.classList.add("bet-high");
            } else {
                betLabel.classList.add("bet-very-high");
            }

            const centerXPercent = (centerX / containerWidth) * 100;
            const centerYPercent = (centerY / containerHeight) * 100;
            const betOffsetPercent = 8;
            const isAboveCenter = yPercent < centerYPercent;
            const isLeftOfCenter = xPercent < centerXPercent;
            const betYOffsetPercent = isAboveCenter ? betOffsetPercent : -betOffsetPercent;
            const betXOffsetPercent = isLeftOfCenter ? betOffsetPercent : -betOffsetPercent;
            const betXAbsolutePercent = xPercent + betXOffsetPercent;
            const betYAbsolutePercent = yPercent + betYOffsetPercent;

            betLabel.style.left = `${betXAbsolutePercent}%`;
            betLabel.style.top = `${betYAbsolutePercent}%`;
            console.log(`Player ${playerId}: totalBet=${totalBet}, position=(${xPercent}%, ${yPercent}%), bet position=(${betXAbsolutePercent}%, ${betYAbsolutePercent}%)`);

            const previousBet = previousBets[playerId] || 0;
            let soundToPlay;
            if (totalBet - previousBet <= 20) {
                soundToPlay = oneChipSound;
            } else if (totalBet - previousBet < 200) {
                soundToPlay = smallBetSound;
            } else if (totalBet - previousBet < 500) {
                soundToPlay = normalBetSound;
            } else {
                soundToPlay = bigAllInSound;
            }
            soundToPlay.play().catch(error => {
                console.error("Error playing bet sound:", error);
            });
            previousBets[playerId] = totalBet;
            playerCircle.appendChild(betLabel);
        } else {
            console.log(`Player ${playerId}: totalBet=${totalBet}, bet label hidden`);
            betLabel.style.display = "none";
        }

        const roleLabel = document.createElement("div");
        roleLabel.className = "role-label";
        roleLabel.style.left = `calc(${xPercent}% + 3vw)`;
        roleLabel.style.top = `${yPercent}%`;
        roleLabel.style.zIndex = "10";
        if (player.is_dealer) {
            roleLabel.textContent = "D";
            roleLabel.style.backgroundColor = "#FFD700";
        } else if (player.is_small_blind) {
            roleLabel.textContent = "SB";
            roleLabel.style.backgroundColor = "#87CEEB";
        } else if (player.is_big_blind) {
            roleLabel.textContent = "BB";
            roleLabel.style.backgroundColor = "#FF6347";
        } else {
            roleLabel.style.display = "none";
        }
        playerCircle.appendChild(roleLabel);

        if (winnerName) {
            const isWinner = Array.isArray(winnerName)
                ? winnerName.includes(playerId)
                : playerId === winnerName;
            if (isWinner) {
                console.log(`Adding Winner label for ${playerId}, winnerName: ${JSON.stringify(winnerName)}`);
                const winnerLabel = document.createElement("div");
                winnerLabel.className = "winner-label";
                winnerLabel.textContent = "Winner";
                playerDiv.appendChild(winnerLabel);
            }
        }

        if (currentPlayer && playerId === currentPlayer && !winnerName && playerId !== currentUsername) {
            const currentLabel = document.createElement("div");
            currentLabel.className = "current-label";
            currentLabel.textContent = "Current";
            playerDiv.appendChild(currentLabel);
        }

        const isCurrentPlayer = (player.name === currentUsername) || (player.player_id === currentUsername);
        const isWinner = winnerName && (Array.isArray(winnerName) ? winnerName.includes(playerId) : playerId === winnerName);
        console.log(`Processing cards for ${playerId}: isCurrentPlayer=${isCurrentPlayer}, isWinner=${isWinner}, stage=${stage}, winnerName=${winnerName}, hand=${JSON.stringify(playerHands[playerId])}`);

        if (isCurrentPlayer && playerHands[playerId] && playerHands[playerId].length > 0) {
            console.log(`Displaying hand for current player ${playerId} in .card-container`);
            const hand = playerHands[playerId];
            updatePlayerHand(hand, true, cardContainer, stage, isWinner);
            if (stage === "preflop" && isNewGame) {
                console.log("Cards displayed for current player on preflop, setting isNewGame to false");
                isNewGame = false;
            }
        } else if (stage === "showdown") {
            const hand = player.hand && player.hand.length > 0 ? player.hand : (playerHands[playerId] || []);
            console.log(`Showdown: Displaying hand for ${playerId}: ${hand.join(", ")}`);
            updatePlayerHand(hand, false, cardContainer, stage, isWinner);
        } else if (stage && !winnerName) {
            console.log(`Displaying card backs for ${playerId} (active game, no winner)`);
            for (let i = 0; i < 2; i++) {
                const img = document.createElement("img");
                img.src = "/static/images/card_back_black.png";
                cardContainer.appendChild(img);
            }
        } else if (winnerName && isWinner) {
            const hand = player.hand && player.hand.length > 0 ? player.hand : (playerHands[playerId] || []);
            console.log(`Displaying winner's hand for ${playerId}: ${hand.join(", ")}`);
            updatePlayerHand(hand, false, cardContainer, stage, true);
        } else if (winnerName && !isWinner) {
            console.log(`Displaying card backs for ${playerId} (has winner, not winner)`);
            for (let i = 0; i < 2; i++) {
                const img = document.createElement("img");
                img.src = "/static/images/card_back_black.png";
                cardContainer.appendChild(img);
            }
        } else if (!stage && !winnerName) {
            console.log(`No cards displayed for ${playerId} (pre-game state, no winner)`);
        }

        if (player.ready !== undefined) {
            playerDiv.innerHTML += `${playerId}<br>${player.ready ? "Ready" : "Not Ready"}<br><span class="balance">${balance}</span>`;
        } else {
            playerDiv.innerHTML += `${playerId}<br><span class="balance">${balance}</span>`;
        }
        playerDiv.appendChild(cardContainer);
        playerCircle.appendChild(playerDiv);
    });
}

    function updatePlayerHand(hand, isCurrentPlayer, container, stage = null, isWinner = false) {
    if (!container) {
        console.error("Container not found in DOM");
        return;
    }

    console.log(`updatePlayerHand called: isCurrentPlayer=${isCurrentPlayer}, hand=${JSON.stringify(hand)}, container=${container.id || container.className}, stage=${stage}, isWinner=${isWinner}`);

    container.innerHTML = ""; // Очищаем контейнер перед рендерингом

    if (!hand || hand.length === 0) {
        console.log("No hand data received, keeping container empty");
        return;
    }

    if (isCurrentPlayer) {
        console.log(`Rendering hand for current player: ${JSON.stringify(hand)}`);
        hand.forEach((card, index) => {
            const cardWrapper = document.createElement("div");
            cardWrapper.className = "card-wrapper";
            cardWrapper.setAttribute("data-card-id", `${index}`);

            const cardInner = document.createElement("div");
            cardInner.className = "card-inner";

            const cardFront = document.createElement("div");
            cardFront.className = "card-front";
            const frontImg = document.createElement("img");
            const normalizedCard = normalizeCardName(card);
            frontImg.src = `/static/images/${normalizedCard}.png`;
            frontImg.alt = card;
            frontImg.onerror = () => console.error(`Failed to load card: ${normalizedCard}.png`);
            cardFront.appendChild(frontImg);

            // Добавляем тыльную сторону только если будет анимация
            if (isNewGame && stage === "preflop") {
                console.log(`Animating card ${index}: ${card}, isNewGame=${isNewGame}, stage=${stage}`);
                setTimeout(() => {
                    console.log(`Flipping card ${index}: ${card}`);
                    if (cardWrapper) {
                        cardWrapper.classList.add("flipped");
                        console.log("Flipped class added");
                        cardTurnSound.play().catch(error => {
                            console.error("Sound error:", error);
                        });
                    } else {
                        console.error("cardWrapper not found");
                    }
                }, index * 300);
            } else {
                console.log(`No animation for card ${index}: stage=${stage}`);
                cardInner.style.transform = "rotateY(180deg)"; // Без анимации
            }

            cardInner.appendChild(cardFront);
            cardWrapper.appendChild(cardInner);
            container.appendChild(cardWrapper);

            // Применяем анимацию только если это новая игра и этап preflop
            if (isNewGame && stage === "preflop") {
                setTimeout(() => {
                    console.log(`Flipping card ${index}: ${card}`);
                    cardWrapper.classList.add("flipped");
                    cardTurnSound.play().catch(error => {
                        console.error("Error playing card turn sound:", error);
                    });
                }, index * 300);
            } else {
                // Без анимации сразу показываем лицевую сторону
                cardInner.style.transform = "rotateY(180deg)";
            }
        });
    } else {
        console.log("Not current player, showing cards or backs");
        if (stage === "showdown" || isWinner) {
            // Показываем карты для других игроков на showdown или если это победитель
            hand.forEach(card => {
                const normalizedCard = normalizeCardName(card);
                const img = document.createElement("img");
                img.src = `/static/images/${normalizedCard}.png`;
                img.onerror = () => console.error(`Failed to load card: ${normalizedCard}.png`);
                container.appendChild(img);
            });
        } else {
            // Показываем рубашки для других игроков
            for (let i = 0; i < 2; i++) {
                const img = document.createElement("img");
                img.src = "/static/images/card_back_black.png";
                container.appendChild(img);
            }
        }
    }

    console.log(`After rendering, container contains ${container.children.length} children`);
}

    function updateCommunityCards(cards, stage) {
    const container = document.getElementById("community-cards");
    if (!container) {
        console.error("Element #community-cards not found in DOM");
        return;
    }

    // Сохраняем текущее состояние контейнера, чтобы не перерисовывать уже открытые карты
    const existingCards = container.children.length;
    if (!existingCards) {
        // Если контейнер пуст, создаём 5 слотов для карт
        for (let i = 0; i < 5; i++) {
            const cardWrapper = document.createElement("div");
            cardWrapper.className = "card-wrapper";
            cardWrapper.setAttribute("data-card-id", `${i}`);

            const cardInner = document.createElement("div");
            cardInner.className = "card-inner";

            const cardFront = document.createElement("div");
            cardFront.className = "card-front";
            const frontImg = document.createElement("img");
            frontImg.src = "/static/images/card_back_black.png"; // По умолчанию рубашка
            frontImg.alt = "Card back";
            cardFront.appendChild(frontImg);

            const cardBack = document.createElement("div");
            cardBack.className = "card-back";
            const backImg = document.createElement("img");
            backImg.src = "/static/images/card_back_black.png";
            backImg.alt = "Card back";
            cardBack.appendChild(backImg);

            cardInner.appendChild(cardFront);
            cardInner.appendChild(cardBack);
            cardWrapper.appendChild(cardInner);
            container.appendChild(cardWrapper);
        }
    }

    const visibleCards = cards || [];
    console.log("Community cards received:", visibleCards);

    // Подсчитываем количество новых видимых карт
    const newVisibleCardCount = visibleCards.filter(card => card).length;
    let cardsToReveal = 0;

    // Определяем, сколько новых карт нужно открыть
    if (newVisibleCardCount > lastVisibleCardCount) {
        cardsToReveal = newVisibleCardCount - lastVisibleCardCount;
    }

    // Обновляем карты
    visibleCards.forEach((card, index) => {
        if (card) {
            const normalizedCard = normalizeCardName(card);
            const cardWrapper = container.children[index];
            const frontImg = cardWrapper.querySelector(".card-front img");
            frontImg.src = `/static/images/${normalizedCard}.png`;
            frontImg.alt = card;
            frontImg.onerror = () => console.error(`Failed to load community card: ${normalizedCard}.png`);

            // Применяем анимацию только для новых карт
            if (index >= lastVisibleCardCount && index < newVisibleCardCount) {
                setTimeout(() => {
                    console.log(`Flipping community card ${index}: ${card}`);
                    cardWrapper.classList.add("flipped");
                    cardTurnSound.play().catch(error => {
                        console.error("Error playing card turn sound:", error);
                    });
                }, (index - lastVisibleCardCount) * 300); // Задержка для последовательного переворота
            } else {
                // Для уже открытых карт просто показываем лицевую сторону
                cardWrapper.querySelector(".card-inner").style.transform = "rotateY(180deg)";
            }
        }
    });

    lastVisibleCardCount = newVisibleCardCount;
}

    function toggleButtons(enabled) {
        const buttons = ["bet-button", "call-pass-button", "fold-button", "all-in-button", "bet-amount"];
        buttons.forEach(id => {
            const element = document.getElementById(id);
            if (element) {
                element.disabled = !enabled;
            } else {
                console.error(`Element with ID "${id}" not found in toggleButtons`);
            }
        });
    }

    function addEventListenerSafe(id, event, callback) {
        const element = document.getElementById(id);
        if (element) {
            element.addEventListener(event, callback);
        } else {
            console.error(`Element with ID "${id}" not found during initialization`);
        }
    }

    addEventListenerSafe("login-button", "click", login);
    addEventListenerSafe("register-button", "click", register);
    addEventListenerSafe("ready-button", "click", () => {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({ action: "ready", ready: true }));
        }
    });
    addEventListenerSafe("bet-button", "click", () => {
        if (ws && ws.readyState === WebSocket.OPEN) {
            const amount = parseInt(document.getElementById("bet-amount").value);
            if (amount > 0) {
                ws.send(JSON.stringify({ action: "bet", value: amount }));
            }
        }
    });
    addEventListenerSafe("call-pass-button", "click", () => {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({ action: "call_or_pass" }));
        }
    });
    addEventListenerSafe("fold-button", "click", () => {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({ action: "fold" }));
        }
    });
    addEventListenerSafe("all-in-button", "click", () => {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({ action: "all_in" }));
        }
    });
});