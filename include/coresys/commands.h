#ifndef COMMANDS_H
#define COMMANDS_H



#include "help.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "config.h"
// #include "pug.h"
#include "ball.h"
#include "timeutils.h"
#include "kernel.h"
#include "grapher.h"
#include "mirror.h"
#include "pingpong.h"
#include "d20.h"
#include "coin.h"
#include "wallpaper.h"
#include "tools/calc.h"
#include "tools/fetch.h"
#include "tools/ping.h"
#include "tools/utils.h"
#include "tools/sysinfo.h"
#include "tools/settings.h"
#include <math.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <Arduino.h>

void runCommand(const std::string& cmd);
void showVersion();
void showHelp();
void showHelpOS();
void addToHistory(const std::string& cmd);
void showHistory();

void processCommand(const std::string& args);
void showSystemStats();
void killProcessCmd(int pid);

extern bool promptPrinted;
extern Theme current;

#endif