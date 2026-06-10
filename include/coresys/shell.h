#ifndef SHELL_H
#define SHELL_H


#include "kernel.h"
#include "display.h"
#include "theme.h"
#include <Arduino.h>
#include <string>
#include <vector>
#include "config.h"
#include "commands.h"

extern std::string input;

extern std::vector<CursorPosition> inputPositions;


/*!
Set to true when a fullscreen OS task is exitted
via vTaskDelete(). This flag lets the shell detect
the unlock on its next tick and redraw the prompt in terminal.
*/
extern bool screenJustUnlocked;

extern bool pressedEnd;

extern bool screenLocked;

extern bool inputLocked;

extern bool initEnded;

extern int16_t initY;
extern int16_t overFlownLines;
extern int lastScrollOffset;

void serialInputProcess(void *parameter);

#endif