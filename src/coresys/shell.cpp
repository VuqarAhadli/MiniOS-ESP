#include "shell.h"

std::string input = "";
std::vector<CursorPosition> inputPositions;
bool screenJustUnlocked = false;
bool pressedEnd = false;
bool screenLocked = false;
bool inputLocked = false;
int16_t initY = 0;
int16_t overFlownLines = 0;
int lastScrollOffset = 0;


void serialInputProcess(void *parameter) {
    const TickType_t delay = 10 / portTICK_PERIOD_MS;
    while(!initEnded) vTaskDelay(delay);
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