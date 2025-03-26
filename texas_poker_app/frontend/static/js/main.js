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
        ws = new WebSocket(`ws://127.0.0.1:8000/game/ws?token=${token}`);

        ws.onopen = () => {
            console.log("WebSocket connected");
        };

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
                    updatePlayerList(data.players, username, currentWinner, null);
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
                    isWaiting = false; // Сбрасываем флаг ожидания
                    gameEnded = false;
                    currentWinner = null; // Сбрасываем победителя
                    currentPlayer = null;
                    playerHands = {}; // Сбрасываем руки игроков
                    lastCommunityCards = []; // Сбрасываем общие карты
                    lastStage = null;
                    document.getElementById("table-center").style.display = "none";
                    document.getElementById("player-info").style.display = "none";
                    document.getElementById("action-bar").style.display = "none";
                    document.getElementById("ready-button").style.display = "block";
                    toggleButtons(false);
                    const playerCircle = document.getElementById("player-circle");
                    const oldBetLabels = playerCircle.querySelectorAll(".player-bet");
                    oldBetLabels.forEach(label => label.remove());
                    if (data.players) {
                        data.players.forEach(player => {
                            playerBalances[player.player_id] = player.balance !== undefined ? player.balance : (playerBalances[player.player_id] || 0);
                            console.log(`Updated playerBalances[${player.player_id}] = ${playerBalances[player.player_id]}`);
                        });
                        updatePlayerList(data.players, username, null, null);
                    }
                }

                if (data.stage) {
                    console.log("Game state");
                    // Если игрок участвует в игре, сбрасываем флаг ожидания
                    if (data.players.some(p => p.name === username)) {
                        isWaiting = false;
                    }
                    document.getElementById("table-center").style.display = "block";
                    document.getElementById("player-info").style.display = "flex";
                    document.getElementById("stage").textContent = `Stage: ${data.stage}`;

                    if (!gameEnded) {
                        updatePlayerHand(data.hand, username === data.current_player);
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
                    updatePlayerList(data.players, username, data.stage === "showdown" ? currentWinner : null, data.stage);
                    const isActivePlayer = data.current_player === username && !data.players.find(p => p.name === username)?.folded;
                    console.log(`Current player: ${data.current_player}, isActivePlayer: ${isActivePlayer}, gameEnded: ${gameEnded}, stage: ${data.stage}`);
                    if (data.stage === "showdown") {
                        gameEnded = true; // Устанавливаем флаг окончания игры на шоудауне
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
                        updatePlayerList(data.players, username, currentWinner, data.stage);
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
        const value = card.replace(/[♠♥♦♣]/g, '');
        const suit = card.match(/[♠♥♦♣]/)?.[0] || 'spades';
        return `${suitMap[suit]}_${value.toLowerCase()}`;
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
            betLabel.textContent = totalBet;
            if (totalBet > 0) {
                betLabel.style.display = "block";
                betLabel.classList.add("visible");

                if (totalBet <= 50) {
                    betLabel.classList.add("bet-low");
                } else if (totalBet <= 200) {
                    betLabel.classList.add("bet-medium");
                } else {
                    betLabel.classList.add("bet-high");
                }

                const centerXPercent = (centerX / containerWidth) * 100;
                const centerYPercent = (centerY / containerHeight) * 100;

                const distanceToCenterX = centerXPercent - xPercent;
                const distanceToCenterY = centerYPercent - yPercent;
                const distanceToCenter = Math.sqrt(distanceToCenterX ** 2 + distanceToCenterY ** 2);

                const directionX = distanceToCenterX / distanceToCenter;
                const directionY = distanceToCenterY / distanceToCenter;

                const betOffsetPercent = 8;
                const betXOffsetPercent = directionX * betOffsetPercent;
                const betYOffsetPercent = directionY * betOffsetPercent;

                const betXAbsolutePercent = xPercent + betXOffsetPercent;
                const betYAbsolutePercent = yPercent + betYOffsetPercent;

                betLabel.style.left = `${betXAbsolutePercent}%`;
                betLabel.style.top = `${betYAbsolutePercent}%`;

                console.log(`Player ${playerId}: totalBet=${totalBet}, position=(${xPercent}%, ${yPercent}%), bet position=(${betXAbsolutePercent}%, ${betYAbsolutePercent}%)`);

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

            // Отображение карт игрока
            const isCurrentPlayer = (player.name === currentUsername) || (player.player_id === currentUsername);
            const isWinner = winnerName && (Array.isArray(winnerName) ? winnerName.includes(playerId) : playerId === winnerName);

            // Логируем состояние для отладки
            console.log(`Processing cards for ${playerId}: isCurrentPlayer=${isCurrentPlayer}, isWinner=${isWinner}, stage=${stage}, winnerName=${winnerName}, hand=${JSON.stringify(playerHands[playerId])}`);

            if (isCurrentPlayer && playerHands[playerId] && playerHands[playerId].length > 0) {
                // Всегда показываем свои карты, если они есть
                const hand = playerHands[playerId];
                console.log(`Displaying hand for current player ${playerId}: ${hand.join(", ")}`);
                updatePlayerHand(hand, true, cardContainer);
            } else if (stage === "showdown") {
                // На стадии шоудауна показываем карты всех игроков
                const hand = player.hand && player.hand.length > 0 ? player.hand : (playerHands[playerId] || []);
                console.log(`Showdown: Displaying hand for ${playerId}: ${hand.join(", ")}`);
                updatePlayerHand(hand, true, cardContainer);
            } else if (stage && !winnerName) {
                // Во время активной игры (есть stage, но нет победителя) показываем рубашки карт для других игроков
                console.log(`Displaying card backs for ${playerId} (active game, no winner)`);
                for (let i = 0; i < 2; i++) {
                    const img = document.createElement("img");
                    img.src = "/static/images/card_back_black.png";
                    cardContainer.appendChild(img);
                }
            } else if (winnerName && isWinner) {
                // Если есть победитель, показываем его карты
                const hand = player.hand && player.hand.length > 0 ? player.hand : (playerHands[playerId] || []);
                console.log(`Displaying winner's hand for ${playerId}: ${hand.join(", ")}`);
                updatePlayerHand(hand, true, cardContainer);
            } else if (winnerName && !isWinner) {
                // Для остальных игроков (не победителя) показываем рубашки карт, но не для текущего игрока
                console.log(`Displaying card backs for ${playerId} (has winner, not winner)`);
                for (let i = 0; i < 2; i++) {
                    const img = document.createElement("img");
                    img.src = "/static/images/card_back_black.png";
                    cardContainer.appendChild(img);
                }
            } else if (!stage && !winnerName) {
                // В предигровом состоянии без победителя карты не отображаются
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

    function updatePlayerHand(hand, isCurrentPlayer, container = document.getElementById("player-hand")) {
        container.innerHTML = "";
        if (hand && isCurrentPlayer) {
            hand.forEach(card => {
                const normalizedCard = normalizeCardName(card);
                const img = document.createElement("img");
                img.src = `/static/images/${normalizedCard}.png`;
                img.onerror = () => console.error(`Failed to load card: ${normalizedCard}.png`);
                container.appendChild(img);
            });
        } else if (!isCurrentPlayer) {
            for (let i = 0; i < 2; i++) {
                const img = document.createElement("img");
                img.src = "/static/images/card_back_black.png";
                container.appendChild(img);
            }
        }
    }

    function updateCommunityCards(cards, stage) {
        const container = document.getElementById("community-cards");
        if (!container) {
            console.error("Element #community-cards not found in DOM");
            return;
        }
        container.innerHTML = "";
        for (let i = 0; i < 5; i++) {
            const img = document.createElement("img");
            img.src = "/static/images/card_back_black.png";
            container.appendChild(img);
        }

        const visibleCards = cards || [];
        console.log("Community cards received:", visibleCards);
        visibleCards.forEach((card, index) => {
            if (card) {
                const normalizedCard = normalizeCardName(card);
                const img = container.children[index];
                img.src = `/static/images/${normalizedCard}.png`;
                img.onerror = () => console.error(`Failed to load community card: ${normalizedCard}.png`);
            }
        });

        if (stage === "Flop" && visibleCards.length >= 3) {
            for (let i = 0; i < 3; i++) {
                const normalizedCard = normalizeCardName(visibleCards[i]);
                container.children[i].src = `/static/images/${normalizedCard}.png`;
                container.children[i].onerror = () => console.error(`Failed to load Flop card ${i}: ${normalizedCard}.png`);
            }
        } else if (stage === "Turn" && visibleCards.length >= 4) {
            for (let i = 0; i < 4; i++) {
                const normalizedCard = normalizeCardName(visibleCards[i]);
                container.children[i].src = `/static/images/${normalizedCard}.png`;
                container.children[i].onerror = () => console.error(`Failed to load Turn card ${i}: ${normalizedCard}.png`);
            }
        } else if (stage === "River" && visibleCards.length === 5) {
            for (let i = 0; i < 5; i++) {
                const normalizedCard = normalizeCardName(visibleCards[i]);
                container.children[i].src = `/static/images/${normalizedCard}.png`;
                container.children[i].onerror = () => console.error(`Failed to load River card ${i}: ${normalizedCard}.png`);
            }
        }
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