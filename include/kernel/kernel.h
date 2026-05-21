#ifndef KERNEL_H
#define KERNEL_H

#include <Arduino.h>
#if defined(DEVICE_RP2350) && !defined(__FREERTOS)
#define __FREERTOS 1
#endif
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <vector>
#include <string>

enum ProcessState {
    PROC_RUNNING,     
    PROC_READY,        
    PROC_BLOCKED,      
    PROC_SLEEPING,     
    PROC_TERMINATED  
};


struct Process {
    TaskHandle_t handle;
    const char* name;
    ProcessState state;
    UBaseType_t priority;
    uint32_t createdAt;      
    uint32_t stackSize;
    int pid;                 
};

#define MAX_PROCESSES 16
#define KERNEL_PRIORITY 3


void kernelInit();
void kernelScheduler(void *parameter);
void kernelShutdown();
extern std::vector<std::string> kernelMessages;
void logKernelMessage(const std::string& msg);

int createProcess(TaskFunction_t function, const char* name, uint32_t stackSize, 
                  UBaseType_t priority);
int killProcess(int pid);
void listProcesses();
ProcessState getProcessState(int pid);
uint32_t getProcessUptime(int pid);


uint32_t getSystemUptime();
uint32_t getFreeMem();
uint32_t getTotalMem();
float getCPUUsage();
void printSystemStats();


void signalProcess(int pid, int signal);
void waitForProcess(int pid);

#endif
