#include "network.h"
#include "display.h"
#include "kernel.h"
#include <string>

#if !defined(DEVICE_RP2350)
#include <ESP32Ping.h>
#include <esp_system.h>
#include <WiFi.h>
#endif

void pingHost(const std::string& host,int retries) {
#if !defined(DEVICE_RP2350)
    if (!isConnected()) {
        printLine("ping: not connected to WiFi");
        logKernelMessage("[NETWORK] ping: not connected to WiFi");
        return;
    }
    
    IPAddress ip;
    if (!WiFi.hostByName(host.c_str(), ip)) {
        printLine("ping: cannot resolve " + host);
        return;
    }
    
    printLine("Pinging " + std::string(ip.toString().c_str()) + "...");
    
    int sent = 0;
    int received = 0;
    float totalTime = 0;
    float minTime = 999999;
    float maxTime = 0;
    
    for (int i = 0; i < retries; i++) {
        sent++;
        
        bool success = Ping.ping(ip, 1);
        
        if (success) {
            received++;
            float time = Ping.averageTime();
            totalTime += time;
            
            if (time < minTime) minTime = time;
            if (time > maxTime) maxTime = time;
            
            printLine(std::to_string(i + 1) + ": Reply from " + std::string(ip.toString().c_str()) + 
                     " time=" + std::to_string(time) + "ms");
        } else {
            printLine(std::to_string(i + 1) + ": Request timeout");
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
    
    printLine("");
    printLine("--- " + host + " ping statistics ---");
    printLine(std::to_string(sent) + " packets sent, " + std::to_string(received) + " received, " +
             std::to_string(((sent - received) * 100) / sent) + "% packet loss");
    
    if (received > 0) {
        float avgTime = totalTime / received;
        printLine("Round-trip min/avg/max = " + 
                 std::to_string(minTime) + "/" + 
                 std::to_string(avgTime) + "/" + 
                 std::to_string(maxTime) + " ms");
    }
#else
    printLine("ping is not supported on this device.");
#endif
}