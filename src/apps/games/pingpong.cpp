#include "display.h"
#include "pingpong.h"
#include <cstdlib>
#include <time.h>
#include <algorithm> 
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>




void fillBorders(void) {
    tft.fillRect(5, 0, 315, 5, 0xDC5B);
    tft.fillRect(5, 225, 315, 5, 0xDC5B);
    tft.fillRect(315, 5, 5, 220, 0xF602);

    tft.drawCircle(160, 113, 42, 0xE0C4);
    tft.drawLine(159, 5, 159, 70, 0xE0C4);
    tft.drawLine(159, 155, 159, 224, 0xE0C4);
}


void drawLoseMenu(int score) {
    tft.fillScreen(0x0);

    tft.fillCircle(6, 80, 35, 0xFFFF);
    tft.drawLine(34, 52, 167, 36, 0xE987);
    tft.drawLine(43, 66, 181, 59, 0xF685);
    tft.drawLine(46, 79, 196, 80, 0xE521);
    tft.drawLine(47, 92, 205, 106, 0xC220);
    tft.drawLine(44, 104, 206, 131, 0xFF2B);
    tft.setTextColor(0xE0C4);
    tft.setTextWrap(false);
    tft.setFont(&FreeSansBold24pt7b);
    tft.setCursor(60, 177);
    tft.println("You Lost");
    tft.setTextColor(0xE300);
    tft.setFont(&FreeSansBold18pt7b);
    tft.setCursor(9, 219);
    tft.print("Press 'r' to restart");
    tft.setTextColor(0xFFFF);
    tft.setFont();
}

void draw_polygon_4() {
    tft.drawLine(171, 67, 44, 143, 0x250D);
    tft.drawLine(44, 143, 211, 116, 0x250D);
    tft.drawLine(211, 116, 56, 160, 0x250D);
    tft.drawLine(56, 160, 209, 116, 0x250D);
    tft.drawLine(209, 116, 53, 152, 0x250D);
    tft.drawLine(53, 152, 170, 69, 0x250D);
    tft.drawLine(170, 69, 57, 156, 0x250D);
    tft.drawLine(57, 156, 208, 115, 0x250D);
    tft.drawLine(208, 115, 49, 148, 0x250D);
    tft.drawLine(49, 148, 171, 67, 0x250D);
}

void drawMenu() {
    tft.fillScreen(0x0);
    tft.setTextColor(0xFFFF);
    tft.setTextWrap(false);
    tft.setFont(&FreeSerifBold18pt7b);
    tft.setCursor(84, 33);
    tft.println("Ping Pong");
    draw_polygon_4();
    tft.fillCircle(199, 86, 35, 0xFFFF);
    tft.setTextColor(0xFF8D);
    tft.setCursor(91, 187);
    tft.println("W for up");
    tft.setTextColor(0x2E0F);
    tft.setCursor(77, 223);
    tft.println("S for down");
    tft.setFont();
}


static void drawGameField(int16_t racketX, int16_t racketY, int16_t score, uint16_t colour) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(5, 230);
    tft.print("Press ENTER to exit...");
    tft.setCursor(180, 230);
    tft.print("Score:");
    tft.print(score);
    tft.drawRect(racketX, racketY, RACKET_WIDTH, RACKET_LENGTH, colour);
    fillBorders();
}

static int optimisedBorderRedraw = 0;

typedef enum {
    PING_PONG_GAME_STATE_PLAYING,
    PING_PONG_GAME_STATE_LOST,
    PING_PONG_GAME_STATE_WAITING_RESTART,
    PING_PONG_GAME_STATE_WAITING_START_MENU
} GameState;

