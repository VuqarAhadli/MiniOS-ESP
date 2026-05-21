#include "sysinfo.h"
#include "display.h"
#include "config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

#if !defined(DEVICE_RP2350)
#include <esp_system.h>
#include <WiFi.h>
#endif

void showMem() {
#if !defined(DEVICE_RP2350)
    uint32_t fbytes = ESP.getFreeHeap();
    uint32_t mibytes = ESP.getMinFreeHeap();
    uint32_t mabytes = ESP.getMaxAllocHeap();
    float fkb = fbytes / 1024.0;
    float mikb = mibytes / 1024.0;
    float makb = mabytes / 1024.0;

    char buf[100];
    sprintf(buf, "Free Heap: %u bytes (%.2f KB)", ESP.getFreeHeap(), fkb);
    printLine(buf);
    sprintf(buf, "Min Free Heap: %u bytes (%.2f KB)", ESP.getMinFreeHeap(), mikb);
    printLine(buf);
    sprintf(buf, "Max Alloc Heap: %u bytes (%.2f KB)", ESP.getMaxAllocHeap(), makb);
    printLine(buf);
#else
    printLine("Free Heap: unsupported");
    printLine("Min Free Heap: unsupported");
    printLine("Max Alloc Heap: unsupported");
#endif
}

void showUptime() {
    unsigned long s = millis() / 1000;
    unsigned long h = s / 3600;
    unsigned long m = (s % 3600) / 60;
    unsigned long sec = s % 60;
    printLine("Uptime: " + std::to_string(h) + "h " + std::to_string(m) + "m " + std::to_string(sec) + "s");
}

void doReboot() {
    printLine("Rebooting...");
    vTaskDelay(100 / portTICK_PERIOD_MS);
#if !defined(DEVICE_RP2350)
    ESP.restart();
#else
    // No hardware restart support available on this device.
#endif
}

void showChipInfo() {
#if !defined(DEVICE_RP2350)
    printLine("Chip Model: " + std::string(ESP.getChipModel()));
    printLine("Chip Cores: " + std::to_string(ESP.getChipCores()));
    printLine("Chip Revision: " + std::to_string(ESP.getChipRevision()));
#else
    printLine("Chip Model: RP2350");
    printLine("Chip Cores: unknown");
    printLine("Chip Revision: unknown");
#endif
}

void showCPUInfo() {
#if !defined(DEVICE_RP2350)
    printLine(
        "CPU: " +
        std::to_string(ESP.getCpuFreqMHz()) + " MHz"
    );
#else
    printLine("CPU: unknown MHz");
#endif
}

void showFlashInfo() {
#if !defined(DEVICE_RP2350)
    uint32_t flashSize = ESP.getFlashChipSize() / 1024 / 1024;
    printLine(
        "Flash: " +
        std::to_string(flashSize) + " MB"
    );
    printLine("Flash Speed: " + std::to_string(ESP.getFlashChipSpeed() / 1000000) + " MHz");
#else
    printLine("Flash: unsupported");
    printLine("Flash Speed: unsupported");
#endif
}

void showWiFiInfo() {
#if !defined(DEVICE_RP2350)
    if (WiFi.isConnected()) {
        printLine("WiFi RSSI: " + std::to_string(WiFi.RSSI()) + " dBm");
        printLine("WiFi Channel: " + std::to_string(WiFi.channel()));
        printLine("MAC: " + std::string(WiFi.macAddress().c_str()));
    } else {
        printLine("WiFi: Disconnected");
    }
#else
    printLine("WiFi: unsupported");
#endif
}

