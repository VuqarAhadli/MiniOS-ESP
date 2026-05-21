#include "kernel.h"
#include "display.h"
#include <Arduino.h>
#if defined(DEVICE_RP2350) && !defined(__FREERTOS)
#define __FREERTOS 1
#endif
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#if !defined(DEVICE_RP2350)
#include <esp_system.h>
#endif
#include <vector>
#include <string>
static Process processTable[MAX_PROCESSES];
static int processCount = 0;
static int nextPID = 1;
static uint32_t bootTime = 0;
static SemaphoreHandle_t kernelMutex = NULL;

std::vector<std::string> kernelMessages;

const size_t MAX_LOG = 100;

void logKernelMessage(const std::string& msg) {
    if (kernelMessages.size() >= MAX_LOG) {
        kernelMessages.erase(kernelMessages.begin()); 
    }
    kernelMessages.push_back(msg);
}

void kernelInit() {
    bootTime = millis();
    kernelMutex = xSemaphoreCreateMutex();
    
    if (!kernelMutex) {
        Serial.println("[KERNEL] ERROR: Failed to create kernel mutex");
        logKernelMessage("[KERNEL] ERROR: Failed to create kernel mutex");
        return;
    }
    
    memset(processTable, 0, sizeof(processTable));
    processCount = 0;
    
    Serial.println("[KERNEL] Kernel initialized");
    logKernelMessage("[KERNEL] Kernel initialized");
}

int createProcess(TaskFunction_t function, const char* name, uint32_t stackSize, 
                  UBaseType_t priority) {
    if (processCount >= MAX_PROCESSES) {
        Serial.println("[KERNEL] ERROR: Process table full");
        logKernelMessage("[KERNEL] ERROR: Process table full");
        return -1;
    }
    
    xSemaphoreTake(kernelMutex, portMAX_DELAY);
    
    TaskHandle_t handle = NULL;
    BaseType_t result = xTaskCreate(
        function,
        name,
        stackSize,
        NULL,
        priority,
        &handle
    );
    
    if (result != pdPASS) {
        xSemaphoreGive(kernelMutex);
        Serial.printf("[KERNEL] ERROR: Failed to create process '%s'\n", name);
        logKernelMessage(std::string("[KERNEL] ERROR: Failed to create process ") + name);
        return -1;
    }
    
    Process *proc = &processTable[processCount];
    proc->handle = handle;
    proc->name = name;
    proc->state = PROC_READY;
    proc->priority = priority;
    proc->createdAt = millis();
    proc->stackSize = stackSize;
    proc->pid = nextPID++;
    
    int pid = proc->pid;
    processCount++;
    
    xSemaphoreGive(kernelMutex);
    
    Serial.printf("[KERNEL] Created process '%s' (PID: %d, Priority: %d)\n", 
                  name, pid, priority);
    logKernelMessage(
    std::string("[KERNEL] Created process '") + name +
    "'(PID:" + std::to_string(pid) +
    ",Priority:" + std::to_string(priority) + ")"
    );
    return pid;
}

int killProcess(int pid) {
    xSemaphoreTake(kernelMutex, portMAX_DELAY);
    
    for (int i = 0; i < processCount; i++) {
        if (processTable[i].pid == pid) {
            TaskHandle_t handle = processTable[i].handle;
            const char* name = processTable[i].name;
            
            for (int j = i; j < processCount - 1; j++) {
                processTable[j] = processTable[j + 1];
            }
            processCount--;
            
            xSemaphoreGive(kernelMutex);
            
            vTaskDelete(handle);
            
            char msg[80];
            sprintf(msg, "[KERNEL] Killed process '%s' (PID: %d)", name, pid);
            printLine(msg);
            logKernelMessage(msg);
            
            return 0;
        }
    }
    
    xSemaphoreGive(kernelMutex);
    
    char msg[80];
    sprintf(msg, "[KERNEL] ERROR: Process PID %d not found", pid);
    printLine(msg);
    logKernelMessage(msg);
    
    return -1;
}

