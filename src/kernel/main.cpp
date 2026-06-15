#include <Arduino.h>
#include <string>
#include <vector>
#include "config.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "commands.h"
// #include "pug.h"
#include "timeutils.h"
#include "kernel.h"
#include "shell.h"
#include "init.h"





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