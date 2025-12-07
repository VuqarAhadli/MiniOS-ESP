#ifndef WALLPAPER_H
#define WALLPAPER_H


#include "display.h"
#include "theme.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <string>


struct Wallpaper {
    char name[32];
    void (*drawWallpaper)();
};

extern Wallpaper wallpapers[];
extern int currentWallpaperNum;
extern int wallpaperCount;
extern Wallpaper currentWallpaper;
void listWallpaper();
void setWallpaper(const std::string& wallpaperName);
Wallpaper getCurrentWallpaper();



#endif