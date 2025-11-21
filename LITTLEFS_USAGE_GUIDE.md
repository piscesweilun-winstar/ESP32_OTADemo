# LittleFS 快速參考指南

## 常用 API

### 初始化
```cpp
#include <LittleFS.h>

// 初始化（自動格式化如果需要）
if (!LittleFS.begin(true)) {
    Serial.println("LittleFS 初始化失敗");
}
```

### 文件操作

#### 寫入文件
```cpp
File file = LittleFS.open("/config.txt", FILE_WRITE);
if (file) {
    file.println("Hello LittleFS");
    file.close();
}
```

#### 讀取文件
```cpp
File file = LittleFS.open("/config.txt", FILE_READ);
if (file) {
    String content = file.readStringUntil('\n');
    Serial.println(content);
    file.close();
}
```

#### 刪除文件
```cpp
LittleFS.remove("/config.txt");
```

#### 檢查文件是否存在
```cpp
if (LittleFS.exists("/config.txt")) {
    Serial.println("文件存在");
}
```

### 目錄操作

#### 列出目錄
```cpp
File root = LittleFS.open("/");
File file = root.openNextFile();
while (file) {
    Serial.print("FileName: ");
    Serial.println(file.name());
    file = root.openNextFile();
}
root.close();
```

#### 建立目錄
```cpp
LittleFS.mkdir("/data");
```

### 存儲空間管理

#### 獲取空間信息
```cpp
size_t totalBytes = LittleFS.totalBytes();
size_t usedBytes = LittleFS.usedBytes();
size_t freeBytes = totalBytes - usedBytes;

Serial.printf("Total: %d, Used: %d, Free: %d\n", 
    totalBytes, usedBytes, freeBytes);
```

#### 格式化檔案系統
```cpp
LittleFS.format();
```

## 實用示例

### 示例 1：讀寫 JSON 配置
```cpp
#include <ArduinoJson.h>

void saveConfig() {
    File file = LittleFS.open("/config.json", FILE_WRITE);
    if (file) {
        StaticJsonDocument<200> doc;
        doc["ssid"] = "MySSID";
        doc["password"] = "MyPassword";
        serializeJson(doc, file);
        file.close();
    }
}

void loadConfig() {
    File file = LittleFS.open("/config.json", FILE_READ);
    if (file) {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, file);
        String ssid = doc["ssid"];
        String password = doc["password"];
        file.close();
    }
}
```

### 示例 2：存儲傳感器數據
```cpp
void logSensorData(float temperature, float humidity) {
    File file = LittleFS.open("/data.csv", FILE_APPEND);
    if (file) {
        file.printf("%lu,%f,%f\n", 
            millis(), temperature, humidity);
        file.close();
    }
}
```

### 示例 3：Web 伺服器提供靜態文件
```cpp
void serveStaticFile(String path) {
    if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, FILE_READ);
        server.sendHeader("Content-Disposition", 
            "attachment; filename=" + path);
        server.streamFile(file, "text/plain");
        file.close();
    } else {
        server.send(404, "text/plain", "File Not Found");
    }
}
```

## OTA 檔案系統更新步驟

### 使用 PlatformIO
1. 在項目根目錄建立 `data/` 資料夾
2. 將文件放入 `data/` 資料夾
3. 執行命令：
```bash
pio run --target uploadfs --upload-port 192.168.4.1
```

### 使用 Arduino IDE（ESP32 SPIFFS Data Upload）
1. 在 Arduino IDE 中安裝 "ESP32 Sketch Data Upload" 插件
2. 建立 `data` 資料夾在 Sketch 目錄中
3. 將文件放入 `data/` 資料夾
4. 選擇 Tools > ESP32 Sketch Data Upload
5. 選擇 Network Port 並確認

## 常見問題

### Q: LittleFS 和 SPIFFS 有什麼區別？
A: LittleFS 是新一代檔案系統，性能更好、更可靠。SPIFFS 已逐漸被淘汰。

### Q: 如何在 OTA 更新期間保護檔案？
A: OTA 在更新時會自動卸載檔案系統，完成後自動重新掛載。

### Q: 可以同時更新 Sketch 和 LittleFS 嗎？
A: 不可以。需要分別進行：先更新 Sketch，再更新 LittleFS。

### Q: 檔案系統滿了怎麼辦？
A: 刪除不需要的文件，或使用 `LittleFS.format()` 完全清空。

### Q: 如何在程式中檢測 LittleFS 更新完成？
A: 在 `onEnd` 回調中可以檢測更新完成，本項目會自動重新掛載。

## 性能提示

1. **避免頻繁讀寫**：實現緩衝機制
2. **大文件處理**：使用流式讀寫而不是全部載入記憶體
3. **文件系統塊大小**：預設為 4096 字節
4. **磨損均衡**：LittleFS 自動處理

## 安全建議

1. 在更新檔案系統前進行備份
2. 測試新檔案後再正式上傳
3. 預留足夠的自由空間（至少 10%）
4. 定期監控檔案系統的完整性

---

更多信息請參考 [ESP32 官方文檔](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
