#include "display.h"


void showHelp() {
    printLine("MiniOS Command Help");
    printLine("");
    printLine("  help file     - File commands");
    printLine("  help system   - System commands");
#if !defined(DEVICE_RP2350)
    printLine("  help network  - Network commands");
#endif
    printLine("  help time     - Time commands");
    printLine("  help display  - Display commands");
    printLine("  help os       - OS management");
    printLine("  help utils    - Utility commands");
    printLine("  help misc     - Miscellaneous commands");
}

void showHelpFile() {
    printLine("File Commands:");
    printLine("  write <file> <text>   - Write text");
    printLine("  append <file> <text>  - Append text");
    printLine("  read <file>           - Read file");
    printLine("  delete <file>         - Delete file (alias: rm)");
    printLine("  ls                    - List files (alias: dir)");
    printLine("  mv <old> <new>        - Rename file (alias: rename)");
    printLine("  cp <src> <dst>        - Copy file (alias: copy)");
}

void showHelpSystem() {
    printLine("System Commands:");
    printLine("  mem      - Memory info (alias: free)");
    printLine("  dmesg    - System logs");
    printLine("  uptime   - System uptime");
    printLine("  reboot   - Restart device (alias: restart)");
    printLine("  fetch    - System info (alias: neofetch)");
    printLine("  name     - Change username(alias: username)");
    printLine("  os       - OS logo (alias: logo)");
    printLine("  version  - OS version (alias: ver)");
    printLine("  clear    - Clear display (alias: cls)");
    printLine("  history  - Command history (alias: hist)");
}

void showHelpNetwork() {
#if !defined(DEVICE_RP2350)
    printLine("Network Commands:");
    printLine("  wifi                - Connect to WiFi");
    printLine("  disconnect          - Disconnect WiFi");
    printLine("  scanwifi            - Scan networks");
    printLine("  ifconfig            - Network info");
    printLine("  ping <host> <tries> - Ping host");
    printLine("  nslookup <host>     - DNS lookup");
    printLine("  curl <url>          - Fetch URL");
    printLine("  curl -v <url>       - Verbose mode");
#else
    printLine("Network commands are not available on this device.");
#endif
}

void showHelpUtils() {
    printLine("Utility Commands:");
    printLine("  calc <expr>                 - Calculator");
    printLine("  hex <number>                - Dec to hex");
    printLine("  bin <number>                - Dec to bin");
    printLine("  base64 encode <text>        - Encode Base64");
    printLine("  base64 decode <text>        - Decode Base64");
    printLine("  graph <expression> <colour> - Graph function");
    printLine("  echo <text>                 - Print text");
}

void showHelpTime() {
    printLine("Time Commands:");
    printLine("  time            - Current time");
    printLine("  synctime        - Sync with NTP");
    printLine("  calendar        - Show calendar");
    printLine("  timer <sec>     - Countdown timer");
    printLine("  stopwatch       - Elapsed timer");
    printLine("  alarm <HH:MM>   - Set alarm");
}

void showHelpOS() {
    printLine("OS Commands:");
    printLine("  ps / processes - List processes");
    printLine("  sysstat / stat - System stats");
    printLine("  kill <pid>     - Kill process");
    printLine("  settings       - Settings menu");
}

void showHelpDisplay() {
    printLine("Display Commands:");
    printLine("  themes          - List themes");
    printLine("  theme <n>       - Select theme");
    printLine("  screensaver <n> - Run screensaver");
}

void showHelpMiscellaneous() {
    printLine("Miscellaneous Commands:");
    printLine("  ball <w> <n> <t> - Simulate balls");
    printLine("  pug              - Show pug image");
    printLine("  pong             - Ping-pong game");
    printLine("  d20              - Simulate D20 die");
    printLine("  coin             - Simulate coin flip");
}
