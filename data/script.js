// ==================== WEBSOCKET ====================
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function Send_Data(data) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send(data);
        console.log("📤 Gửi:", data);
    } else {
        console.warn("⚠️ WebSocket chưa sẵn sàng!");
        alert("⚠️ WebSocket chưa kết nối!");
    }
}

function onMessage(event) {
    console.log("📩 Nhận:", event.data);
    try {
        var data = JSON.parse(event.data);
        // Có thể thêm xử lý riêng nếu cần (ví dụ cập nhật trạng thái)
    } catch (e) {
        console.warn("Không phải JSON hợp lệ:", event.data);
    }
}


// ==================== UI NAVIGATION ====================
let relayList = [];
let deleteTarget = null;

function showSection(id, event) {
    document.querySelectorAll('.section').forEach(sec => sec.style.display = 'none');
    document.getElementById(id).style.display = id === 'settings' ? 'flex' : 'block';
    document.querySelectorAll('.nav-item').forEach(i => i.classList.remove('active'));
    event.currentTarget.classList.add('active');
}


// ==================== HOME GAUGES ====================
window.onload = function () {
    const gaugeTemp = new JustGage({
        id: "gauge_temp",
        value: 26,
        min: -10,
        max: 50,
        donut: true,
        pointer: false,
        gaugeWidthScale: 0.25,
        gaugeColor: "transparent",
        levelColorsGradient: true,
        levelColors: ["#00BCD4", "#4CAF50", "#FFC107", "#F44336"]
    });

    const gaugeHumi = new JustGage({
        id: "gauge_humi",
        value: 60,
        min: 0,
        max: 100,
        donut: true,
        pointer: false,
        gaugeWidthScale: 0.25,
        gaugeColor: "transparent",
        levelColorsGradient: true,
        levelColors: ["#42A5F5", "#00BCD4", "#0288D1"]
    });

    setInterval(() => {
        gaugeTemp.refresh(Math.floor(Math.random() * 15) + 20);
        gaugeHumi.refresh(Math.floor(Math.random() * 40) + 40);
    }, 3000);
};


// ==================== DEVICE FUNCTIONS ====================
function openAddRelayDialog() {
    document.getElementById('addRelayDialog').style.display = 'flex';
}
function closeAddRelayDialog() {
    document.getElementById('addRelayDialog').style.display = 'none';
}
function saveRelay() {
    const name = document.getElementById('relayName').value.trim();
    const gpio = document.getElementById('relayGPIO').value.trim();
    if (!name || !gpio) return alert("⚠️ Please fill all fields!");
    relayList.push({ id: Date.now(), name, gpio, state: false });
    renderRelays();
    closeAddRelayDialog();
}
function renderRelays() {
    const container = document.getElementById('relayContainer');
    container.innerHTML = "";
    relayList.forEach(r => {
        const card = document.createElement('div');
        card.className = 'device-card';
        card.innerHTML = `
      <i class="fa-solid fa-bolt device-icon"></i>
      <h3>${r.name}</h3>
      <p>GPIO: ${r.gpio}</p>
      <button class="toggle-btn ${r.state ? 'on' : ''}" onclick="toggleRelay(${r.id})">
        ${r.state ? 'ON' : 'OFF'}
      </button>
      <i class="fa-solid fa-trash delete-icon" onclick="showDeleteDialog(${r.id})"></i>
    `;
        container.appendChild(card);
    });
}
function toggleRelay(id) {
    const relay = relayList.find(r => r.id === id);
    if (relay) {
        relay.state = !relay.state;
        const relayJSON = JSON.stringify({
            page: "device",
            value: {
                name: relay.name,
                status: relay.state ? "ON" : "OFF",
                gpio: relay.gpio
            }
        });
        Send_Data(relayJSON);
        renderRelays();
    }
}
function showDeleteDialog(id) {
    deleteTarget = id;
    document.getElementById('confirmDeleteDialog').style.display = 'flex';
}
function closeConfirmDelete() {
    document.getElementById('confirmDeleteDialog').style.display = 'none';
}
function confirmDelete() {
    relayList = relayList.filter(r => r.id !== deleteTarget);
    renderRelays();
    closeConfirmDelete();
}


// ==================== SETTINGS FORM (BỔ SUNG) ====================
let statusCheckInterval = null; //new - interval for polling connection status

document.getElementById("settingsForm").addEventListener("submit", function (e) {
    e.preventDefault();

    const ssid = document.getElementById("ssid").value.trim();
    const password = document.getElementById("password").value.trim();
    const token = document.getElementById("token").value.trim();
    const server = document.getElementById("server").value.trim();
    const port = document.getElementById("port").value.trim();

    //new - send WiFi config via HTTP and start polling for connection status
    const xhr = new XMLHttpRequest();
    xhr.open("GET", `/connect?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(password)}`, true);
    xhr.onload = function() {
        if (xhr.status === 200) {
            alert("📡 Đang kết nối WiFi...");
            startStatusPolling(); //new - start checking connection status
        }
    };
    xhr.send();
});

//new - poll /status endpoint to check WiFi connection and auto-redirect
function startStatusPolling() {
    let attempts = 0;
    const maxAttempts = 30; // Poll for 30 seconds max
    
    statusCheckInterval = setInterval(function() {
        attempts++;
        
        fetch('/status') //new - check connection status from server
            .then(response => response.json())
            .then(data => {
                console.log("Status:", data);
                
                if (data.status === "connected" && data.ip) { //new - connection successful
                    clearInterval(statusCheckInterval);
                    alert(`✅ Kết nối thành công! Đang chuyển hướng đến ${data.ip}...`);
                    
                    //new - automatically redirect to new IP address
                    setTimeout(function() {
                        window.location.href = `http://${data.ip}`;
                    }, 2000);
                } else if (attempts >= maxAttempts) { //new - timeout handling
                    clearInterval(statusCheckInterval);
                    alert("⚠️ Không thể kết nối WiFi. Vui lòng thử lại.");
                }
            })
            .catch(err => {
                console.warn("Status check failed:", err);
                if (attempts >= maxAttempts) {
                    clearInterval(statusCheckInterval);
                    alert("⚠️ Không thể kết nối WiFi. Vui lòng thử lại.");
                }
            });
    }, 1000); // Check every second
}
