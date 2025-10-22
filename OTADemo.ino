// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <WiFi.h>
#include <WebServer.h>  // 用於自訂網頁介面
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>

const char *ssid = "BasicOTA_AP";      // AP 的 SSID，請修改為您想要的名稱
const char *password = "12345678";     // AP 的密碼，請修改為您想要的密碼（至少 8 個字元）
uint32_t last_ota_time = 0;

WebServer server(80);  // HTTP 伺服器，用於自訂網頁

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  // 改為 AP 模式
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.print("AP Started. SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());  // AP 的 IP 通常為 192.168.4.1

  // 自訂初始網頁介面（美化版：改善 CSS 樣式、佈局與可讀性）
  server.on("/", []() {
    // 獲取 IP 並格式化為字串
    IPAddress apIP = WiFi.softAPIP();
    char ipStr[16];
    sprintf(ipStr, "%d.%d.%d.%d", apIP[0], apIP[1], apIP[2], apIP[3]);
    
    String ssidStr = String(ssid);  // 轉為 String 以便拼接
    String passwordStr = String(password);

    // 分段建構 HTML：靜態部分用 raw string，動態部分用 +=
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>BasicOTA Dashboard</title>
  <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;700&display=swap" rel="stylesheet">
  <style>
    :root {
      --primary-color: #2196F3; /* 藍色主調 */
      --secondary-color: #4CAF50; /* 綠色強調 */
      --background-color: #f5f7fa;
      --card-background: #ffffff;
      --text-color: #333333;
      --text-light: #666666;
      --border-color: #e0e0e0;
      --shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
      --border-radius: 12px;
    }
    body {
      font-family: 'Roboto', sans-serif;
      background: linear-gradient(135deg, var(--background-color) 0%, #e3f2fd 100%);
      margin: 0;
      padding: 20px;
      color: var(--text-color);
      line-height: 1.6;
      min-height: 100vh;
    }
    .container {
      max-width: 700px;
      margin: 0 auto;
      background: var(--card-background);
      padding: 30px;
      border-radius: var(--border-radius);
      box-shadow: var(--shadow);
      text-align: center;
    }
    h1 {
      color: var(--primary-color);
      font-weight: 700;
      margin-bottom: 10px;
      font-size: 2.2em;
    }
    .subtitle {
      color: var(--text-light);
      font-size: 1.1em;
      margin-bottom: 30px;
    }
    .info-card {
      background: linear-gradient(135deg, #e3f2fd 0%, #bbdefb 100%);
      padding: 20px;
      margin: 20px 0;
      border-radius: var(--border-radius);
      border-left: 5px solid var(--primary-color);
    }
    .info-card p {
      margin: 8px 0;
      font-size: 1.1em;
      font-weight: 400;
    }
    .status-indicator {
      display: inline-block;
      width: 12px;
      height: 12px;
      background: var(--secondary-color);
      border-radius: 50%;
      margin-right: 10px;
      animation: pulse 2s infinite;
    }
    @keyframes pulse {
      0% { opacity: 1; }
      50% { opacity: 0.5; }
      100% { opacity: 1; }
    }
    .instructions {
      background: #fff8e1;
      padding: 25px;
      margin: 25px 0;
      border-radius: var(--border-radius);
      text-align: left;
      border-left: 5px solid #ff9800;
    }
    .instructions h3 {
      color: #ff9800;
      margin-top: 0;
      font-size: 1.4em;
    }
    .instructions ol {
      padding-left: 20px;
      font-size: 1.05em;
    }
    .instructions li {
      margin-bottom: 12px;
      color: var(--text-color);
    }
    .button {
      display: inline-block;
      padding: 12px 24px;
      margin: 15px 10px;
      background: var(--secondary-color);
      color: white;
      text-decoration: none;
      border-radius: var(--border-radius);
      font-weight: 500;
      font-size: 1.1em;
      transition: background 0.3s ease, transform 0.2s ease;
      box-shadow: var(--shadow);
    }
    .button:hover {
      background: #45a049;
      transform: translateY(-2px);
    }
    .footer {
      margin-top: 40px;
      padding-top: 20px;
      border-top: 1px solid var(--border-color);
      color: var(--text-light);
      font-size: 0.9em;
    }
    @media (max-width: 600px) {
      .container { padding: 20px; }
      h1 { font-size: 1.8em; }
      .instructions { padding: 20px; }
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>BasicOTA Demo - AP Mode</h1>
    <p class="subtitle">歡迎使用無線 OTA 更新系統</p>
    <div class="info-card">
      <p><span class="status-indicator"></span><strong>裝置狀態：</strong>就緒</p>
      <p><strong>裝置 IP：</strong> )rawliteral";

    html += ipStr;  // 插入 IP
    html += R"rawliteral(</p>
      <p><strong>SSID：</strong> )rawliteral";

    html += ssidStr;  // 插入 SSID
    html += R"rawliteral(</p>
      <p><strong>連線密碼：</strong> )rawliteral";

    html += passwordStr;  // 插入密碼
    html += R"rawliteral(</p>
    </div>
    <div class="instructions">
      <h3>OTA 更新步驟</h3>
      <ol>
        <li>連線到此 AP (SSID: <strong>)rawliteral";

    html += ssidStr;  // 插入 SSID
    html += R"rawliteral(</strong>，密碼: <strong>)rawliteral";

    html += passwordStr;  // 插入密碼
    html += R"rawliteral(</strong>)</li>
        <li>在 Arduino IDE 中，選擇工具 > 連接埠 > <strong>)rawliteral";

    html += ipStr;  // 插入 IP
    html += R"rawliteral(</strong> (網路連接埠)</li>
        <li>上傳程式碼以進行 OTA 更新 (預設埠: 3232)</li>
      </ol>
    </div>
    <a href="https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/ota_updates.html" target="_blank" class="button">查看 OTA 文件</a>
    <div class="footer">
      <p>© 2024 Espressif Systems | 安全更新，輕鬆管理</p>
    </div>
  </div>
</body>
</html>
    )rawliteral";

    server.send(200, "text/html; charset=UTF-8", html);  // 指定 UTF-8 編碼
  });

  server.begin();  // 啟動 HTTP 伺服器
  Serial.println("HTTP server started");

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with plain text (will be hashed internally)
  // The authentication uses PBKDF2-HMAC-SHA256 with 10,000 iterations
  // ArduinoOTA.setPassword("admin");

  // Or set password with pre-hashed value (SHA256 hash of "admin")
  // SHA256(admin) = 8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918
  // ArduinoOTA.setPasswordHash("8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      if (millis() - last_ota_time > 500) {
        Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
        last_ota_time = millis();
      }
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Connect to the AP and visit http://" + WiFi.softAPIP().toString() + " for instructions.");
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();  // 處理 HTTP 請求
}