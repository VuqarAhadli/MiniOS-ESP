#include "config.h"
#include "display.h"
#include "kernel.h"
#include "theme.h"
#include <Arduino.h>
#if !defined(DEVICE_RP2350)
#include <SPIFFS.h>
#endif
#include <string>


const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET = 4 * 3600;   
const int DAYLIGHT_OFFSET = 0;
const char* OS_VERSION = "MiniOS-ESP v2.1.4";

static std::string deviceName = "Mini";
static int savedTheme = 0;
static int savedWallpaper = 0;
#if defined(DEVICE_RP2350)
static std::string storedSSID = "";
static std::string storedPASS = "";
#endif


#define CONFIG_FILE "/config.cfg"

#if defined(DEVICE_RP2350)
static void setConfigKey(const std::string& key, const std::string& value) {
    if (key == "deviceName") {
        deviceName = value;
    } else if (key == "theme") {
        savedTheme = std::stoi(value);
    } else if (key == "wallpaper") {
        savedWallpaper = std::stoi(value);
    } else if (key == "SSID") {
        storedSSID = value;
    } else if (key == "PASS") {
        storedPASS = value;
    }
}

static std::string getConfigKey(const std::string& key) {
    if (key == "deviceName") return deviceName;
    if (key == "theme") return std::to_string(savedTheme);
    if (key == "wallpaper") return std::to_string(savedWallpaper);
    if (key == "SSID") return storedSSID;
    if (key == "PASS") return storedPASS;
    return "";
}
#else
static void setConfigKey(const std::string& key, const std::string& value) {
    std::string content = "";
    File f = SPIFFS.open(CONFIG_FILE, FILE_READ);
    if (f) {
        while (f.available()) {
            String line = f.readStringUntil('\n');
            std::string l = std::string(line.c_str());
            if (!l.empty() && l.back() == '\r') l.pop_back();
            if (l.empty()) continue;

            size_t tilleq = l.find('=');
            if (tilleq != std::string::npos && l.substr(0, tilleq) == key) continue;

            content += l + "\n";
        }
        f.close();
    }

    content += key + "=" + value + "\n";

    File out = SPIFFS.open(CONFIG_FILE, FILE_WRITE);
    if (out) {
        out.print(content.c_str());
        out.flush();
        out.close();
    }
}

static std::string getConfigKey(const std::string& key) {
    File f = SPIFFS.open(CONFIG_FILE, FILE_READ);
    if (!f) return "";

    while (f.available()) {
        String line = f.readStringUntil('\n');
        std::string l = std::string(line.c_str());
        if (!l.empty() && l.back() == '\r') l.pop_back();
        if (l.empty()) continue;

        size_t eq = l.find('=');
        if (eq != std::string::npos && l.substr(0, eq) == key) {
            f.close();
            return l.substr(eq + 1);
        }
    }

    f.close();
    return "";
}
#endif


void loadConfig() {
#if !defined(DEVICE_RP2350)
    if (!SPIFFS.exists(CONFIG_FILE)) {
        printLine("[CONFIG] No config file, using defaults.");
        logKernelMessage("[CONFIG] No config file, using defaults.");
        return;
    }

    std::string name = getConfigKey("deviceName");
    if (!name.empty() && name.length() <= 32) {
        deviceName = name;
    }

    std::string themeStr = getConfigKey("theme");
    if (!themeStr.empty()) {
        try {
            savedTheme = std::stoi(themeStr);
        } catch (...) {
            savedTheme = 0;
        }
    }

    std::string wallpaperStr = getConfigKey("wallpaper");
    if (!wallpaperStr.empty()) {
        try {
            savedWallpaper = std::stoi(wallpaperStr);
        } catch (...) {
            savedWallpaper = 0;
        }
    }

    printLine("[CONFIG] Loaded: name=" + deviceName +
              " theme=" + std::to_string(savedTheme) +
              " wallpaper=" + std::to_string(savedWallpaper));
    logKernelMessage("[CONFIG] Loaded: name=" + deviceName +
                             " theme=" + std::to_string(savedTheme) +
                             " wallpaper=" + std::to_string(savedWallpaper));
#else
    printLine("[CONFIG] RP2350 config support not available. Using defaults.");
    logKernelMessage("[CONFIG] RP2350 config support not available. Using defaults.");
#endif
}

