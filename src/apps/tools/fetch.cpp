#include "sysinfo.h"
#include "display.h"
#include "config.h"

void fetch() {
    showLogo();
    // printLine("");
    printLine("User: " + getDeviceName());
    showUptime();
    showMem();
    showChipInfo();
    showFlashInfo();
    showCPUInfo();
    showWiFiInfo();

    printLine("");
    
    tft.setCursor(5,currentCursorY);

    uint16_t colors1[8] = {0x4208,0xF800,0x07E0,0xFFE0,0x001F,0xF81F,0x07FF,0xFFFF};
    uint16_t colors2[8] = {0x0000,0x7800,0x03E0,0x7BE0,0x0010,0x780F,0x03EF,0xC618};
    
    for(uint8_t i = 0 ; i < 8; ++i){
        tft.setTextColor(0x0, colors1[i]);
        tft.print("   ");
    }

    printLine("");
    tft.setCursor(5,currentCursorY);

    for(uint8_t i = 0 ; i < 8; ++i){
        tft.setTextColor(0x0, colors2[i]);
        tft.print("   ");
    }

    printLine("");
}