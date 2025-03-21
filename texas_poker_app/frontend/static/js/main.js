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
    let gameEnded = false; // Добавляем флаг для отслеживания окончания игры
    let currentPlayer = null;

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
            const data = JSON.parse(event.data);
            console.log("Received WebSocket message:", data);

            // Обновляем баланс в состоянии "Pre-game"
            if (data.players && !data.stage && !data.winner && !data.all_ready) {
                console.log("Pre-game state without winner, stage, or all ready");
                gameEnded = false;
                data.players.forEach(player => {
                    playerBalances[player.player_id] = player.balance !== undefined ? player.balance : (playerBalances[player.player_id] || 0);
                    console.log(`Updated playerBalances[${player.player_id}] = ${playerBalances[player.player_id]}`);
                });
                updatePlayerList(data.players, username, currentWinner);
                document.getElementById("ready-button").style.display = "block";
                document.getElementById("action-bar").style.display = "none";
                toggleButtons(false);
                document.getElementById("table-center").style.display = "none";
                document.getElementById("player-info").style.display = "none";
            }

            // Обновляем баланс в состоянии "All ready"
            if (data.all_ready) {
                console.log("All players ready, resetting game state");
                gameEnded = false;
                currentWinner = null;
                currentPlayer = null;
                playerHands = {};
                document.getElementById("table-center").style.display = "none";
                document.getElementById("player-info").style.display = "none";
                document.getElementById("action-bar").style.display = "none";
                document.getElementById("ready-button").style.display = "block";
                toggleButtons(false);
                if (data.players) {
                    data.players.forEach(player => {
                        playerBalances[player.player_id] = player.balance !== undefined ? player.balance : (playerBalances[player.player_id] || 0);
                        console.log(`Updated playerBalances[${player.player_id}] = ${playerBalances[player.player_id]}`);
                    });
                    updatePlayerList(data.players, username, null);
                }
            }

            // Обновляем баланс в состоянии игры
            if (data.stage) {
                console.log("Game state");
                document.getElementById("table-center").style.display = "block";
                document.getElementById("player-info").style.display = "flex";
                document.getElementById("stage").textContent = `Stage: ${data.stage}`;

                if (!gameEnded) {
                    updatePlayerHand(data.hand, username === data.current_player);
                    updateCommunityCards(data.community_cards, data.stage);
                    if (data.pots) {
                        console.log("Pots received:", data.pots);
                        const total_pot = data.pots.reduce((sum, pot) => sum + pot.amount, 0);
                        const potText = `Pot: ${total_pot} (${data.pots.map(p => p.amount).join(" | ")})`;
                        document.getElementById("pot").textContent = potText;
                        console.log("Pot displayed:", potText); // Добавляем лог
                    } else {
                        console.log("No pots received in game state");
                        document.getElementById("pot").textContent = "Pot: 0";
                    }
                }

                // Обновляем баланс текущего игрока из data.balance
                if (data.balance !== undefined) {
                    playerBalances[username] = data.balance;
                    console.log(`Updated playerBalances[${username}] = ${playerBalances[username]} from data.balance`);
                }

                // Обновляем баланс для всех игроков из data.players
                if (data.players) {
                    data.players.forEach(player => {
                        playerBalances[player.name] = player.balance !== undefined ? player.balance : (playerBalances[player.name] || 0);
                        console.log(`Updated playerBalances[${player.name}] = ${playerBalances[player.name]} from data.players`);
                        if (player.name === username && data.hand) {
                            playerHands[player.name] = data.hand;
                        }
                    });
                }

                // Обновляем отображение баланса текущего игрока
                document.getElementById("balance").innerHTML = `${username} <span>${playerBalances[username]}</span>`;
                currentPlayer = data.current_player;
                updatePlayerList(data.players, username, data.stage === "showdown" ? currentWinner : null);
                document.getElementById("ready-button").style.display = "none";
                const isActivePlayer = data.current_player === username && !data.players.find(p => p.name === username)?.folded;
                console.log(`Current player: ${data.current_player}, isActivePlayer: ${isActivePlayer}, gameEnded: ${gameEnded}, stage: ${data.stage}`);
                if (data.stage && data.stage !== "showdown" && isActivePlayer) {
                    document.getElementById("action-bar").style.display = "flex";
                    toggleButtons(true);
                } else {
                    document.getElementById("action-bar").style.display = "none";
                    toggleButtons(false);
                    if (data.stage === "showdown") {
                        document.getElementById("ready-button").style.display = "block";
                        document.getElementById("table-center").style.display = "block";
                        document.getElementById("player-info").style.display = "flex";
                    }
                }
                const betAmount = document.getElementById("bet-amount");
                if (betAmount) {
                    betAmount.max = data.balance;
                    betAmount.min = data.current_bet + 1 || 1;
                }
            }

            // Обновляем баланс в состоянии победителя
            if (data.winner) {
                console.log("Winner state:", data.winner);
                gameEnded = true;
                const winnerName = typeof data.winner === "object" ? (data.winner.name || data.winner.player_id) : data.winner;
                currentWinner = winnerName;
                currentPlayer = null;
                if (data.players) {
                    data.players.forEach(player => {
                        playerBalances[player.name] = player.balance;
                        console.log(`Updated playerBalances[${player.name}] = ${playerBalances[player.name]} from winner state`);
                        if (player.hand) {
                            playerHands[player.name] = player.hand;
                        }
                    });
                    updatePlayerList(data.players, username, currentWinner);
                } else {
                    console.warn("No players data with winner message");
                }
                toggleButtons(false);
                document.getElementById("action-bar").style.display = "none";
                document.getElementById("ready-button").style.display = "block";
            }
        };

        ws.onclose = () => {
            console.log("WebSocket disconnected");
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

    // main.js (фрагмент функции updatePlayerList)

    function updatePlayerList(players, currentUsername, winnerName = null) {
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
        playerCircle.innerHTML = "";

        const containerWidth = gameContainer.offsetWidth;
        const containerHeight = gameContainer.offsetHeight;
        const aspectRatio = containerWidth / containerHeight;
        const radius = Math.min(containerWidth, containerHeight) * 0.35;

        const centerX = containerWidth / 2;
        const centerY = containerHeight / 2;

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

            const playerId = player.player_id || player.name;
            const balance = playerBalances[playerId] !== undefined ? playerBalances[playerId] : (player.balance || 0);
            console.log(`Displaying balance for ${playerId}: ${balance}`);
            const cardContainer = document.createElement("div");
            cardContainer.className = "card-container";

            if (player.is_dealer) {
                console.log(`Displaying Dealer label for ${playerId}`);
                const dealerLabel = document.createElement("div");
                dealerLabel.className = "dealer-label";
                dealerLabel.textContent = "Dealer";
                playerDiv.appendChild(dealerLabel);
            }

            if (player.is_small_blind) {
                const sbLabel = document.createElement("div");
                sbLabel.className = "blind-label";
                sbLabel.textContent = "SB: 10";
                playerDiv.appendChild(sbLabel);
            }
            if (player.is_big_blind) {
                const bbLabel = document.createElement("div");
                bbLabel.className = "blind-label";
                bbLabel.textContent = "BB: 20";
                playerDiv.appendChild(bbLabel);
            }

            if (winnerName && playerId === winnerName) {
                const winnerLabel = document.createElement("div");
                winnerLabel.className = "winner-label";
                winnerLabel.textContent = "Winner";
                playerDiv.appendChild(winnerLabel);
            }

            if (currentPlayer && playerId === currentPlayer && !winnerName) {
                const currentLabel = document.createElement("div");
                currentLabel.className = "current-label";
                currentLabel.textContent = "Current";
                playerDiv.appendChild(currentLabel);
            }

            if (player.name === currentUsername) {
                const hand = playerHands[player.name] || [];
                updatePlayerHand(hand, true, cardContainer);
            } else {
                if (winnerName && playerId === winnerName) {
                    const hand = player.hand && player.hand.length > 0 ? player.hand : (playerHands[playerId] || []);
                    updatePlayerHand(hand, true, cardContainer);
                } else {
                    for (let i = 0; i < 2; i++) {
                        const img = document.createElement("img");
                        img.src = "/static/images/card_back_black.png";
                        cardContainer.appendChild(img);
                    }
                }
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