void listProcesses() {
    xSemaphoreTake(kernelMutex, portMAX_DELAY);
    
    if (processCount == 0) {
        printLine("[KERNEL] No processes running");
        logKernelMessage("[KERNEL] No processes running");
        xSemaphoreGive(kernelMutex);
        return;
    }
    
    printLine("");
    printLine("PROCESS LIST");
    printLine("----------------------------------");
    
    uint32_t now = millis();
    for (int i = 0; i < processCount; i++) {
        Process *proc = &processTable[i];
        
        const char* stateStr;
        switch (proc->state) {
            case PROC_RUNNING: stateStr = "RUN"; break;
            case PROC_READY: stateStr = "RDY"; break;
            case PROC_BLOCKED: stateStr = "BLK"; break;
            case PROC_SLEEPING: stateStr = "SLP"; break;
            case PROC_TERMINATED: stateStr = "END"; break;
            default: stateStr = "???"; break;
        }
        
        uint32_t uptime = now - proc->createdAt;
        uint32_t uptimeSec = uptime / 1000;
        
        char line[80];
        sprintf(line, "%d: %-12s P:%d %s %lus",
                proc->pid,
                proc->name,
                proc->priority,
                stateStr,
                uptimeSec);
        printLine(line);
    }
    
    printLine("----------------------------------");
    
    xSemaphoreGive(kernelMutex);
}

ProcessState getProcessState(int pid) {
    xSemaphoreTake(kernelMutex, portMAX_DELAY);
    
    for (int i = 0; i < processCount; i++) {
        if (processTable[i].pid == pid) {
            ProcessState state = processTable[i].state;
            xSemaphoreGive(kernelMutex);
            return state;
        }
    }
    
    xSemaphoreGive(kernelMutex);
    return PROC_TERMINATED;
}

uint32_t getProcessUptime(int pid) {
    xSemaphoreTake(kernelMutex, portMAX_DELAY);
    
    for (int i = 0; i < processCount; i++) {
        if (processTable[i].pid == pid) {
            uint32_t uptime = millis() - processTable[i].createdAt;
            xSemaphoreGive(kernelMutex);
            return uptime;
        }
    }
    
    xSemaphoreGive(kernelMutex);
    return 0;
}

uint32_t getSystemUptime() {
    return millis() - bootTime;
}

uint32_t getFreeMem() {
#if !defined(DEVICE_RP2350)
    return heap_caps_get_free_size(MALLOC_CAP_8BIT);
#else
    // return (uint32_t)xPortGetFreeHeapSize();
    return 0;
#endif
}

uint32_t getTotalMem() {
#if !defined(DEVICE_RP2350)
    return heap_caps_get_total_size(MALLOC_CAP_8BIT);
#else
    // return (uint32_t)configTOTAL_HEAP_SIZE;
    return 0;
#endif
}

float getCPUUsage() {
    uint32_t free = getFreeMem();
    uint32_t total = getTotalMem();
    return 100.0f * (1.0f - ((float)free / (float)total));
}

void printSystemStats() {
    uint32_t uptime = getSystemUptime();
    uint32_t uptimeSec = uptime / 1000;
    uint32_t uptimeMin = uptimeSec / 60;
    uint32_t uptimeHr = uptimeMin / 60;
    
    uint32_t freeMem = getFreeMem();
    uint32_t totalMem = getTotalMem();
    float cpuUsage = getCPUUsage();
    
    printLine("");
    printLine("==================================");
    printLine("     SYSTEM STATISTICS");
    printLine("==================================");
    
    char line[80];
    
    sprintf(line, "Uptime:    %02lud %02lu:%02lu:%02lu", 
            uptimeHr / 24, uptimeHr % 24, uptimeMin % 60, uptimeSec % 60);
    printLine(line);
    
    sprintf(line, "Free RAM:  %lu bytes", freeMem);
    printLine(line);
    
    sprintf(line, "Total RAM: %lu bytes", totalMem);
    printLine(line);
    
    sprintf(line, "CPU Usage: %.1f%%", cpuUsage);
    printLine(line);
    
    sprintf(line, "Processes: %d/%d", processCount, MAX_PROCESSES);
    printLine(line);
    
    printLine("==================================");
}

void signalProcess(int pid, int signal) {
    char msg[80];
    sprintf(msg, "Signal %d sent to PID %d", signal, pid);
    printLine(msg);
    logKernelMessage(msg);
}

void waitForProcess(int pid) {
    while (getProcessState(pid) != PROC_TERMINATED) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void kernelScheduler(void *parameter) {
    while (1) {
        xSemaphoreTake(kernelMutex, portMAX_DELAY);
        
        for (int i = 0; i < processCount; i++) {
            eTaskState taskState = eTaskGetState(processTable[i].handle);
            
            switch (taskState) {
                case eRunning:
                    processTable[i].state = PROC_RUNNING;
                    break;
                case eReady:
                    processTable[i].state = PROC_READY;
                    break;
                case eBlocked:
                    processTable[i].state = PROC_BLOCKED;
                    break;
                case eSuspended:
                    processTable[i].state = PROC_SLEEPING;
                    break;
                case eDeleted:
                    processTable[i].state = PROC_TERMINATED;
                    break;
                default:
                    break;
            }
        }
        
        xSemaphoreGive(kernelMutex);
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}