static void pingpongTask(void* pvParameters) {
    vTaskDelay(70 / portTICK_PERIOD_MS);
    int16_t score = 0;
    std::srand(time(nullptr));

    screenLocked = true;

    int x = X_MAX / 2;
    int y = Y_MAX / 2;
    int speedX = 1;
    int speedY = 1;
    int ballRadius = 5;
    uint16_t colour = 65535;
    uint16_t ballColour = 65535;
    int16_t racketX = 0;
    int16_t racketY = Y_MAX / 2;

    GameState state = PING_PONG_GAME_STATE_WAITING_START_MENU;

    while (true) {
        if (Serial.available()) {
            char c = Serial.read();

            if (c == '\n') {
                clearScreen();
                screenLocked = false;
                screenJustUnlocked = true;
                vTaskDelete(NULL);
                vTaskDelay(30 / portTICK_PERIOD_MS);
                return;
            }

            if (state == PING_PONG_GAME_STATE_WAITING_RESTART && (c == 'r' || c == 'R')) {
                score = 0;
                ballColour = 65535;
                racketY = Y_MAX / 2;
                x = X_MAX / 2;
                y = Y_MAX / 2;
                speedX = 1;
                speedY = 1;
                optimisedBorderRedraw = 0;
                drawGameField(racketX, racketY, score, colour);
                state = PING_PONG_GAME_STATE_PLAYING;
            }

            if (state == PING_PONG_GAME_STATE_PLAYING) {
                if (c == 'W' || c == 'w') {
                    tft.fillRect(racketX, racketY, RACKET_WIDTH, RACKET_LENGTH, ST77XX_BLACK);
                    racketY = std::max((int16_t)0, (int16_t)(racketY - 15));
                    tft.fillRect(racketX, racketY, RACKET_WIDTH, RACKET_LENGTH, colour);
                } else if (c == 'S' || c == 's') {
                    tft.fillRect(racketX, racketY, RACKET_WIDTH, RACKET_LENGTH, ST77XX_BLACK);
                    racketY = std::min((int16_t)(Y_MAX - RACKET_LENGTH), (int16_t)(racketY + 15));
                    tft.fillRect(racketX, racketY, RACKET_WIDTH, RACKET_LENGTH, colour);
                }
            }
        }


        if (state == PING_PONG_GAME_STATE_WAITING_START_MENU) {
            drawMenu();
            for (;;) {
                if (Serial.available()) {
                    char c = Serial.read();
                    if (c == '\n') {
                        clearScreen();
                        screenLocked = false;
                        screenJustUnlocked = true;
                        vTaskDelete(NULL);
                        vTaskDelay(30 / portTICK_PERIOD_MS);
                        return;
                    }
                    if (c != '\r') {
                        drawGameField(racketX, racketY, score, colour);
                        state = PING_PONG_GAME_STATE_PLAYING;
                        break;
                    }
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

        } else if (state == PING_PONG_GAME_STATE_PLAYING) {

            tft.fillCircle(x, y, ballRadius, ST77XX_BLACK);

            x += speedX;
            y += speedY;

            if (x > 0 &&  x - ballRadius <= racketX + RACKET_WIDTH &&
                y >= racketY && y <= racketY + RACKET_LENGTH) {

                x = racketX + RACKET_WIDTH + ballRadius;
                speedX = -speedX;
                speedY += (std::rand() % 3 - 1);
                speedY = std::max(-2, std::min(speedY, 2));
                score++;
                if (score == 5) ballColour = ST77XX_YELLOW;
                if (score == 10) ballColour = ST77XX_ORANGE;
                if (score == 15) ballColour = ST77XX_GREEN;
                if (score == 20) ballColour = ST77XX_CYAN;
                tft.setCursor(180, 230);
                tft.fillRect(180, 230, 140, 10, ST77XX_BLACK);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(1);
                tft.print("Score:");
                tft.print(score);
            }


            if (y - ballRadius <= 0 || y + ballRadius >= Y_MAX) {
                speedY = -speedY;
            }


            if (x + ballRadius >= X_MAX) {
                x = X_MAX - ballRadius;
                speedX = -speedX;
            }


            if (x - ballRadius < 0) {
                tft.fillCircle(x, y, ballRadius, ST77XX_BLACK);
                tft.fillRect(racketX, racketY, RACKET_WIDTH, RACKET_LENGTH, ST77XX_BLACK);

                x = X_MAX / 2;
                y = Y_MAX / 2;
                speedX = ((std::rand() % 2) == 1) ? 1 : -1;
                speedY = ((std::rand() % 2) == 1) ? -2 : 2;
                optimisedBorderRedraw = 0;

                state = PING_PONG_GAME_STATE_LOST;
                vTaskDelay(10 / portTICK_PERIOD_MS);
                continue;
            }


            optimisedBorderRedraw = (optimisedBorderRedraw + 1) % 5;
            if (optimisedBorderRedraw == 0) fillBorders();

            tft.fillCircle(x, y, ballRadius, ballColour);

        } else if (state == PING_PONG_GAME_STATE_LOST) {
            drawLoseMenu(score);
            state = PING_PONG_GAME_STATE_WAITING_RESTART;

        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void pingpongGame() {
    xTaskCreate(
        pingpongTask,
        "PingPongTask",
        6144,
        NULL,
        1,
        NULL
    );
}