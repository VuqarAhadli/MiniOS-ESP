#include "wallpaper.h"
#include "config.h"
#include "display.h"
#include "kernel.h"

int currentWallpaperNum = 0;
int wallpaperCount = 3;

void drawNoneWallpaper() {
    tft.fillScreen(getCurrentTheme().bg);
}


void drawBlocksWallpaper() {
    tft.fillRect(0, 0, 80, 80, 0xE987);

    tft.fillRect(0, 80, 80, 80, 0x61B0);

    tft.fillRect(0, 160, 80, 80, 0x2E0F);

    tft.fillRect(80, 0, 80, 80, 0xE521);

    tft.fillRect(80, 80, 80, 80, 0x1AF6);

    tft.fillRect(80, 160, 80, 80, 0x63F1);

    tft.fillRect(160, 0, 80, 80, 0xFAA4);

    tft.fillRect(160, 80, 80, 80, 0xCEE7);

    tft.fillRect(160, 160, 80, 80, 0x651D);

    tft.fillRect(240, 0, 80, 80, 0x6225);

    tft.fillRect(240, 80, 80, 80, 0x1B9B);

    tft.fillRect(240, 160, 80, 80, 0xC0E5);
}




static void draw_polygon_1() {
    tft.drawLine(106, 40, 235, 70, 0xCD5C);
    tft.drawLine(235, 70, 103, 138, 0xCD5C);
    tft.drawLine(103, 138, 185, 33, 0xCD5C);
    tft.drawLine(185, 33, 199, 152, 0xCD5C);
    tft.drawLine(199, 152, 106, 40, 0xCD5C);
}

static void draw_polygon_2() {
    tft.drawLine(30, 128, 103, 198, 0xCD5C);
    tft.drawLine(103, 198, 29, 222, 0xCD5C);
    tft.drawLine(29, 222, 86, 135, 0xCD5C);
    tft.drawLine(86, 135, 78, 234, 0xCD5C);
    tft.drawLine(78, 234, 30, 128, 0xCD5C);
}

static void draw_polygon_3() {
    tft.drawLine(257, 198, 279, 129, 0xCD5C);
    tft.drawLine(279, 129, 299, 199, 0xCD5C);
    tft.drawLine(299, 199, 256, 143, 0xCD5C);
    tft.drawLine(256, 143, 307, 157, 0xCD5C);
    tft.drawLine(307, 157, 257, 198, 0xCD5C);
}

static void draw_polygon_4() {
    tft.drawLine(162, 158, 175, 237, 0xCD5C);
    tft.drawLine(175, 237, 226, 200, 0xCD5C);
    tft.drawLine(226, 200, 139, 209, 0xCD5C);
    tft.drawLine(139, 209, 224, 232, 0xCD5C);
    tft.drawLine(224, 232, 162, 158, 0xCD5C);
}

static void draw_polygon_5() {
    tft.drawLine(39, 6, 15, 99, 0xCD5C);
    tft.drawLine(15, 99, 94, 87, 0xCD5C);
    tft.drawLine(94, 87, 3, 35, 0xCD5C);
    tft.drawLine(3, 35, 71, 131, 0xCD5C);
    tft.drawLine(71, 131, 39, 6, 0xCD5C);
}

static void draw_polygon_6() {
    tft.drawLine(285, 13, 263, 76, 0xCD5C);
    tft.drawLine(263, 76, 301, 76, 0xCD5C);
    tft.drawLine(301, 76, 250, 37, 0xCD5C);
    tft.drawLine(250, 37, 285, 92, 0xCD5C);
    tft.drawLine(285, 92, 285, 13, 0xCD5C);
}
void drawStarsWallpaper() {
    tft.fillScreen(0x0);

    draw_polygon_1();

    draw_polygon_2();

    draw_polygon_3();

    draw_polygon_4();

    draw_polygon_5();

    draw_polygon_6();
}

Wallpaper wallpapers[] = {
        {"none", drawNoneWallpaper},
        {"blocks", drawBlocksWallpaper},
        {"stars", drawStarsWallpaper}
};

Wallpaper currentWallpaper = wallpapers[0];

void listWallpaper() {
    for (int i = 0 ; i < wallpaperCount; ++i) {
        std::string marker = (i == currentWallpaperNum) ? " *" : "";
        printLine(std::to_string(i) + "." + wallpapers[i].name + marker);
    }
}

void setWallpaper(const std::string& wallpaperName) {
    for (int i = 0; i < wallpaperCount; i++) {
        if (wallpaperName == wallpapers[i].name) {
            currentWallpaperNum = i;
            currentWallpaper = wallpapers[i];
            saveSavedWallpaper(currentWallpaperNum);
            clearScreen();
            printLine("[SYSTEM] Wallpaper set: " + std::string(wallpapers[currentWallpaperNum].name));
            logKernelMessage("[SYSTEM] Wallpaper set: " + std::string(wallpapers[currentWallpaperNum].name));
            return;
        }
    }
    
    try {
        int wn = std::stoi(wallpaperName);
        if (wn >= 0 && wn < wallpaperCount) {
            currentWallpaperNum = wn;
            currentWallpaper = wallpapers[wn];
            saveSavedWallpaper(currentWallpaperNum);
            clearScreen();
            printLine("[SYSTEM] Wallpaper set: " + std::string(wallpapers[currentWallpaperNum].name));
            logKernelMessage("[SYSTEM] Wallpaper set: " + std::string(wallpapers[currentWallpaperNum].name));

            return;
        }
    } catch (...) {
    }
    
    printLine("Invalid wallpaper.");
    printLine("Use 'wallpapers' to list.");
}

Wallpaper getCurrentWallpaper() {
    return wallpapers[currentWallpaperNum];
}