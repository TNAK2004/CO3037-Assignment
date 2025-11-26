#include "mainserver.h"
#include <WiFi.h>
#include <WebServer.h>

bool led1_state = false;
bool fan_state = false;
bool isAPMode = true;

WebServer server(80);

// String ssid = "";
// String password = "";
// String wifi_ssid = "";
// String wifi_password = "";

unsigned long connect_start_ms = 0;
bool connecting = false;

String mainPage()
{
  // float temperature = glob_temperature;
  // float humidity = glob_humidity;
  float temperature = 0;
  float humidity = 0;
  
  // if (xSemaphoreTake(Sema4need4Humi, portMAX_DELAY))
  // {
      if (xQueueReceive(humiQueue, &humidity , pdMS_TO_TICKS(500))) {  
          printf("[Main Page] Receive Humidity: %f - Free: %d\n", humidity, uxQueueSpacesAvailable(humiQueue));
      } 
  // }

  // if (xSemaphoreTake(Sema4need4Temp, portMAX_DELAY))
  // {
      if (xQueueReceive(tempQueue, &temperature , pdMS_TO_TICKS(500))) {  
          printf("[Main Page] Receive Temperature: %f - Free: %d\n", temperature, uxQueueSpacesAvailable(tempQueue));
      }
  // }

  String led1 = led1_state ? "ON" : "OFF";
  String fan = fan_state ? "ON" : "OFF";

  return R"rawliteral(
  <!DOCTYPE html>
  <html lang="vi">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dashboard</title>
    <style>
      body {
        font-family: "Segoe UI", Arial, sans-serif;
        background: #fff6e6;
        color: #333;
        text-align: center;
        margin: 0;
        height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
      }

      .logo {
        width: 220px;
        height: 90px;
        border-radius: 20px;
        box-shadow: 0 4px 10px rgba(0,0,0,0.2);
        margin-bottom: 25px;
        background: #f2f3f5;
        object-fit: contain;
        padding: 10px 15px;
      }

      .welcome {
        font-size: 2.2em;
        font-weight: bold;
        margin-bottom: 30px;
        padding: 15px 30px;
        border: 3px solid #ff9800;
        border-radius: 15px;
        background-color: #fff;
        box-shadow: 0 6px 20px rgba(255, 152, 0, 0.3);
        text-shadow: 1px 1px 2px rgba(0,0,0,0.1);
        color: #ff6f00;
      }

      .container {
        background: linear-gradient(135deg, #ff9800, #ffd54f);
        padding: 35px 45px;
        border-radius: 20px;
        box-shadow: 0 4px 25px rgba(0,0,0,0.15);
        width: 90%;
        max-width: 400px;
        color: #fff;
        backdrop-filter: blur(6px);
        display: flex; 
        flex-direction: column; 
        gap: 20px; 
      }

      h1 {
        font-size: 1.8em;
        margin-top: 0;
        margin-bottom: 0px; 
      }

      .box-title {
          font-size: 1.4em;
          font-weight: bold;
          margin-bottom: 10px;
          border-bottom: 2px solid #fff200;
          padding-bottom: 5px;
          color: #fff200;
      }
      
      .monitoring-box, .control-box {
          background: rgba(255, 255, 255, 0.2);
          padding: 15px;
          border-radius: 15px;
          box-shadow: inset 0 0 10px rgba(0,0,0,0.1);
      }

      .sensor {
        font-size: 1.1em;
        margin: 10px 0;
      }

      .sensor span {
        font-weight: bold;
        color: #fff200;
      }

      button {
        margin: 10px;
        background: #fff200;;
        color: #000;
        font-weight: bold;
        border: none;
        border-radius: 20px;
        padding: 10px 20px;
        cursor: pointer;
        transition: all 0.3s;
        font-size: 1em;
      }

      button:hover {
        background: #ffe300;
        transform: scale(1.05);
      }

      #settings {
        background: #fff6e6;
        color: #ff8c00;
        font-weight: bold;
        margin-top: 15px; 
      }

      #settings:hover {
        background: #ffe4c4;
      }
    </style>
  </head>

  <body>
    <div class="welcome">WELCOME :3 </div>
    <div class="container">
      <h1>NAAK's Dashboard</h1>
      
      <div class="monitoring-box">
          <div class="box-title">Monitoring</div>
          <div class="sensor">
            🌡️ Nhiệt độ: <span id="temp">)rawliteral" +
              String(temperature) + R"rawliteral(</span> &deg;C
          </div>
          <div class="sensor">
            💧 Độ ẩm: <span id="hum">)rawliteral" +
              String(humidity) + R"rawliteral(</span> %
          </div>
      </div>
      
      <div class="control-box">
          <div class="box-title">Control LED</div>
          <div>
            <button onclick='toggleLED(1)'>💡 LED1: <span id="l1">)rawliteral" +
              led1 + R"rawliteral(</span></button>
            <button onclick='toggleLED(2)'>💡 FAN: <span id="l2">)rawliteral" +
              fan + R"rawliteral(</span></button>
          </div>
      </div>

      <button id="settings" onclick="window.location='/settings'">⚙️ Cài đặt</button>
    </div>

    <script>
      function toggleLED(id) {
        fetch('/toggle?led='+id)
          .then(response=>response.json())
          .then(json=>{
            document.getElementById('l1').innerText=json.led1;
            document.getElementById('l2').innerText=json.fan;
          });
      }

      setInterval(()=>{
        fetch('/sensors')
          .then(res=>res.json())
          .then(d=>{
            document.getElementById('temp').innerText=d.temp;
            document.getElementById('hum').innerText=d.hum;
          });
      },3000);
    </script>
  </body>
  </html>
  )rawliteral";
}

