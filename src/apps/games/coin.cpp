#include "display.h"
#include "coin.h"
#include <cstdlib>
#include <time.h>
#if !defined(DEVICE_RP2350)
#include <esp_random.h>
#endif

static uint32_t getRandom32() {
#if !defined(DEVICE_RP2350)
    return esp_random();
#else
    return ((uint32_t)std::rand() << 17) ^ ((uint32_t)std::rand() << 2) ^ (uint32_t)(std::rand() & 1);
#endif
}

static const int CX = X_MAX / 2;
static const int CY = Y_MAX / 2 - 10;
static const int R  = 80;  

static const uint16_t BG = 0x0461;
static const uint16_t T_BG = 0xb9bf;
static const uint16_t C_RIM = 0xB5A0;  
static const uint16_t C_OUTER = 0xFEA0;  
static const uint16_t C_MID = 0xFFE0;  
static const uint16_t C_INNER = 0xFFC0;  
static const uint16_t C_TEXT = 0xB5A0;  


static void drawCoin(CoinState state) {

    tft.fillCircle(CX,CY,R,C_MID);
    for (int deg = 0; deg < 360; deg += 4) {
        float rad  = deg * 3.14159f / 180.0f;
        float cosi = cosf(rad), sinu = sinf(rad);
        uint16_t col = (deg % 8 == 0) ? C_RIM : C_OUTER;
         tft.drawLine(CX +(int)((R - 2)*cosi),CY+(int)((R - 2)* sinu),CX+(int)( R *cosi),CY+(int)(R *sinu),col);
    }


    struct { int r; uint16_t col; } rings[] = {
        {R -2, C_OUTER },  
        {R -16, C_INNER },   
        {R -30, C_MID }, 
        {R -44,C_OUTER },   
        {R -56,C_INNER },   
    };

    for (auto& rg : rings) {
        tft.fillCircle(CX, CY, rg.r, rg.col);
    }

    tft.fillCircle(CX, CY, R - 58, C_INNER);



    const char* label = (state == HEAD) ? "HEADS" : "TAILS";
    int charW  = 6 * 3; 
    int labelW = 5 * charW;     
    tft.setCursor(CX - labelW / 2, CY - 8);   
    tft.setTextSize(3);
    tft.setTextColor(C_TEXT, C_INNER);
    tft.print(label);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
}

static void eraseCoin() {
    tft.fillCircle(CX,CY,R,BG);
}

void coinGame() {
    tft.fillScreen(BG);
    screenLocked = true;

    tft.setTextColor(ST77XX_WHITE, T_BG);
    tft.setTextSize(1);
    tft.setCursor(5, 220);
    tft.print("ENTER=exit  SPACE=flip");

    tft.setCursor(CX - 40, CY - 10);
    tft.setTextSize(1);
    tft.print("Press SPACE");
    tft.setCursor(CX - 40 , CY);
    tft.print("to flip...");

    int nTails = 0;
    int nHeads = 0;

    while (true) {
        if (Serial.available()) {
            char c = Serial.read();

            if (c == '\n') {
                clearScreen();
                screenLocked = false;
                return;
            }

            bool doFlip = (c == ' ');

            if (c == '\x1b') {
                vTaskDelay(7 / portTICK_PERIOD_MS);
                while (Serial.available()) Serial.read();
                doFlip = true;
            }

            if (doFlip) {
                    CoinState state = (CoinState)((getRandom32() >> 31) & 1);


                for (int f = 0; f < 3; f++) {
                    eraseCoin();
                    CoinState fake = (CoinState)((getRandom32() >> 31) & 1);
                    vTaskDelay(65 / portTICK_PERIOD_MS);
                }

                eraseCoin();



                drawCoin(state);
                tft.setTextSize(1);
                tft.setTextColor(ST77XX_WHITE, T_BG);
                tft.setCursor(5, 180);
                tft.print("You got: ");

                if (state == HEAD) {
                    tft.print("Heads.");
                    nHeads++;
                } else {
                    tft.print("Tails.");
                    nTails++;
                }

                tft.fillRect(5, 200, 120, 8, T_BG);
                tft.setCursor(5, 200);
                tft.print("Heads: ");
                tft.print(nHeads);
                tft.print("  Tails: ");
                tft.print(nTails);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}