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

  // 自訂初始網頁介面（修復字串拼接：分段建構 HTML 以插入動態變數）
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
<html>
<head>
  <meta charset="UTF-8">
  <title>BasicOTA Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin: 50px; background-color: #f0f0f0; }
    .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
    h1 { color: #333; }
    .info { background: #e7f3ff; padding: 10px; margin: 10px 0; border-radius: 5px; }
    .button { display: inline-block; padding: 10px 20px; margin: 10px; background: #4CAF50; color: white; text-decoration: none; border-radius: 5px; }
    .button:hover { background: #45a049; }
    .instructions { background: #fff3cd; padding: 15px; margin: 20px 0; border-radius: 5px; text-align: left; }
  </style>
</head>
<body>
  <div class="container">
    <h1>BasicOTA Demo - AP Mode</h1>
    <div class="info">
      <p><strong>Device IP:</strong> )rawliteral";

    html += ipStr;  // 插入 IP
    html += R"rawliteral(</p>
      <p><strong>SSID:</strong> )rawliteral";

    html += ssidStr;  // 插入 SSID
    html += R"rawliteral(</p>
      <p><strong>Status:</strong> Ready for OTA Updates</p>
    </div>
    <div class="instructions">
      <h3>OTA 更新指示：</h3>
      <ol>
        <li>連線到此 AP (SSID: )rawliteral";

    html += ssidStr;  // 插入 SSID
    html += R"rawliteral(，密碼: )rawliteral";

    html += passwordStr;  // 插入密碼
    html += R"rawliteral() )</li>
        <li>在 Arduino IDE 中，選擇工具 > 連接埠 > )rawliteral";

    html += ipStr;  // 插入 IP
    html += R"rawliteral( (網路連接埠)</li>
        <li>上傳程式碼以進行 OTA 更新 (預設埠: 3232)</li>
      </ol>
    </div>
    <p>使用 Arduino IDE 連線到此 IP 進行無線更新！</p>
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