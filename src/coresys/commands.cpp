#include "commands.h"

#define HISTORY_SIZE 10  
std::string commandHistory[HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;





void showVersion() {
    printLine("MiniOS " + std::string(OS_VERSION));
    printLine("Repository: github.com/VuqarAhadli");
}


void addToHistory(const std::string& cmd) {
    if (cmd.length() == 0) return;
    if (historyCount > 0 && commandHistory[(historyIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE] == cmd) return;
    commandHistory[historyIndex] = cmd;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
    if (historyCount < HISTORY_SIZE) historyCount++;
}

void showHistory() {
    if (historyCount == 0) {
        printLine("No command history.");
        return;
    }
    printLine("Command history:");
    int start = (historyIndex - historyCount + HISTORY_SIZE) % HISTORY_SIZE;
    for (int i = 0; i < historyCount; i++) {
        int idx = (start + i) % HISTORY_SIZE;
        printLine(std::to_string(i + 1) + ": " + commandHistory[idx]);
    }
}




struct CommandArgs {
    std::string cmd;
    std::string arg1;
    std::string arg2;
    std::string rest;
};

CommandArgs parseCommand(const std::string& input_in) {
    CommandArgs args;
    std::string input = input_in;
    
    input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
    input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
    
    size_t firstSpace = input.find(' ');
    if (firstSpace == std::string::npos) {
        args.cmd = input;
        return args;
    }
    
    args.cmd = input.substr(0, firstSpace);
    std::string remainder = input.substr(firstSpace + 1);
    
    remainder.erase(0, remainder.find_first_not_of(" \t\n\r\f\v"));
    remainder.erase(remainder.find_last_not_of(" \t\n\r\f\v") + 1);
    
    size_t secondSpace = remainder.find(' ');
    if (secondSpace == std::string::npos) {
        args.arg1 = remainder;
        return args;
    }
    
    args.arg1 = remainder.substr(0, secondSpace);
    args.rest = remainder.substr(secondSpace + 1);
    args.rest.erase(0, args.rest.find_first_not_of(" \t\n\r\f\v"));
    args.rest.erase(args.rest.find_last_not_of(" \t\n\r\f\v") + 1);
    
    size_t thirdSpace = args.rest.find(' ');
    if (thirdSpace != std::string::npos) {
        args.arg2 = args.rest.substr(0, thirdSpace);
        args.rest = args.rest.substr(thirdSpace + 1);
        args.rest.erase(0, args.rest.find_first_not_of(" \t\n\r\f\v"));
        args.rest.erase(args.rest.find_last_not_of(" \t\n\r\f\v") + 1);
    } else if (args.rest.length() > 0) {
        args.arg2 = args.rest;
        args.rest = "";
    }
    
    return args;
}

void runCommand(const std::string& cmd_in) {
    std::string cmd = cmd_in;
    cmd.erase(0, cmd.find_first_not_of(" \t\n\r\f\v"));
    cmd.erase(cmd.find_last_not_of(" \t\n\r\f\v") + 1);
    
    if (cmd.length() == 0){
        if(currentCursorY>=MAX_Y){
            clearScreen();
            currentCursorY=0;
        }
        return;
    }
    
    addToHistory(cmd);
    
    CommandArgs args = parseCommand(cmd);
    std::string baseCmd = args.cmd;
    std::transform(baseCmd.begin(), baseCmd.end(), baseCmd.begin(), ::tolower);
    
    if (baseCmd == "write") {
        if (args.arg1.length() == 0 || args.arg2.length() == 0) {
            printLine("Usage: write <filename> <text>");
            return;
        }
        writeFile(args.arg1, args.arg2 + args.rest);
    }
    else if (baseCmd == "append") {
        if (args.arg1.length() == 0 || args.arg2.length() == 0) {
            printLine("Usage: append <filename> <text>");
            return;
        }
        appendFile(args.arg1, args.arg2 + args.rest);
    }
    else if (baseCmd == "read") {
        if (args.arg1.length() == 0) {
            printLine("Usage: read <filename>");
            return;
        }
        readFile(args.arg1);
    }
    else if (baseCmd == "delete" || baseCmd == "rm") {
        if (args.arg1.length() == 0) {
            printLine("Usage: delete <filename>");
            return;
        }
        deleteFile(args.arg1);
    }
    else if (baseCmd == "ls" || baseCmd == "dir") {
        listFiles();
    }
    else if (baseCmd == "mv" || baseCmd == "rename") {
        if (args.arg1.length() == 0 || args.arg2.length() == 0) {
            printLine("Usage: mv <old> <new>");
            return;
        }
        renameFile(args.arg1, args.arg2);
    }
    else if (baseCmd == "cp" || baseCmd == "copy") {
        if (args.arg1.length() == 0 || args.arg2.length() == 0) {
            printLine("Usage: cp <src> <dst>");
            return;
        }
        copyFile(args.arg1, args.arg2);
    }
    else if (baseCmd == "wifi") {
        if (args.arg1 == "disconnect") {
            disconnectWiFi();
        }
        else if (args.arg1 == "conf" || args.arg1 == "config"){
            connectWiFi(true);
        }
        else connectWiFi(false);
    }
    else if (baseCmd == "disconnect") {
        disconnectWiFi();
    }
    else if (baseCmd == "scanwifi" || baseCmd == "wifiscan") {
        scanWiFi();
    }
    else if (baseCmd == "ifconfig" || baseCmd == "netinfo" || baseCmd == "ipconfig") {
        showNetworkInfo();
    }
    else if (baseCmd == "curl") {
        if (args.arg1.length() == 0) {
            printLine("Usage: curl [-v] <url>");
            return;
        }
        
        if (args.arg1 == "-v") {
            if (args.arg2.length() == 0) {
                printLine("Usage: curl -v <url>");
                return;
            }
            curlURLVerbose(args.arg2);
        } else {
            curlURL(args.arg1);
        }
    }
    else if (baseCmd == "ping") {
        if (args.arg1.empty()) {
            printLine("Usage: ping <host> <tries>");
            return;
        }

        int defaultTries = 4;

        if (!args.arg2.empty()) {
            try {
                defaultTries = std::stoi(args.arg2);
            } catch (...) {
                printLine("Invalid number for tries, using default (4)");
            }
        }

        pingHost(args.arg1, defaultTries);
    }
    else if (baseCmd == "nslookup" || baseCmd == "dns") {
        if (args.arg1.length() == 0) {
            printLine("Usage: nslookup <host>");
            return;
        }
        dnsLookup(args.arg1);
    }
    else if (baseCmd == "mem" || baseCmd == "memory") {
        showMem();
    }
    else if (baseCmd == "uptime") {
        showUptime();
    }
    else if (baseCmd == "reboot" || baseCmd == "restart") {
        saveConfig();
        doReboot();
    }
    else if (baseCmd == "fetch" || baseCmd == "neofetch" || baseCmd == "fastfetch" ) {
        fetch();
    }
    else if (baseCmd == "os" || baseCmd == "logo") {
        showLogo();
    }
    else if (baseCmd == "version" || baseCmd == "ver") {
        showVersion();
    }
    else if (baseCmd == "clear" || baseCmd == "cls") {
        clearScreen();
        
    }
    else if (baseCmd == "history" || baseCmd == "hist") {
        showHistory();
    }
    else if (baseCmd == "ps" || baseCmd == "processes" || baseCmd == "top") {
        listProcesses();
    }
    else if (baseCmd == "sysstat" || baseCmd == "stat") {
        showSystemStats();
    }
    else if (baseCmd == "kill") {
        if (args.arg1.length() == 0) {
            printLine("Usage: kill <pid>");
            return;
        }
        int pid = 0;
        try {
            pid = std::stoi(args.arg1);
        } catch (...) {
            pid = 0;
        }
        if (pid <= 0) {
            printLine("Invalid PID");
            return;
        }
        killProcess(pid);
    }
    else if (baseCmd == "echo") {
        echoCommand(args.arg1 + (args.arg2.length() > 0 ? " " + args.arg2 : "") + (args.rest.length() > 0 ? " " + args.rest : ""));
    }
    else if (baseCmd == "calc") {
        if (args.arg1.length() == 0) {
            printLine("Usage: calc <expression>");
            return;
        }
        calc(args.arg1 + args.arg2 + args.rest);
    }
    else if (baseCmd == "hex") {
        if (args.arg1.length() == 0) {
            printLine("Usage: hex <number>");
            return;
        }
        hexCommand(args.arg1);
    }
    else if (baseCmd == "bin") {
        if (args.arg1.length() == 0) {
            printLine("Usage: bin <number>");
            return;
        }
        binCommand(args.arg1);
    }
    else if (baseCmd == "base64") {
        if (args.arg1.length() == 0) {
            printLine("Usage: base64 encode <text>");
            printLine("       base64 decode <text>");
            return;
        }
        
        std::string operation = args.arg1;
        std::string text = args.arg2;
        if (args.rest.length() > 0) {
            text = args.arg2 + " " + args.rest;
        }
        
        if (text.length() == 0) {
            printLine("Usage: base64 encode <text>");
            printLine("       base64 decode <text>");
            return;
        }
        
        base64Command(operation, text);
    }   
    else if (baseCmd == "time" || baseCmd == "date") {
        printLine(getTime());
    }
    else if (baseCmd == "synctime" || baseCmd == "ntpupdate") {
        syncTime();
    }
    else if (baseCmd == "calendar" || baseCmd == "cal") {
        showCalendar();
    }
    else if (baseCmd == "timer") {
        if (args.arg1.length() == 0) {
            printLine("Usage: timer <seconds>");
            return;
        }
        int seconds = 0;
        try {
            seconds = std::stoi(args.arg1);
        } catch (...) {
            seconds = 0;
        }
        if (seconds <= 0) {
            printLine("Invalid time");
            return;
        }
        timerCommand(seconds);
    }
    else if (baseCmd == "stopwatch" || baseCmd == "sw") {
        stopwatchCommand();
    }
    else if (baseCmd == "alarm") {
        if (args.arg1.length() == 0) {
            if (systemAlarm.active) {
                printLine("Alarm set for " + std::to_string(systemAlarm.hour) + ":" + 
                         (systemAlarm.minute < 10 ? "0" : "") + std::to_string(systemAlarm.minute));
            } else {
                printLine("No alarm set.");
            }
        } else {
            setAlarm(args.arg1);
        }
    }
    else if (baseCmd == "themes") {
        listThemes();
    }
    else if (baseCmd == "theme") {
        if (args.arg1.length() == 0) {
            printLine("Usage: theme <number>");
            listThemes();
            return;
        }
        setTheme(args.arg1);
    }
    // else if (baseCmd == "pug") {
    //     displayPug();
    // }
    else if (baseCmd == "screensaver" || baseCmd == "ss") {
        if (args.arg1.length() == 0) {
            printLine("Usage: screensaver <mode>");
            printLine("Available modes: 1-8");
            return;
        }
        
        int mode = std::stoi(args.arg1);
        if (mode < 1 || mode > 10) {
            printLine("Invalid mode. Use 1-9.");
            return;
        }
        
        screensaver(mode);
    }
    else if (baseCmd == "graph" || baseCmd == "plot") {
        if (args.arg1.length() == 0) {
            printLine("Usage: graph <expression> [color]");
            printLine("Example: graph sin(x) red");
            return;
        }
        std::string color = args.arg2.length() > 0 ? args.arg2 : "blue";
        funcToGraph(args.arg1, color);
    }
    else if (baseCmd == "username" || baseCmd == "name") {
        // if (args.arg1.length() == 0 || args.arg2.length() != 0) {
        //     printLine("Usage: username <name>");
        //     return;
        // }
        setDeviceName();
    }
    else if (baseCmd == "help" || baseCmd == "h") {
        if (args.arg1.length() == 0) {
            showHelp();
        } else if (args.arg1 == "file") {
            showHelpFile();
        } else if (args.arg1 == "system") {
            showHelpSystem();
        } else if (args.arg1 == "network") {
            showHelpNetwork();
        } else if (args.arg1 == "utils") {
            showHelpUtils();
        } else if (args.arg1 == "time") {
            showHelpTime();
        } else if (args.arg1 == "display") {
            showHelpDisplay();
        } else if (args.arg1 == "os") {
            showHelpOS();
        } else if (args.arg1 == "misc") {
            showHelpMiscellaneous();
        } else {
            printLine("Unknown help topic: " + args.arg1);
            showHelp();
        }
    }
    else if (baseCmd == "ball") {
        bool trail = false;
        int numBalls = 1;

        if (args.arg1.length() == 0) {
            initialiseBall(10, trail, numBalls);
        }
        else {
            try {
                int radius = std::stoi(args.arg1);

               

                if (args.arg2.length() != 0) {
                    try {
                        numBalls = std::stoi(args.arg2);
                        if (numBalls < 1) {
                            printLine("Error: number of balls must be at least 1.");
                            return;
                        }
                    }
                    catch (const std::invalid_argument&) {
                        printLine("Error: number of balls must be a positive integer.");
                        return;
                    }
                }
                if (args.rest.length() != 0) {
                    if (args.rest == "trail") {
                        trail = true;
                    }
                }
                if (radius <= 0) {
                    printLine("Error: ball radius must be a positive integer.");
                }
                else if (radius > 115) {
                    printLine("Error: ball radius must be smaller than 116 pixels.");
                }
                else {
                    initialiseBall(radius, trail, numBalls);
                }
            }
            catch (const std::invalid_argument&) {
                printLine("Error: invalid radius, must be a number.");
            }
        }
    }
    // else if (baseCmd == "mirror") {
    //     startMirror();
    // }
    else if (baseCmd == "dmesg" || baseCmd == "log")
    {
        for(const std::string& i : kernelMessages){
            printLine(i);
        }
    }
    else if (baseCmd == "pong" || baseCmd == "pingpong")
    {
        pingpongGame();
    }
    else if (baseCmd == "d20" || baseCmd == "dice")
    {
        d20Game();
    }
    else if (baseCmd == "coin" || baseCmd == "coinflip" || baseCmd == "flip" )
    {
        coinGame();
    }
    else if (baseCmd == "settings" || baseCmd == "config")
    {
        showSettings();
    }

    else if (baseCmd == "oslogo" || baseCmd == "logo")
    {
        drawScreen();
    }

    
    else {
        printLine("Unknown command: " + baseCmd);
        printLine("Type 'help' for available commands");
    }
}




void processCommand(String args) {
    listProcesses();
}

void showSystemStats() {
    printSystemStats();
}

void killProcessCmd(int pid) {
    killProcess(pid);
}
