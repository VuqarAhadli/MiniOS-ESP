#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include <Adafruit_ST7789.h>
#include <string>
#if defined(DEVICE_RP2350) && !defined(__FREERTOS)
#define __FREERTOS 1
#endif
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern Adafruit_ST7789 tft;
extern bool screenLocked;
extern bool screenJustUnlocked;
extern bool screenCleared;
extern int16_t currentCursorY;
extern int16_t currentCursorX;

#define MAX_Y 235

void initDisplay();
void applyTheme();
void clearScreen();
void newPage();
void renderScreen();
void printLine(const std::string& s);
void printLineNoBuffer(const std::string& s);
void printLineNoSerialLineBreak(const std::string& s);
void print(const std::string& s);
void print(const char& s);
void printPrompt(bool serial);
void addToBuffer(const std::string& s);

void scrollUp(int lines = 3);
void scrollDown(int lines = 3);
void scrollToBottom();
void scrollToTop();

void screensaver(int mode);

void showLogo();

void drawScreen();
uint8_t sin8(int angle);

#define SCROLL_BUFFER_SIZE 100
#define LINE_HEIGHT 8
/*   30    */
#define LINES_ON_SCREEN (MAX_Y / LINE_HEIGHT) 

extern std::string lineBuffer[SCROLL_BUFFER_SIZE];
extern int bufferHead;
extern int bufferCount;
extern int scrollOffset;
extern SemaphoreHandle_t bufferMutex;

struct CursorPosition {
    int16_t x;
    int16_t y;
};
#endif