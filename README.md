# ESP32 BasicOTA AP Mode with Custom Dashboard

[![ESP32](https://img.shields.io/badge/ESP32-Compatible-brightgreen)](https://www.espressif.com/en/products/socs/esp32)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-orange)](https://www.arduino.cc/en/software)

## 專案描述

此專案基於 Espressif 官方的 **BasicOTA** 範例，修改為 **Access Point (AP) 模式**，讓 ESP32 裝置獨立建立 WiFi 熱點，無需連線外部路由器即可進行 **無線 OTA (Over-The-Air) 更新**。同時，新增一個 **美化自訂網頁介面**（Dashboard），提供直觀的裝置資訊、連線指示，以及 OTA 更新步驟說明。

- **核心功能**：使用 ArduinoOTA 庫進行韌體更新，支援 Sketch 和 Filesystem 更新。
- **AP 模式**：裝置自動建立 WiFi 熱點，IP 預設為 `192.168.4.1`。
- **網頁介面**：現代化 UI，使用 CSS 漸層、動畫和響應式設計，包含裝置狀態、SSID/密碼顯示，以及詳細 OTA 步驟。

適合初學者或嵌入式開發者快速測試 OTA 功能，無需複雜的網路設定。

## 功能特點

- **AP 模式 WiFi**：一鍵建立熱點，連線後即時存取 OTA。
- **美化 Dashboard**：包含狀態指示燈、漸層卡片、互動按鈕，以及 Espressif 官方文件連結。
- **動態內容**：網頁自動顯示裝置 IP、SSID 和密碼。
- **OTA 回調**：進度追蹤、錯誤處理（無認證，預設埠 3232）。
- **響應式設計**：支援手機/桌面瀏覽器。
- **UTF-8 相容**：正確顯示中文，避免亂碼。

## 硬體需求

- **ESP32 開發板**：如 ESP32 DevKitC、NodeMCU-32S 等（非 ESP8266）。
- **USB 轉串口**：用於初次燒錄（後續可 OTA）。
- **電腦**：安裝 Arduino IDE。

## 軟體需求

- **Arduino IDE**：版本 2.0+，安裝 ESP32 核心（Boards Manager > "esp32" by Espressif）。
  - 燒錄設定：Flash Size 至少 4MB；Partition Scheme 預設。
- **庫依賴**（自動安裝）：
  - WiFi.h
  - WebServer.h
  - ESPmDNS.h
  - NetworkUdp.h
  - ArduinoOTA.h

## 安裝與設定

### 1. 準備 Arduino IDE
1. 開啟 Arduino IDE。
2. 在 **File > Preferences** 中，新增 ESP32 核心 URL：`https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`。
3. 在 **Tools > Board > Boards Manager** 搜尋 "esp32" 並安裝。
4. 選擇板子：**Tools > Board > ESP32 Arduino > ESP32 Dev Module**。
5. 設定連接埠：**Tools > Port**（您的 USB 埠）。

### 2. 修改程式碼
- 開啟 `BasicOTA_AP.ino`（或複製程式碼到新檔案）。
- 修改以下變數：
  ```c
  const char *ssid = "YourAPName";      // 自訂 AP 名稱
  const char *password = "YourPassword"; // 自訂密碼（至少 8 字元）
  ```
- 若需 OTA 認證，取消註解並設定：
  ```c
  ArduinoOTA.setPassword("admin");  // 或使用預哈希值
  ```

### 3. 初次燒錄
1. 連線 ESP32 至電腦。
2. 點擊 **Upload** 燒錄程式。
3. 開啟 **Serial Monitor**（115200 baud），確認 "AP Started" 和 IP 顯示。

## 使用方法

### 1. 啟動裝置
- 燒錄後，ESP32 自動建立 AP 熱點（預設 SSID: "BasicOTA_AP"，密碼: "12345678"）。
- Serial Monitor 顯示：`IP address: 192.168.4.1`。

### 2. 連線並存取 Dashboard
1. 在手機/電腦 WiFi 設定中，連線至 AP（SSID 和密碼如上）。
2. 開啟瀏覽器，輸入 `http://192.168.4.1`（或 Serial 顯示的 IP）。
3. 您將看到美化 Dashboard：
   - **裝置資訊**：IP、SSID、密碼、狀態（綠色脈動燈表示就緒）。
   - **OTA 步驟**：詳細中文指示，包括 Arduino IDE 設定。
   - **額外連結**：按鈕連結 Espressif OTA 文件。

### 3. 進行 OTA 更新
1. 在 Arduino IDE：
   - 選擇 **Tools > Port > Network Ports > 192.168.4.1 at 3232**（或顯示的 IP）。
2. 編譯新程式碼，點擊 **Upload**。
3. Serial Monitor 顯示進度（如 "Progress: 50%"）。
4. 更新完成後，裝置自動重啟。

**注意**：
- OTA 僅在 AP 連線狀態下有效。
- 若斷線，重新連線 AP 並刷新端口列表。

## 截圖

### Dashboard 範例
（想像一個現代 UI：藍綠漸層背景、卡片式資訊區、橙色指示區塊）

- **桌面版**：寬版佈局，清晰標題。
- **手機版**：自動調整，易讀步驟。

## 疑難排解

- **網頁顯示亂碼**：
  - 確保 Arduino IDE 檔案編碼為 UTF-8（File > Save As > Encoding: UTF-8）。
  - 瀏覽器設定：Chrome > 設定 > 語言 > 新增中文（繁體）。
- **IP 顯示錯誤**：
  - 檢查 Serial：若為 "0.0.0.0"，重啟裝置或確認 WiFi.softAP() 成功。
- **OTA 連線失敗**：
  - 確認端口選擇正確（網路端口，非 USB）。
  - 若有防火牆，允許 3232 埠。
  - 錯誤碼：參考 Serial 輸出（e.g., OTA_AUTH_ERROR 表示需密碼）。
- **記憶體不足**：使用 ESP32-S3 等較大 RAM 板子。

若問題持續，請檢查 Serial 輸出並在 [Espressif 論壇](https://www.esp32.com/) 求助。

## 授權

本專案基於 Espressif 官方範例，採用 **Apache License 2.0**。詳見 [LICENSE](LICENSE) 或原始碼註解。

## 貢獻與聯絡

- **原始碼**：基於 [Espressif ArduinoOTA](https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA)。
- **改版者**：Grok (xAI) 輔助開發。
- 歡迎 Pull Request 或 Issue！如有建議，聯絡 [xAI](https://x.ai)。

---

*最後更新：2025 年 10 月 22 日*  
感謝 Espressif 提供優秀的 ESP32 生態！ 🚀