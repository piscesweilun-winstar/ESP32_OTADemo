/*
  -----------------------
  ElegantOTA - Demo Example (AP Mode with Custom Web Interface)

  Modified by Grok: https://grok.com/c/d5edab44-2515-45d7-b6dc-4b78db43a0a1
  -----------------------

  Skill Level: Beginner

  This example provides a bare minimal app with ElegantOTA functionality
  in Access Point (AP) mode, including a custom web interface at the root URL.

  Github: https://github.com/ayushsharma82/ElegantOTA
  WiKi: https://docs.elegantota.pro

  Works with following hardware:
  - ESP8266
  - ESP32
  - RP2040 (with WiFi) (Example: Raspberry Pi Pico W)


  Important note for RP2040 users:
  - RP2040 requires LittleFS partition for the OTA updates to work. Without LittleFS partition, OTA updates will fail.
    Make sure to select Tools > Flash Size > "2MB (Sketch 1MB, FS 1MB)" option.
  - If using bare RP2040, it requires WiFi module like Pico W for ElegantOTA to work.

  -------------------------------

  Upgrade to ElegantOTA Pro: https://elegantota.pro

  注意：請修改 ssid 和 password 變數來設定您的 AP 名稱和密碼。
  Custom Interface: Visit the root URL (e.g., http://192.168.4.1) for a simple dashboard with OTA link.
*/


#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#elif defined(TARGET_RP2040) || defined(TARGET_RP2350) || defined(PICO_RP2040) || defined(PICO_RP2350)
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>
#endif

#include <ElegantOTA.h>

const char* ssid = "ElegantOTA_AP";  // AP 的 SSID，請修改為您想要的名稱
const char* password = "12345678";   // AP 的密碼，請修改為您想要的密碼（至少 8 個字元）

#if defined(ESP8266)
ESP8266WebServer server(80);
#elif defined(ESP32)
WebServer server(80);
#elif defined(TARGET_RP2040) || defined(TARGET_RP2350) || defined(PICO_RP2040) || defined(PICO_RP2350)
WebServer server(80);
#endif

unsigned long ota_progress_millis = 0;

void onOTAStart() {
    // Log when OTA has started
    Serial.println("OTA update started!");
    // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
    // Log every 1 second
    if (millis() - ota_progress_millis > 1000) {
        ota_progress_millis = millis();
        Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
    }
}

void onOTAEnd(bool success) {
    // Log when OTA has finished
    if (success) {
        Serial.println("OTA update finished successfully!");
    } else {
        Serial.println("There was an error during OTA update!");
    }
    // <Add your own code here>
}

void setup(void) {
    Serial.begin(115200);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.println("");

    Serial.print("AP Started. SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());  // AP 的 IP 通常為 192.168.4.1

    // Custom web interface at root "/"
    server.on("/", []() {
        String html = R"(
<!DOCTYPE html>
<html>
<head>
  <title>ElegantOTA Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin: 50px; background-color: #f0f0f0; }
    .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
    h1 { color: #333; }
    .info { background: #e7f3ff; padding: 10px; margin: 10px 0; border-radius: 5px; }
    .button { display: inline-block; padding: 10px 20px; margin: 10px; background: #4CAF50; color: white; text-decoration: none; border-radius: 5px; }
    .button:hover { background: #45a049; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ElegantOTA Demo - AP Mode</h1>
    <div class="info">
      <p><strong>Device IP:</strong> )"
                      + WiFi.softAPIP().toString() + R"(</p>
      <p><strong>SSID:</strong> )"
                      + String(ssid) + R"(</p>
      <p><strong>Status:</strong> Ready for OTA Updates</p>
    </div>
    <a href="/update" class="button">Start OTA Update</a>
    <p>Connect your device to this AP and use the button above to update firmware.</p>
  </div>
</body>
</html>
    )";
        server.send(200, "text/html", html);
    });

    ElegantOTA.begin(&server);  // Start ElegantOTA
    // ElegantOTA callbacks
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    server.begin();
    Serial.println("HTTP server started");
    Serial.println("Connect to the AP and visit http://" + WiFi.softAPIP().toString() + " for the custom dashboard.");
}

void loop(void) {
    server.handleClient();
    ElegantOTA.loop();
}
