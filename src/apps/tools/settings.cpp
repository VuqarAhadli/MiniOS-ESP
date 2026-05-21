#include "settings.h"
#include "display.h"
#include "config.h"
#include "theme.h"
#include "network.h"
#include "kernel.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
#include <cstring>

#define MENU_START_X 10
#define MENU_START_Y 0
#define BOX_WIDTH 300
#define BOX_HEIGHT 22
#define BOX_SPACING 25
#define MENU_ITEMS 5
#define THEME_BOX_WIDTH 140
#define THEME_BOX_HEIGHT 18
#define THEME_COL_SPACING 160



typedef enum {
    MENU_DEVICE_NAME,
    MENU_WIFI_STATUS,
    MENU_THEME,
    MENU_TIME_SYNC,
    MENU_EXIT
} MenuItemType;

struct MenuItem {
    MenuItemType type;
    const char* label;
    const char* description;
};

static MenuItem menuItems[MENU_ITEMS] = {
    { MENU_DEVICE_NAME, "Device Name", "" },
    { MENU_WIFI_STATUS, "WiFi Settings", "" },
    { MENU_THEME, "Theme Selection", "" },
    { MENU_TIME_SYNC, "Sync Time (NTP)", "" },
    { MENU_EXIT, "Exit Settings", "" }
};

static void drawBox(int16_t x, int16_t y, uint16_t borderColour, uint16_t bgColour, bool selected, bool subMenu) {

    tft.drawRect(x, y, BOX_WIDTH, subMenu ? BOX_HEIGHT : BOX_HEIGHT, borderColour);
    

    tft.fillRect(x + 1, y + 1, BOX_WIDTH - 2, BOX_HEIGHT - 2, bgColour);
    

    if (selected) {
        tft.drawRect(x - 2, y - 2, BOX_WIDTH + 4, BOX_HEIGHT + 4, getCurrentTheme().prompt.name);
    }
}


static void drawBoxText(int16_t x, int16_t y, const char* label, const char* value, bool selected) {
    uint16_t textColour = selected ? getCurrentTheme().bg : getCurrentTheme().fg;
    
    tft.setTextColor(textColour);
    tft.setTextSize(1);
    tft.setCursor(x + 10, y + 7);
    tft.print(label);
    
    if (strlen(value) > 0) {
        tft.setCursor(x + 160, y + 7);
        tft.setTextSize(1);
        tft.print(value);
    }
}



