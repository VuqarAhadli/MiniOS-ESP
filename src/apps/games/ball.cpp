// #include "display.h"
// #include "ball.h"
// #include <cstdlib>
// #include <time.h>

// void initialiseBall(int ballRadius, bool trail) {

//     std::srand(time(nullptr));

//     tft.fillScreen(ST77XX_BLACK);
//     screenLocked = true;
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(1);
//     tft.setCursor(5, 230);
//     tft.print("Press ENTER to exit...");
//     Serial.println("Press ENTER to exit...");

//     int x = 160;
//     int y = 115;
//     int speedX = 2;  
//     int speedY = 2;  
//     uint64_t colour = 65535;
    
//     while (true) {
//         if (Serial.available()) {
//             char c = Serial.read();
//             if (c == '\n') {
//                 clearScreen();
//                 screenLocked = false;
//                 break;
//             }
//         }
//         if (!trail) tft.fillCircle(x, y, ballRadius, ST77XX_BLACK);
//         x += speedX;
//         y += speedY;
//         if (x - ballRadius <= 1 || x + ballRadius >= X_MAX){ 
//             speedX = -speedX;
//             colour = std::rand()%65535;
//         }
//         if (y - ballRadius <= 1 || y + ballRadius >= Y_MAX){ 
//             speedY = -speedY;
//             colour = std::rand()%65535;
//         }
//         tft.fillCircle(x, y, ballRadius, colour);
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
// }
#include "display.h"
#include "ball.h"
#include <cstdlib>
#include <time.h>
#include <cmath>
#include <vector>

const float GRAVITY = 0.05f;
const float FLOOR_DAMP = 0.85f;  
const float WALL_DAMP = 0.90f; 

struct Ball {
    float x, y;
    float speedX, speedY;
    uint16_t colour;
};

void initialiseBall(int ballRadius, bool trail, int numBalls) {
    std::srand(time(nullptr));
    tft.fillScreen(ST77XX_BLACK);
    screenLocked = true;
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(5, 230);
    tft.print("Press ENTER to exit...");
    Serial.println("Press ENTER to exit...");

    std::vector<Ball> balls;
    balls.reserve(numBalls);

    for (int i = 0; i < numBalls; i++) {
        float angle = 2.0f * M_PI * ((float)std::rand() / RAND_MAX);
        float speed = 1.5f + (std::rand() / (float)RAND_MAX) * 3.5f;
        Ball b;
        b.x = 160;
        b.y = 115;
        b.speedX = speed * cosf(angle);
        b.speedY = speed * sinf(angle);
        b.colour = std::rand() % 65536;
        balls.push_back(b);
    }

    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                balls.clear();
                clearScreen();
                screenLocked = false;
                break;
            }
        }

        for (Ball& b : balls) {
            if (!trail) tft.fillCircle((int)b.x, (int)b.y, ballRadius, ST77XX_BLACK);
            
            b.speedY += GRAVITY;
            b.x += b.speedX;
            b.y += b.speedY;

            if (b.x - ballRadius <= 0) {
                b.speedX = std::abs(b.speedX) * WALL_DAMP;
                b.x = ballRadius;
                // b.colour = std::rand() % 65536;
            } else if (b.x + ballRadius >= X_MAX) {
                b.speedX = -std::abs(b.speedX) * WALL_DAMP;
                b.x = X_MAX - ballRadius;
                // b.colour = std::rand() % 65536;
            }
            
            if (b.y - ballRadius <= 0) {
                b.speedY = std::abs(b.speedY) * WALL_DAMP;
                b.y = ballRadius;
                // b.colour = std::rand() % 65536;
            } else if (b.y + ballRadius >= Y_MAX) {
                b.speedY = -std::abs(b.speedY) * FLOOR_DAMP;
                b.y = Y_MAX - ballRadius;
                // b.colour = std::rand() % 65536;
            }
            
            tft.fillCircle((int)b.x, (int)b.y, ballRadius, b.colour);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}