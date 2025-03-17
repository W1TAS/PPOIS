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
    let playerBalances = {}; // Объект для хранения баланса игроков

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

        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            console.log("Received WebSocket message:", data);

            if (data.players && !data.stage) {
                console.log("Pre-game state");
                // Сохраняем баланс игроков
                data.players.forEach(player => {
                    playerBalances[player.player_id] = player.balance || 0;
                });
                updatePlayerList(data.players, username);
                document.getElementById("ready-button").style.display = "block";
                document.getElementById("action-bar").style.display = "none";
                document.getElementById("table-center").style.display = "none";
                document.getElementById("player-info").style.display = "none";
                document.getElementById("player-hand").textContent = "";
                document.getElementById("community-cards").textContent = "";
                document.getElementById("pot").textContent = "";
                document.getElementById("balance").textContent = "";
                document.getElementById("current-player").textContent = "";
                document.getElementById("winner").style.display = "none";
            }

            if (data.stage) {
                console.log("Game state");
                document.getElementById("table-center").style.display = "block";
                document.getElementById("player-info").style.display = "flex";
                document.getElementById("stage").textContent = `Stage: ${data.stage}`;
                document.getElementById("player-hand").textContent = `Your hand: ${data.hand.join(", ")}`;
                document.getElementById("community-cards").textContent = `Community: ${data.community_cards.join(", ")}`;

                if (data.pots) {
                    const total_pot = data.pots.reduce((sum, pot) => sum + pot.amount, 0);
                    document.getElementById("pot").textContent = `Pot: ${total_pot} (${data.pots.map(p => p.amount).join(" | ")})`;
                } else {
                    document.getElementById("pot").textContent = "Pot: 0";
                }

                // Обновляем баланс текущего игрока
                playerBalances[username] = data.balance || 0;
                // Обновляем баланс остальных игроков, если сервер передал их
                if (data.players) {
                    data.players.forEach(player => {
                        if (player.balance !== undefined) {
                            playerBalances[player.name] = player.balance;
                        }
                    });
                }

                document.getElementById("balance").innerHTML = `${username} <span>${playerBalances[username]}</span>`; // Имя и баланс
                document.getElementById("current-player").textContent = `Current: ${data.current_player}`;
                updatePlayerList(data.players, username);
                document.getElementById("ready-button").style.display = "none";
                const isActivePlayer = data.current_player === username && !data.players.find(p => p.name === username)?.folded;
                document.getElementById("action-bar").style.display = isActivePlayer ? "flex" : "none";
                toggleButtons(isActivePlayer);
                const betAmount = document.getElementById("bet-amount");
                if (betAmount) {
                    betAmount.max = data.balance;
                    betAmount.min = data.current_bet + 1 || 1;
                }
            }

            if (data.winner) {
                console.log("Winner state");
                document.getElementById("winner").style.display = "block";
                document.getElementById("winner").textContent = `Winner: ${data.winner}`;
                toggleButtons(false);
                document.getElementById("action-bar").style.display = "none";
                document.getElementById("ready-button").style.display = "block";
            }

            if (data.error) {
                alert(data.error);
            }
        };

        ws.onclose = () => {
            console.log("WebSocket disconnected");
        };
    }

    function updatePlayerList(players, currentUsername) {
        const playerCircle = document.getElementById("player-circle");
        if (!playerCircle) {
            console.error("Element #player-circle not found in DOM");
            return;
        }
        playerCircle.innerHTML = "";

        const containerWidth = playerCircle.offsetWidth;
        const containerHeight = playerCircle.offsetHeight;
        const aspectRatio = containerWidth / containerHeight; // Возвращаем aspectRatio
        const radius = Math.min(containerWidth, containerHeight) * 0.3;

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
            const angleDeg = 270 - (360 / totalPlayers) * index;
            const angleRad = (angleDeg * Math.PI) / 180;
            const xOffset = radius * Math.cos(angleRad) * aspectRatio;
            const yOffset = radius * Math.sin(angleRad);

            const xPercent = ((centerX + xOffset) / containerWidth) * 100;
            const yPercent = ((centerY - yOffset) / containerHeight) * 100;

            const playerDiv = document.createElement("div");
            playerDiv.className = "player";
            playerDiv.style.left = `${xPercent}%`;
            playerDiv.style.top = `${yPercent}%`;

            const playerId = player.player_id || player.name;
            const balance = playerBalances[playerId] || 0; // Используем сохранённый баланс
            if (player.ready !== undefined) {
                playerDiv.innerHTML = `${playerId}<br>${player.ready ? "Ready" : "Not Ready"}<br><span class="balance">${balance}</span>`;
            } else {
                playerDiv.innerHTML = `${player.name}<br><span class="balance">${balance}</span>${player.bet ? `<br>Bet: ${player.bet}` : ""}${player.folded ? "<br>Folded" : ""}`;
            }
            playerCircle.appendChild(playerDiv);
        });
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