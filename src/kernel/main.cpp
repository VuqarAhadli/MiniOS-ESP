#include <Arduino.h>
#include <string>
#include <vector>
#include "config.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "commands.h"
#include "pug.h"
#include "timeutils.h"
#include "kernel.h"

std::string input = "";

/*!
    Set to true when a fullscreen OS task is exitted
    via vTaskDelete(). This flag lets the shell detect
    the unlock on its next tick and redraw the prompt in terminal.
*/
bool screenJustUnlocked = false;
bool pressedEnd = false;
bool screenLocked = false;
bool inputLocked = false;



std::vector<CursorPosition> inputPositions;

void initProcess(void *parameter) {
    printLine("MiniOS - FreeRTOS Kernel");
    
    printLine("[SYSTEM] Display initialized");
    logKernelMessage("[SYSTEM] Display initialized");

    if (!initFilesystem()) {
        printLine("[ERROR] Filesystem failed");
        logKernelMessage("[ERROR] Filesystem failed");
        vTaskDelete(NULL);
        return;
    }
    
    printLine("[SYSTEM] Filesystem initialized");
    logKernelMessage("[SYSTEM] Filesystem initialized");
    loadConfig(); 
    setTheme(std::to_string(getSavedTheme()));

    printLine("[SYSTEM] MiniOS Ready");
    logKernelMessage("[SYSTEM] MiniOS Ready");
    printLine("Type 'help' for commands");
    printLine("");
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}
int16_t initY = currentCursorY;
int16_t overFlownLines  = 0;
int lastScrollOffset = 0;

void serialInputProcess(void *parameter) {
    const TickType_t delay = 10 / portTICK_PERIOD_MS;
    bool promptPrinted = false;
    vTaskDelay(500 / portTICK_PERIOD_MS);
    tft.setCursor(5, tft.getCursorY());
    printPrompt(true);
    promptPrinted = true;

    for (;;) {

        if (screenJustUnlocked) {
        screenJustUnlocked = false;
        input = "";
        inputPositions.clear();
        overFlownLines = 0;
        initY = currentCursorY;
        tft.setCursor(5, currentCursorY);
        printPrompt(true);
        tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
        tft.print(" ");
        tft.setCursor(currentCursorX, currentCursorY);
        tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);

        }
        
        bool shouldRecover = false;
        if (bufferMutex != NULL) {
            xSemaphoreTake(bufferMutex, portMAX_DELAY);
            shouldRecover = (lastScrollOffset > 0 && scrollOffset == 0);
            xSemaphoreGive(bufferMutex);
        } else {
            shouldRecover = (lastScrollOffset > 0 && scrollOffset == 0);
        }
        
        if (shouldRecover) {
            input = "";
            inputPositions.clear();
            overFlownLines = 0;

            pressedEnd = false;
            // initY = currentCursorY;
            tft.setCursor(5, currentCursorY);
            printPrompt(false);

            currentCursorX = tft.getCursorX();
            currentCursorY = tft.getCursorY();
            initY = currentCursorY;

            promptPrinted = true;
            tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
            tft.print(" ");
            tft.setCursor(currentCursorX, currentCursorY);
            tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);

        }
        
        if (!screenLocked && !inputLocked && Serial.available() ) {

            if (Serial.peek() == '\x1b') {
                Serial.read();
                vTaskDelay(5 / portTICK_PERIOD_MS);
                if (Serial.available() >= 2) {
                    char b = Serial.read();
                    char c = Serial.read();
                    if (b == '[') {
                        if (c == 'A') { scrollUp(4); continue; }
                        else if (c == 'B') { scrollDown(4); continue; }
                        else if (c == 'F') {
                            scrollToBottom();
                            continue;
                        }
                        else if (c == 'H') {
                            scrollToTop();
                            continue;
                        }
                    }
                }
                continue;
            }
            
            int currentScroll = 0;
            if (bufferMutex != NULL) {
                xSemaphoreTake(bufferMutex, portMAX_DELAY);
                currentScroll = scrollOffset;
                xSemaphoreGive(bufferMutex);
            }
            
            char c = Serial.read(); 
            

            if (c == '\n') {
                if (currentScroll > 0) {
                    scrollToBottom();
                    continue;
                }

                tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);
                tft.print(" ");
                tft.setCursor(currentCursorX, currentCursorY);

                if (screenCleared) {
                    currentCursorY = tft.getCursorY();
                    screenCleared = false;
                    tft.setCursor(5, currentCursorY);
                    printPrompt(true);
                    promptPrinted = true; 
                }
                if (input.length() > 0) {
                    addToBuffer(">" + getDeviceName() + "@Mini:" + input);
                    printLineNoBuffer("");
                    runCommand(input);
                    currentCursorY = tft.getCursorY();
                    tft.setCursor(5, currentCursorY);
                    printPrompt(true);
                    promptPrinted = true;
                    tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
                    tft.print(" ");
                    tft.setCursor(currentCursorX, currentCursorY);
                    tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);
                }

                input = "";
                overFlownLines = 0;
                initY = currentCursorY;
                promptPrinted = false; 
            } 
            // Other input only processed when at bottom
            else if (currentScroll == 0) {
                if (c == '\b' || c == 127) {
                    if (input.length() > 0) {
                        input.pop_back();
                        Serial.write('\b');
                        Serial.write(' ');
                        Serial.write('\b');

                        CursorPosition previousChar = inputPositions.back();
                        inputPositions.pop_back();

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
                } else {
                    input += c;
                    inputPositions.push_back({currentCursorX, currentCursorY});
                    print(c);
                    if (initY != currentCursorY){
                        overFlownLines++;
                    }

                    currentCursorX = tft.getCursorX();
                    currentCursorY = tft.getCursorY();

                    tft.setTextColor(getCurrentTheme().bg, getCurrentTheme().fg);
                    tft.print(" ");
                    tft.setCursor(currentCursorX, currentCursorY);
                    tft.setTextColor(getCurrentTheme().fg, getCurrentTheme().bg);
                }
            }
            
        }

        // Update last scroll position for recovery logic
        if (bufferMutex != NULL) {
            xSemaphoreTake(bufferMutex, portMAX_DELAY);
            lastScrollOffset = scrollOffset;
            xSemaphoreGive(bufferMutex);
        } else {
            lastScrollOffset = scrollOffset;
        }

        vTaskDelay(delay);
    }
}

void alarmCheckProcess(void *parameter) {
    const TickType_t delay = 1000 / portTICK_PERIOD_MS;
    
    while (1) {
        if (!screenLocked) {
            checkAlarm();
        }
        vTaskDelay(delay);
    }
}

void watchdogProcess(void *parameter) {
    const TickType_t delay = 5000 / portTICK_PERIOD_MS;
    
    while (1) {
        vTaskDelay(delay);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    while (Serial.available()) {
        Serial.read();
    }
    
    Serial.println("MiniOS - FreeRTOS Kernel");
    Serial.println("Initializing...");
    
    initDisplay();
    kernelInit();
    
    createProcess(initProcess, "init", 4096, 1);
    createProcess(alarmCheckProcess, "alarm", 2048, 1);
    createProcess(watchdogProcess, "watchdog", 1024, 0);
    createProcess(kernelScheduler, "scheduler", 2048, KERNEL_PRIORITY);
    createProcess(serialInputProcess, "shell", 16384, 2);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}