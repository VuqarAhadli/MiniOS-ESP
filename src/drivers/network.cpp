#if !defined(DEVICE_RP2350)

#include "network.h"
#include "display.h"
#include "timeutils.h"
#include "config.h"
#include "kernel.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Ping.h>
#include <string>

std::string WIFI_SSID = "";
std::string WIFI_PASS = "";
NetworkStatus networkStatus = NET_DISCONNECTED;
extern bool inputLocked;

void connectWiFi(bool useConfig) {

    if (!useConfig){
        inputLocked = true;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        
        if (WiFi.status() == WL_CONNECTED) {
            printLine("Already connected!");
            printLine("SSID: " + std::string(WiFi.SSID().c_str()));
            printLine("IP: " + std::string(WiFi.localIP().toString().c_str()));
            printLine("RSSI: " + std::to_string(WiFi.RSSI()) + " dBm");
            inputLocked = false;
            return;
        }
        
        while (Serial.available() > 0) Serial.read();
        tft.setCursor(5,currentCursorY);
        print("Enter SSID: ");
        WIFI_SSID = "";
        while (true) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n' || c == '\r') {
                    if (WIFI_SSID.length() > 0) {
                        Serial.println();
                        break;
                    }
                } else if (c == '\b' || c == 127) {
                    if (WIFI_SSID.length() > 0) {
                        WIFI_SSID.pop_back();
                        Serial.write('\b');
                        Serial.write(' ');
                        Serial.write('\b');
                        currentCursorX -= 6;
                        tft.setCursor(currentCursorX, currentCursorY);
                        tft.print(" ");
                        tft.setCursor(currentCursorX, currentCursorY);
                    }
                } else if (c >= 32 && c <= 126) {
                    WIFI_SSID += c;
                    print(c);
                }
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        WIFI_SSID.erase(0, WIFI_SSID.find_first_not_of(" \t\n\r\f\v"));
        WIFI_SSID.erase(WIFI_SSID.find_last_not_of(" \t\n\r\f\v") + 1);
        
        while (Serial.available() > 0) Serial.read();
        printLine("");
        tft.setCursor(5,currentCursorY);
        print("Enter Password: ");
        WIFI_PASS = "";
        while (true) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n' || c == '\r') {
                    if (WIFI_PASS.length() > 0) {
                        Serial.println();
                        break;
                    }
                } else if (c == '\b' || c == 127) {
                    if (WIFI_PASS.length() > 0) {
                        WIFI_PASS.pop_back();
                        Serial.write('\b');
                        Serial.write(' ');
                        Serial.write('\b');
                        currentCursorX -= 6;
                        tft.setCursor(currentCursorX, currentCursorY);
                        tft.print(" ");
                        tft.setCursor(currentCursorX, currentCursorY);
                    }
                } else if (c >= 32 && c <= 126) {
                    WIFI_PASS += c;
                    print('*');
                }
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        WIFI_PASS.erase(0, WIFI_PASS.find_first_not_of(" \t\n\r\f\v"));
        WIFI_PASS.erase(WIFI_PASS.find_last_not_of(" \t\n\r\f\v") + 1);
        
        inputLocked = false;
    }
    else{
        if(WIFI_PASS.empty() || WIFI_SSID.empty()){
            WIFI_SSID = getWifiSSID();
            WIFI_PASS = getWifiPass();
        }
    }
    printLine("");
    printLine("Connecting to: " + WIFI_SSID);

    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    
    int attempts = 0;
    tft.setCursor(5,currentCursorY);
    while (WiFi.status() != WL_CONNECTED && attempts < 12) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        printLine("");
        printLine("Connected!");
        logKernelMessage("[NETWORK] Connected!");
        printLine("SSID: " + std::string(WiFi.SSID().c_str()));
        logKernelMessage("[NETWORK] SSID: " + std::string(WiFi.SSID().c_str()));
        printLine("IP: " + std::string(WiFi.localIP().toString().c_str()));
        logKernelMessage("[NETWORK] IP: " + std::string(WiFi.localIP().toString().c_str()));
        printLine("RSSI: " + std::to_string(WiFi.RSSI()) + " dBm");
        logKernelMessage("[NETWORK] RSSI: " + std::to_string(WiFi.RSSI()) + " dBm");

        if(!useConfig) setWifiConfig(WIFI_SSID,WIFI_PASS);
        syncTime();
    } else {
        printLine("");
        printLine("Failed to connect.");
        logKernelMessage("[NETWORK] Failed to connect.");
    }
}

void disconnectWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        printLine("Not connected to WiFi");
        logKernelMessage("[NETWORK] Not connected to WiFi");
        return;
    }
    
    WiFi.disconnect();
    networkStatus = NET_DISCONNECTED;
    printLine("WiFi disconnected");
    logKernelMessage("[NETWORK] WiFi disconnected");
}

void scanWiFi() {
    printLine("Scanning networks...");
    
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        printLine("No networks found");
        logKernelMessage("[NETWORK] No networks found");
    } else {
        printLine("Found " + std::to_string(n) + " networks:");
        printLine("");
        printLine("  SSID                   RSSI  Ch  Enc");
        printLine("  ----------------------------------------");
        
        for (int i = 0; i < n; i++) {
            String ssid_s = WiFi.SSID(i);
            std::string ssid = std::string(ssid_s.c_str());
            if (ssid.length() > 20) ssid = ssid.substr(0, 20);
            
            std::string encryption;
            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN: encryption = "Open"; break;
                case WIFI_AUTH_WEP: encryption = "WEP"; break;
                case WIFI_AUTH_WPA_PSK: encryption = "WPA"; break;
                case WIFI_AUTH_WPA2_PSK: encryption = "WPA2"; break;
                case WIFI_AUTH_WPA_WPA2_PSK: encryption = "WPA/2"; break;
                default: encryption = "WPA2"; break;
            }
            
            char line[60];
            sprintf(line, "%2d %-20s %4d  %2d  %s",
                    i + 1,
                    ssid.c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    encryption.c_str());
            printLine(line);
        }
    }
    
    WiFi.scanDelete();
}

void showNetworkInfo() {
    if (WiFi.status() != WL_CONNECTED) {
        printLine("Not connected to WiFi");
        logKernelMessage("[NETWORK] Not connected to WiFi");
        return;
    }
    
    printLine("IP Address: " + std::string(WiFi.localIP().toString().c_str()));
    printLine("Gateway: " + std::string(WiFi.gatewayIP().toString().c_str()));
    printLine("Subnet: " + std::string(WiFi.subnetMask().toString().c_str()));
    printLine("DNS: " + std::string(WiFi.dnsIP().toString().c_str()));
    printLine("MAC: " + std::string(WiFi.macAddress().c_str()));
    printLine("RSSI: " + std::to_string(WiFi.RSSI()) + " dBm");
    printLine("Channel: " + std::to_string(WiFi.channel()));
}

bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

std::string getLocalIP() {
    return std::string(WiFi.localIP().toString().c_str());
}

int getSignalStrength() {
    return WiFi.RSSI();
}

#else

#include "network.h"
#include "display.h"
#include "kernel.h"
#include <string>

std::string WIFI_SSID = "";
std::string WIFI_PASS = "";
NetworkStatus networkStatus = NET_DISCONNECTED;
extern bool inputLocked;

void connectWiFi(bool useConfig) {
    printLine("WiFi is not supported on this device.");
}

void disconnectWiFi() {
    printLine("WiFi is not supported on this device.");
}

void scanWiFi() {
    printLine("WiFi scan is not supported on this device.");
}

void showNetworkInfo() {
    printLine("WiFi is not supported on this device.");
}

bool isConnected() {
    return false;
}

std::string getLocalIP() {
    return "";
}

int getSignalStrength() {
    return 0;
}

#endif