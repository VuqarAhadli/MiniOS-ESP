#ifndef CONFIG_H
#define CONFIG_H
#include <string>



#if defined(DEVICE_ESP32_STANDARD)
    // ESP32 SPI pins pre-configured
    #define TFT_MOSI 23
    #define TFT_SCLK 18
    #define TFT_CS 5
    #define TFT_DC 2
    #define TFT_RST 4
    #define DEVICE_NAME "ESP32"

#elif defined(DEVICE_ESP32_S3)
    // ESP32 S3 SPI pins pre-configured
    #define TFT_MOSI 17
    #define TFT_SCLK 18
    #define TFT_CS 14
    #define TFT_DC 15
    #define TFT_RST 4 

    #define DEVICE_NAME "ESP32-S3"

#elif defined(DEVICE_RP2350)
    #define TFT_CS 17
    #define TFT_RST 20
    #define TFT_DC 26
    #define TFT_MOSI 19
    #define TFT_SCLK 18

    #define DEVICE_NAME "RP-2350"
#else
    // Default fallback (ESP32 Standard)
    #define TFT_CS 5
    #define TFT_DC 2
    #define TFT_RST 4
    #define DEVICE_NAME "ESP32"

#endif

// #define MAX_Y    230

// TIME
extern const char* NTP_SERVER;
extern const long GMT_OFFSET;
extern const int DAYLIGHT_OFFSET;

// OS INFO
extern const char* OS_VERSION;

// USERSPACE PERSISTENCE
void loadConfig();      
void saveConfig();      

std::string getDeviceName();
void setDeviceName();

int getSavedTheme();          
void saveSavedTheme(int index);

int getSavedTheme();          
void saveSavedTheme(int index);

int getSavedWallpaper();          
void saveSavedWallpaper(int index);


void setWifiConfig(const std::string& SSID, const std::string& PASS);
std::string getWifiSSID();
std::string getWifiPass();

#endif