static std::string getMenuValue(MenuItemType type) {
    switch (type) {
        case MENU_DEVICE_NAME:
            return getDeviceName();
        case MENU_WIFI_STATUS:
            return isConnected() ? "Connected" : "Disconnected";
        case MENU_THEME:
            return std::string(getCurrentTheme().name);
        case MENU_TIME_SYNC:
            return "Press to sync";
        case MENU_EXIT:
            return "";
        default:
            return "";
    }
}

 static void handleWiFiMenu() {
    tft.fillScreen(getCurrentTheme().bg);
    tft.setTextColor(getCurrentTheme().prompt.name);
    tft.setTextSize(2);
    tft.setCursor(120, 0);
    tft.print("WiFi Menu");
    
    int16_t y = 50;
    int selectedWiFi = 0;
    const int wifiOptions = 3;
    
    while (true) {

        for (int i = 0; i < wifiOptions; i++) {
            bool isSelected = (i == selectedWiFi);
            uint16_t bgColour = isSelected ? getCurrentTheme().fg : getCurrentTheme().bg;
            
            drawBox(MENU_START_X, y + (i * BOX_SPACING), getCurrentTheme().prompt.aux, bgColour, isSelected, true);
            
            const char* options[] = {"Connect", "Disconnect", "Back"};
            drawBoxText(MENU_START_X, y + (i * BOX_SPACING), options[i], "", isSelected);
        }
        

        if (Serial.available()) {
            char c = Serial.read();
            
            if (c == '\n') {

                switch (selectedWiFi) {
                    case 0: 
                        connectWiFi(true);
                        break;
                    case 1: 
                        disconnectWiFi();
                        break;
                    case 2: 
                        return;
                }
                tft.fillScreen(getCurrentTheme().bg);
                vTaskDelay(750 / portTICK_PERIOD_MS);
            } else if (c == 'A' || c == 'w') { 
                selectedWiFi = (selectedWiFi - 1 + wifiOptions) % wifiOptions;
            } else if (c == 'B' || c == 's') { 
                selectedWiFi = (selectedWiFi + 1) % wifiOptions;
            }
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

static void handleThemeMenu() {
    tft.fillScreen(getCurrentTheme().bg);
    tft.setTextColor(getCurrentTheme().prompt.name);
    tft.setTextSize(2);
    tft.setCursor(50, 0);
    tft.print("Theme Selection");
    
    int selectedTheme = currentTheme;
    int16_t startY = 40;
    
    while (true) {
        // Draw themes in 2-column layout
        for (int i = 0; i < themeCount; i++) {
            bool isSelected = (i == selectedTheme);
            uint16_t bgColour = isSelected ? getCurrentTheme().fg : getCurrentTheme().bg;
            
            int col = i % 2;  // 0 or 1 for left/right column
            int row = i / 2;  // which row
            
            int16_t x = 10 + (col * THEME_COL_SPACING);
            int16_t y = startY + (row * 25);
            
            if (y > 200) break;
            
            // Draw theme box
            tft.drawRect(x, y, THEME_BOX_WIDTH, THEME_BOX_HEIGHT, getCurrentTheme().prompt.aux);
            tft.fillRect(x + 1, y + 1, THEME_BOX_WIDTH - 2, THEME_BOX_HEIGHT - 2, bgColour);
            
            if (isSelected) {
                tft.drawRect(x - 2, y - 2, THEME_BOX_WIDTH + 4, THEME_BOX_HEIGHT + 4, getCurrentTheme().prompt.name);
            }
            
            uint16_t textColour = isSelected ? getCurrentTheme().bg : getCurrentTheme().fg;
            tft.setTextColor(textColour);
            tft.setTextSize(1);
            tft.setCursor(x + 5, y + 5);
            tft.print(themes[i].name);
        }
        
        if (Serial.available()) {
            char c = Serial.read();
            
            if (c == '\n') {
                setTheme(std::string(themes[selectedTheme].name));
                logKernelMessage("[CONFIG] Theme changed to: " + std::string(themes[selectedTheme].name));
                return;
            } else if (c == 'A' || c == 'w') {  // Up
                selectedTheme = (selectedTheme - 2 + themeCount) % themeCount;
            } else if (c == 'B' || c == 's') {  // Down
                selectedTheme = (selectedTheme + 2) % themeCount;
            } else if (c == 'C' || c == 'a' || c == 'D') {  // Left/Right
                selectedTheme = (selectedTheme + 1) % themeCount;
            }
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}


static void handleDeviceNameEdit() {
    tft.fillScreen(getCurrentTheme().bg);
    tft.setTextColor(getCurrentTheme().prompt.name);
    tft.setTextSize(2);
    tft.setCursor(120, 0);
    tft.print("Device Name");
    
    tft.setTextColor(getCurrentTheme().fg);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Current: ");
    tft.print(getDeviceName().c_str());
    
    tft.setCursor(10, 60);
    tft.print("Press ENTER to go back");
    
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                return;
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

static void handleTimeSync() {
    tft.fillScreen(getCurrentTheme().bg);
    tft.setTextColor(getCurrentTheme().prompt.name);
    tft.setTextSize(2);
    tft.setCursor(120, 120);
    tft.print("Syncing Time...");
    
    if (!isConnected()) {
        tft.setTextColor(getCurrentTheme().fg);
        tft.setTextSize(1);
        tft.setCursor(10, 50);
        tft.setTextColor(ST77XX_RED);
        tft.print("WiFi not connected!");
        tft.setTextColor(getCurrentTheme().fg);
        tft.setCursor(10, 70);
        tft.print("Press ENTER to go back");
        
        while (true) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n') return;
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }
    

    tft.setTextColor(getCurrentTheme().fg);
    tft.setTextSize(1);
    tft.setCursor(120, 120);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("Syncing from NTP server...");
    tft.setTextColor(getCurrentTheme().fg);

#if !defined(DEVICE_RP2350)
    configTime(0, 0, "pool.ntp.org");
    
    int attempts = 0;
    while (time(nullptr) < 100000 && attempts < 30) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        tft.print(".");
        attempts++;
    }
    
    if (time(nullptr) > 100000) {
        tft.fillRect(10, 50, 200, 20, getCurrentTheme().bg);
        tft.setCursor(10, 50);
        tft.setTextColor(ST77XX_GREEN);
        tft.print("Time synced successfully!");
        tft.setTextColor(getCurrentTheme().fg);
        logKernelMessage("[SYSTEM] Time synced via NTP");
    } else {
        tft.fillRect(10, 50, 200, 20, getCurrentTheme().bg);
        tft.setCursor(10, 50);
        tft.print("Time sync failed");
    }
#else
    tft.fillRect(10, 50, 200, 20, getCurrentTheme().bg);
    tft.setCursor(10, 50);
    tft.setTextColor(ST77XX_RED);
    tft.print("Time sync unsupported");
    tft.setTextColor(getCurrentTheme().fg);
#endif
    
    tft.setCursor(10, 80);
    tft.print("Press ENTER to go back");
    
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') return;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

static void settingsTask(void* pvParameters) {
    vTaskDelay(16 / portTICK_PERIOD_MS);
    
    screenLocked = true;
    tft.fillScreen(getCurrentTheme().bg);
    
    int selectedItem = 0;
    
    while (true) {
        tft.setTextColor(getCurrentTheme().prompt.name);
        tft.setTextSize(2);
        tft.setCursor(MENU_START_X, 2);
        tft.print("Settings Menu");
        
        for (int i = 0; i < MENU_ITEMS; i++) {
            bool isSelected = (i == selectedItem);
            uint16_t bgColour = isSelected ? getCurrentTheme().fg : getCurrentTheme().bg;
            int16_t y = MENU_START_Y + 25 + (i * BOX_SPACING);
            
            drawBox(MENU_START_X, y, getCurrentTheme().prompt.aux, bgColour, isSelected, true);
            
            std::string value = getMenuValue(menuItems[i].type);
            drawBoxText(MENU_START_X, y, menuItems[i].label, value.c_str(), isSelected);
        }
        
        tft.setTextColor(getCurrentTheme().fg);
        tft.setTextSize(1);
        tft.setCursor(MENU_START_X, 210);
        tft.print("UP/DOWN: Navigate  ENTER: Select");
        
        if (Serial.available()) {
            char c = Serial.read();
            
            if (c == '\n') {

                switch (menuItems[selectedItem].type) {
                    case MENU_DEVICE_NAME:
                        handleDeviceNameEdit();
                        tft.fillScreen(getCurrentTheme().bg);
                        break;
                    case MENU_WIFI_STATUS:
                        handleWiFiMenu();
                        tft.fillScreen(getCurrentTheme().bg);
                        break;
                    case MENU_THEME:
                        handleThemeMenu();
                        applyTheme();
                        tft.fillScreen(getCurrentTheme().bg);
                        break;
                    case MENU_TIME_SYNC:
                        handleTimeSync();
                        tft.fillScreen(getCurrentTheme().bg);
                        break;
                    case MENU_EXIT:
                        clearScreen();
                        screenLocked = false;
                        screenJustUnlocked = true;
                        logKernelMessage("[SYSTEM] Settings menu closed");
                        vTaskDelete(NULL);
                        vTaskDelay(30 / portTICK_PERIOD_MS);
                        return;
                }
            } else if (c == 'A' || c == 'w') { // Up arrow or 'w' key
                selectedItem = (selectedItem - 1 + MENU_ITEMS) % MENU_ITEMS;
            } else if (c == 'B' || c == 's') { // Down arrow or 's' key
                selectedItem = (selectedItem + 1) % MENU_ITEMS;
            }
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void showSettings() {
    logKernelMessage("[SYSTEM] Opening settings menu");
    xTaskCreate(settingsTask, "SettingsTask", 4096, NULL, 1, NULL);
}
