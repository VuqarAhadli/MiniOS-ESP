#include "init.h"

bool initEnded = false;

void initProcess(void *parameter) {

    printLine("MiniOS - FreeRTOS Kernel");
    
    printLine("[SYSTEM] Display initialized");
    logKernelMessage("[SYSTEM] Display initialized");

    if (!initFilesystem()) {
        printLine("[ERROR] Filesystem failed");
        logKernelMessage("[ERROR] Filesystem failed");
        vTaskDelete(NULL);
        return;
    }
    
    printLine("[SYSTEM] Filesystem initialized");
    logKernelMessage("[SYSTEM] Filesystem initialized");
    loadConfig(); 
    setTheme(std::to_string(getSavedTheme()));
    setWallpaper(std::to_string(getSavedWallpaper()));

    printLine("[SYSTEM] MiniOS Ready");
    logKernelMessage("[SYSTEM] MiniOS Ready");
    printLine("Type 'help' for commands");
    printLine("");
    
    initEnded = true;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}