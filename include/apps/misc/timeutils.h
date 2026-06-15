#ifndef TIMEUTILS_H
#define TIMEUTILS_H


#include <Arduino.h>
#include <string>

void syncTime();
std::string getTime();
void showCalendar();
void timerCommand(int seconds);
void stopwatchCommand();
void setAlarm(const std::string& timeStr);
void checkAlarm();

struct Alarm {
    bool active;
    int hour;
    int minute;
    std::string message;
};
void alarmCheckProcess(void *parameter);
extern Alarm systemAlarm;

#endif