String settingsPage()
{
  return R"rawliteral(
  <!DOCTYPE html>
  <html lang="vi">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wi-Fi Settings</title>
    <style>
      
      body {
        font-family: "Segoe UI", Arial, sans-serif;
        background: #fff6e6; 
        color: #333;
        text-align: center;
        margin: 0;
        height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
      }

      

      
      .container {
        background: linear-gradient(135deg, #ff9800, #ffd54f);
        padding: 35px 45px;
        border-radius: 20px;
        box-shadow: 0 4px 25px rgba(0,0,0,0.15);
        width: 90%;
        max-width: 400px;
        color: #fff; 
        backdrop-filter: blur(6px);
      }

      h1 {
        font-size: 1.8em;
        margin-bottom: 25px; 
      }

      input[type=text], input[type=password] {
        width: 100%;
        padding: 10px;
        border: none;
        border-radius: 10px;
        font-size: 1em;
        box-sizing: border-box;
        margin-bottom: 15px;
        outline: none;
        color: #333; 
      }

      
      button {
        margin-top: 10px;
        background: #fff200; 
        color: #000;
        font-weight: bold;
        border: none;
        border-radius: 25px;
        padding: 10px 20px;
        cursor: pointer;
        transition: all 0.3s;
        font-size: 1em;
        box-shadow: 0 2px 5px rgba(0,0,0,0.2); 
      }

      button:hover {
        background: #ffe300;
        transform: scale(1.05);
      }

      
      #back {
        background: #fff6e6;
        color: #ff8c00; 
        font-weight: bold;
        margin-left: 15px; 
      }

      #back:hover {
        background: #ffe4c4;
      }

      #msg {
        margin-top: 15px; 
        font-weight: 500;
        color: #fff200; 
      }
    </style>
  </head>

  <body>
    <div class="container">
      <h1>⚙️ Cấu hình Wi-Fi</h1>
      <form id="wifiForm">
        <input name="ssid" id="ssid" type="text" placeholder="Tên Wi-Fi (SSID)" required><br>
        <input name="password" id="pass" type="password" placeholder="Mật khẩu (bỏ trống nếu không có)"><br><br>
        <button type="submit">Kết nối</button>
        <button type="button" id="back" onclick="window.location='/'">Quay lại</button>
      </form>
      <div id="msg"></div>
    </div>

    <script>
      document.getElementById('wifiForm').onsubmit = function(e){
        e.preventDefault();
        let ssid = document.getElementById('ssid').value;
        let pass = document.getElementById('pass').value;
        fetch('/connect?ssid='+encodeURIComponent(ssid)+'&pass='+encodeURIComponent(pass))
          .then(r=>r.text())
          .then(msg=>{
            document.getElementById('msg').innerText = msg;
          });
      };
    </script>
  </body>
  </html>
  )rawliteral";
}

