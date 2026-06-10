#ifndef INIT_H
#define INIT_H

#include "kernel.h"
#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "wallpaper.h"

extern bool initEnded;

void initProcess(void *parameter);

#endif