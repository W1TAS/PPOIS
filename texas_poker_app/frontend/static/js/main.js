document.addEventListener("DOMContentLoaded", () => {
    console.log("DOM fully loaded");

    let ws;
    let token = null;
    let username = null;

    function showGame() {
        document.getElementById("auth").style.display = "none";
        document.getElementById("game-table").style.display = "block";
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
                updatePlayerList(data.players);
                document.getElementById("ready-button").style.display = "block";
                document.getElementById("actions").style.display = "none";
                document.getElementById("stage").textContent = "Waiting for players...";
                document.getElementById("player-hand").textContent = "";
                document.getElementById("community-cards").textContent = "";
                document.getElementById("pot").textContent = "";
                document.getElementById("balance").textContent = "";
                document.getElementById("current-player").textContent = "";
                document.getElementById("winner").style.display = "none";
            }

            if (data.stage) {
                console.log("Game state");
                document.getElementById("stage").textContent = `Stage: ${data.stage}`;
                document.getElementById("player-hand").textContent = `Your hand: ${data.hand.join(", ")}`;
                document.getElementById("community-cards").textContent = `Community: ${data.community_cards.join(", ")}`;
                document.getElementById("pot").textContent = `Pot: ${data.pot}`;
                document.getElementById("balance").textContent = `Balance: ${data.balance} (Bet: ${data.current_bet})`;
                document.getElementById("current-player").textContent = `Current Player: ${data.current_player}`;
                updatePlayerList(data.players);
                document.getElementById("ready-button").style.display = "none";
                document.getElementById("actions").style.display = "block";
                const isActivePlayer = data.current_player === username && !data.players.find(p => p.name === username).folded;
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
                document.getElementById("actions").style.display = "none";
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

    function updatePlayerList(players) {
        const playerList = document.getElementById("player-list");
        playerList.innerHTML = "";
        players.forEach(player => {
            const li = document.createElement("li");
            if (player.ready !== undefined) {
                li.textContent = `${player.player_id}: ${player.ready ? "Ready" : "Not Ready"}`;
            } else {
                li.textContent = `${player.name}: ${player.folded ? "Folded" : "Active"} (Bet: ${player.bet})`;
            }
            playerList.appendChild(li);
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