void saveConfig() {
#if !defined(DEVICE_RP2350)
    setConfigKey("deviceName", deviceName);
    setConfigKey("theme", std::to_string(savedTheme));
    setConfigKey("wallpaper", std::to_string(savedWallpaper));
    printLine("[CONFIG] Saved.");
    logKernelMessage("[CONFIG] Saved.");
#else
    printLine("[CONFIG] Save disabled on this device.");
    logKernelMessage("[CONFIG] Save disabled on this device.");
#endif
}


std::string getDeviceName() {
    return deviceName;
}



void setDeviceName() {
    std::string name = "";
    std::vector<CursorPosition> nameInputPositions;

    while (Serial.available() > 0) Serial.read();

    tft.setCursor(5,currentCursorY);
    print("Enter new username: ");

    tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
    tft.print(" ");
    tft.setCursor(currentCursorX, currentCursorY);
    tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);

    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {

                tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);
                tft.print(" ");
                tft.setCursor(currentCursorX, currentCursorY);
                Serial.println();

                Serial.println();
                break;
            } else if (c == '\b' || c == 127) {
                if (name.length() > 0) {

                    name.pop_back();
                    Serial.write('\b');
                    Serial.write(' ');
                    Serial.write('\b');

                    CursorPosition previousChar = nameInputPositions.back();
                    nameInputPositions.pop_back();

                    currentCursorX = previousChar.x;
                    currentCursorY = previousChar.y;

                    tft.setCursor(currentCursorX, currentCursorY);
                    tft.print("  "); 

                    tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
                    tft.setCursor(currentCursorX, currentCursorY);
                    tft.print(" ");
                    tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);
                    tft.setCursor(currentCursorX, currentCursorY);


                }
            } else if (c >= 32 && c <= 126) {
                nameInputPositions.push_back({currentCursorX, currentCursorY});
                name += c;
                print(c);

                currentCursorX = tft.getCursorX();
                currentCursorY = tft.getCursorY();
                
                tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
                tft.print(" ");
                tft.setCursor(currentCursorX, currentCursorY);
                tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    size_t start = name.find_first_not_of(' ');
    size_t end = name.find_last_not_of(' ');

    if (start == std::string::npos) {
        printLine("");
        printLine("Error: Username cannot be empty.");
        return;
    }

    std::string trimmed = name.substr(start, end - start + 1);

    if (trimmed.length() > 32) {
        printLine("");
        printLine("Error: Username too long (max 32).");
        printLine("Your username is " 
                  + std::to_string(trimmed.length())
                  + " characters long."
                 );
        return;
    }
    deviceName = trimmed;
    setConfigKey("deviceName", deviceName);
    printLineNoSerialLineBreak("");
    printLine("Username set: " + deviceName);
    logKernelMessage("[SYSTEM] Username set: " + deviceName);

}


int getSavedTheme() {
    return savedTheme;
}

void saveSavedTheme(int index) {
    savedTheme = index;
    setConfigKey("theme", std::to_string(savedTheme));
}

int getSavedWallpaper() {
    return savedWallpaper;
}

void saveSavedWallpaper(int index) {
    savedWallpaper = index;
    setConfigKey("wallpaper", std::to_string(savedWallpaper));
}

void setWifiConfig(const std::string& SSID, const std::string& PASS){
    setConfigKey("SSID", SSID);
    setConfigKey("PASS", PASS);
    printLine("[NETWORK] config updated.");
    logKernelMessage("[NETWORK] config updated.");
}


std::string getWifiSSID() {
    return getConfigKey("SSID");
}

std::string getWifiPass() {
    return getConfigKey("PASS");
}