// ========== Handlers ==========
void handleRoot() { server.send(200, "text/html", mainPage()); }

void handleToggle()
{
  int led = server.arg("led").toInt();
  if (led == 1)
  {
    led1_state = !led1_state;
    Serial.println("YOUR CODE TO CONTROL LED1");
    digitalWrite(LED1_PIN, led1_state ? HIGH : LOW);
    Serial.printf("LED1 %s\n", led1_state ? "ON" : "OFF");
  }
  else if (led == 2)
  {
    fan_state = !fan_state;
    Serial.println("YOUR CODE TO CONTROL FAN");
    digitalWrite(FAN_PIN, fan_state ? HIGH : LOW);
    Serial.printf("FAN %s\n", fan_state ? "ON" : "OFF");
  }
  server.send(200, "application/json",
              "{\"led1\":\"" + String(led1_state ? "ON" : "OFF") +
                  "\",\"fan\":\"" + String(fan_state ? "ON" : "OFF") + "\"}");
}

void handleSensors()
{
  float t = 0;
  float h = 0;

  // if (xSemaphoreTake(Sema4need4LedBlinky_Temp, portMAX_DELAY) == pdTRUE)
  // {
    if (xQueueReceive(tempQueue, &t, pdMS_TO_TICKS(500))==pdPASS){
      printf("[Server] Receive Temperature: %f - Free: %d\n", t, uxQueueSpacesAvailable(tempQueue));
    }
    else{
      printf("[Server] No Temperature received\n");
    }
  //}

  // if (xSemaphoreTake(Sema4need4NeoBlinky_Humi, portMAX_DELAY) == pdTRUE)
  //{
    if (xQueueReceive(humiQueue, &h, pdMS_TO_TICKS(500))==pdPASS){
      printf("[Server] Receive Humidity: %f - Free: %d\n", h, uxQueueSpacesAvailable(humiQueue));
    }
    else{
      printf("[Server] No Humidity received\n");
    }
  //}

  String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
  server.send(200, "application/json", json);
}

void handleSettings() { server.send(200, "text/html", settingsPage()); }

void handleConnect()
{
  wifi_ssid = server.arg("ssid");
  wifi_password = server.arg("pass");
  server.send(200, "text/plain", "Connecting....");
  isAPMode = false;
  connecting = true;
  connect_start_ms = millis();
  connectToWiFi();
}

// ========== WiFi ==========
void setupServer()
{
  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/sensors", HTTP_GET, handleSensors);
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/connect", HTTP_GET, handleConnect);
  server.begin();
}

void startAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  isAPMode = true;
  connecting = false;
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  if (wifi_password.isEmpty())
  {
    WiFi.begin(wifi_ssid.c_str());
  }
  else
  {
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  }
  Serial.print("Connecting to: ");
  Serial.print(wifi_ssid.c_str());

  Serial.print(" Password: ");
  Serial.print(wifi_password.c_str());
}

// ========== Main task ==========
void main_server_task(void *pvParameters)
{
  pinMode(BOOT_PIN, INPUT_PULLUP);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);

  startAP();
  setupServer();

  while (1)
  {
    server.handleClient();

    // BOOT Button to switch to AP Mode
    if (digitalRead(BOOT_PIN) == LOW)
    {
      vTaskDelay(100);
      if (digitalRead(BOOT_PIN) == LOW)
      {
        if (!isAPMode)
        {
          startAP();
          setupServer();
        }
      }
    }

    // STA Mode
    if (connecting)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print(" STA IP address: ");
        Serial.println(WiFi.localIP());
        isWifiConnected = true; // Internet access

        xSemaphoreGive(xBinarySemaphoreInternet);

        isAPMode = false;
        connecting = false;
      }
      else if (millis() - connect_start_ms > 10000)
      { // timeout 10s
        Serial.println("WiFi connect failed! Back to AP.");
        startAP();
        setupServer();
        connecting = false;
        isWifiConnected = false;
      }
    }

    vTaskDelay(20); // avoid watchdog reset
  }
}