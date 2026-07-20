# MiniOS-ESP Documentation

[![membrowse](https://membrowse.com/badge.svg)](https://membrowse.com/public/VuqarAhadli/MiniOS-ESP)
![PlatformIO](https://img.shields.io/badge/PlatformIO-supported-brightgreen?logo=platformio)
![Arduino](https://img.shields.io/badge/Arduino-supported-success?logo=arduino)
![C++](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-yes-orange?logo=freertos)
![Version](https://img.shields.io/github/v/release/VuqarAhadli/MiniOS-ESP)


## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Hardware Setup](#hardware-setup)
4. [Software Components](#software-components)
5. [Command Reference](#command-reference)
6. [Development Guide](#development-guide)
7. [Troubleshooting](#troubleshooting)

---

## Overview

MiniOS-ESP is a lightweight, Unix-like command-line operating system designed for ESP32 microcontrollers with ST7789 display support. It provides a full-featured shell environment with file management, networking capabilities, time synchronization, and process management built on FreeRTOS.

### Key Features

- **FreeRTOS-based kernel** with process management
- **SPIFFS filesystem** for persistent storage
- **WiFi connectivity** with network utilities
- **NTP time synchronization** and alarm system
- **Mathematical calculator** with advanced functions and graphing
- **Customizable themes** and screensavers
- **Serial command interface** for interactive control
- **Multi-process architecture** with scheduler
- **Unix-like command aliases** for familiar shell experience

### Version
Current Version: **MiniOS-ESP v2.0.4**

---

## Architecture

### System Architecture

```
┌─────────────────────────────────────────┐
│         User Interface Layer            │
│  (Serial Terminal + TFT Display)        │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│         Command Shell Layer             │
│  (Command Parser & History)             │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│      Application Services Layer         │
│  • File System  • Network Stack         │
│  • Time Utils   • Calculator            │
│  • Themes       • Display Manager       │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│         Kernel Layer (FreeRTOS)         │
│  • Process Management                   │
│  • Task Scheduling                      │
│  • Memory Management                    │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│         Hardware Abstraction            │
│  • ESP32 HAL  • Driver Layer            │
└─────────────────────────────────────────┘
```

### Process Model

MiniOS runs five core processes:

| Process | Priority | Stack | Description |
|---------|----------|-------|-------------|
| `init` | 1 | 4096 | System initialization |
| `shell` | 2 | 16384 | Command interpreter |
| `alarm` | 1 | 1024 | Time-based alarms |
| `watchdog` | 0 | 1024 | System monitoring |
| `scheduler` | 3 | 2048 | Process state management |

---

## Hardware Setup

### Required Components

- **ESP32 Development Board** (ESP32-WROOM-32 or compatible)
- **ST7789 TFT Display** (240×320 pixels)
- **USB Cable** for programming and serial communication
- **Power Supply** (5V via USB or 3.3V regulated)

### Pin Configuration

| Signal | ESP32 GPIO | Description |
|--------|------------|-------------|
| TFT_CS | GPIO 5 | Chip Select |
| TFT_DC | GPIO 2 | Data/Command |
| TFT_RST | GPIO 4 | Reset |
| TFT_SCK | GPIO 18 | SPI Clock |
| TFT_SDI | GPIO 23 | SPI MOSI |
| TFT_BLK | 3.3V | Backlight (always on) |

### Wiring Diagram

```
ESP32              ST7789 Display
┌──────┐          ┌──────────┐
│ GPIO5├──────────┤ CS       │
│ GPIO2├──────────┤ DC       │
│ GPIO4├──────────┤ RST      │
│GPIO18├──────────┤ SCK      │
│GPIO23├──────────┤ SDA      │
│ 3.3V ├──────────┤ BLK      │
│ 3.3V ├──────────┤ VCC      │
│  GND ├──────────┤ GND      │
└──────┘          └──────────┘
```

### Physical Setup Tips

1. Use short wires (< 15cm) for stable SPI communication
2. Add a 10µF capacitor between VCC and GND near the display (optional)
3. Ensure solid GND connection between ESP32 and display
4. Mount display securely to prevent connector damage

---

## Software Components

### Core Modules

#### 1. Kernel (`kernel.cpp`)

Manages process lifecycle and system resources using FreeRTOS.

**Key Functions:**
- `kernelInit()` - Initialize kernel and mutex
- `createProcess()` - Spawn new process
- `killProcess()` - Terminate process by PID
- `listProcesses()` - Display running processes
- `printSystemStats()` - Show system statistics

**Process States:**
- `PROC_RUNNING` - Currently executing
- `PROC_READY` - Ready to run
- `PROC_BLOCKED` - Waiting for resource
- `PROC_SLEEPING` - Suspended
- `PROC_TERMINATED` - Finished execution

#### 2. Display (`display.cpp`)

Handles TFT display operations and rendering.

**Key Functions:**
- `initDisplay()` - Initialize ST7789 driver
- `printLine()` - Print text to display and serial
- `clearScreen()` - Clear display and reset cursor
- `screensaver()` - Run animated screensaver
- `showLogo()` - Display MiniOS ASCII logo

**Screensaver Modes:**
> **Experimental feature.** Screensaver modes may cause input latency.

1. **Wave Patterns** - Sinusoidal color waves
2. **Rainbow** - Multi-hue gradient animation
3. **Spiral** - Polar coordinate transformation
4. **Matrix** - Falling character columns            
5. **Fire** - Heat gradient simulation
6. **Starfield** - Twinkling stars
7. **Tunnel** - Perspective effect

#### 3. Filesystem (`filesystem.cpp`)

Provides SPIFFS-based persistent storage.

**Key Functions:**
- `initFilesystem()` - Mount SPIFFS partition
- `writeFile()` - Create/overwrite file
- `appendFile()` - Append to existing file
- `readFile()` - Display file contents
- `deleteFile()` - Remove file
- `listFiles()` - Show all files with sizes
- `renameFile()` - Change filename
- `copyFile()` - Duplicate file

**Storage Limits:**
- Partition size: ~1.5 MB (configurable in `platformio.ini`)
- Max filename length: 31 characters
- Max open files: 5 simultaneous

**Important:** Systems upgrading from v1.x to v2.0.3+ must reformat SPIFFS due to partition layout changes. Use `SPIFFS.format()` or perform a full flash erase.

#### 4. Network (`network.cpp`)

Manages WiFi connectivity and HTTP operations.

**Key Functions:**
- `connectWiFi()` - Interactive WiFi connection
- `scanWiFi()` - Scan for available networks
- `curlURL()` - HTTP GET request with headers
- `pingHost()` - ICMP echo request with statistics

**Network Features:**
- WPA/WPA2 security support
- DHCP client
- DNS resolution
- HTTP/1.1 protocol with redirect following
- Automatic binary content detection
- Response truncation for large payloads (1500 bytes)
- 10-second request timeout
- Verbose mode for detailed debugging

#### 5. Time Utilities (`timeutils.cpp`)

Synchronizes time and manages alarms.

**Key Functions:**
- `syncTime()` - NTP synchronization
- `getTime()` - Formatted timestamp
- `showCalendar()` - Monthly calendar view
- `timerCommand()` - Countdown timer
- `stopwatchCommand()` - Elapsed time measurement
- `setAlarm()` - Schedule one-time alarm

**Time Configuration:**
- NTP Server: `pool.ntp.org`
- Timezone: GMT+4 (configurable in `config.h`)
- Format: `YYYY-MM-DD HH:MM:SS`

#### 6. Calculator (`commands.cpp`)

Advanced mathematical expression evaluator with graphing capabilities.

**Supported Operations:**
- Arithmetic: `+`, `-`, `*`, `/`, `%`, `^`
- Functions: `sqrt`, `sin`, `cos`, `tan`, `log`, `ln`, `exp`, `abs`
- Hyperbolic: `sinh`, `cosh`, `tanh`
- Inverse trig: `asin`, `acos`, `atan`
- Rounding: `ceil`, `floor`, `round`
- Constants: `pi` (3.14159...), `e` (2.71828...)

**Expression Rules:**
- Standard operator precedence
- Right-associative exponentiation
- Nested parentheses supported
- Unary minus handling
- Floating-point results

**Graphing:**
- Function plotting over range x,y ∈ [-10,10]
- Color-coded visualization
- Support for all mathematical functions

#### 7. Theme System (`theme.cpp`)

Customizable color schemes for display.

**Available Themes:**
```cpp
0: matrix      - Green on black (default)
1: cyan        - Cyan on black
2: classic     - White on black
3: blue        - Yellow on blue
4: light       - Black on white
5: dark-orange - Orange on dark
6: purple      - Magenta on dark
7: red-night   - Red on black
```

**Theme Structure:**
```cpp
struct Theme {
    uint16_t bg;    // Background color (RGB565)
    uint16_t fg;    // Foreground color (RGB565)
    String name;    // Theme identifier
};
```

---

## Command Reference

### Command Aliases

MiniOS supports Unix-like command aliases for improved usability:

| Alias | Command | Alias | Command |
|-------|---------|-------|---------|
| `rm` | `delete` | `dir` | `ls` |
| `free` | `mem` | `restart` | `reboot` |
| `neofetch`, `fastfetch` | `fetch` | `wifiscan` | `scanwifi` |
| `ipconfig`, `ifconfig` | Network info | `dns`, `nslookup` | DNS lookup |
| `date` | `time` | `ntpupdate` | `synctime` |
| `cal` | `calendar` | `sw` | `stopwatch` |
| `ss` | `screensaver` | `plot` | `graph` |
| `top` | `ps` | `h` | `help` |

**Note:** Commands are case-insensitive (e.g., `PING`, `ping`, `Ping` all work).

### System Commands

#### `help`
Display command categories or specific help.

**Syntax:**
```
help [category]
```

**Categories:**
- `file` - File operations
- `system` - System information
- `network` - Network utilities
- `utils` - Utility tools
- `time` - Time commands
- `display` - Display settings
- `os` - Process management

**Example:**
```
> help network
Network Commands:

  wifi          - Connect WiFi
  scanwifi      - Scan networks
  curl <url>    - Fetch content
  ping <host>   - Ping host
  dns <host>    - DNS lookup
  ifconfig      - Network info
```

#### `version`
Show MiniOS version and repository.

**Output:**
```
MiniOS MiniOS-ESP v2.0.4
Repository: github.com/VuqarAhadli
```

#### `fetch` / `neofetch` / `fastfetch`
Display comprehensive system information.

**Information Shown:**
- OS logo
- OS version
- System uptime
- Memory statistics
- Chip model and revision
- Flash size and speed
- CPU frequency
- WiFi status and signal
- Color palette visualization

#### `os`
Display MiniOS ASCII art logo.

#### `clear` / `cls`
Clear display and reset cursor.

**Note:** Display automatically clears when output reaches the bottom to prevent overflow.

#### `mem` / `free`
Show memory statistics.

**Output Format:**
```
Free Heap: 245632 bytes (239.88 KB)
Min Free Heap: 238208 bytes (232.62 KB)
Max Alloc Heap: 110592 bytes (108.00 KB)
```

#### `uptime`
Display time since boot.

**Format:** `Xh Ym Zs`

#### `reboot` / `restart`
Restart ESP32 device.

**Warning:** All unsaved data will be lost.

#### `history`
Show command history (last 10 commands).

**Output:**
```
Command history:
1: ls
2: read test.txt
3: calc 2 + 2
...
```

#### `username <name>`
Set local shell identity (session-only, non-persistent).

**Example:**
```
> username Alice
Username set to: Alice
```

---

### File System Commands

#### `write <filename> <text>`
Create or overwrite file with text.

**Examples:**
```
> write notes.txt Hello World
Written 11 bytes.

> write /data/log.txt System started
Written 14 bytes.
```

**Notes:**
- Leading `/` is optional
- Existing files are overwritten
- Maximum file size: ~1.5 MB

#### `append <filename> <text>`
Append text to existing file.

**Example:**
```
> append notes.txt More text
Appended 9 bytes.
```

#### `read <filename>`
Display file contents.

**Example:**
```
> read notes.txt
File: /notes.txt
Hello World
More text
```

#### `delete <filename>` / `rm <filename>`
Remove file from filesystem.

**Example:**
```
> delete notes.txt
File deleted.
```

#### `ls` / `dir`
List all files with sizes.

**Output:**
```
SPIFFS: 1024/1572864 bytes
Files:
  notes.txt - 20 bytes
  config.json - 153 bytes
```

#### `mv <oldname> <newname>`
Rename file.

**Example:**
```
> mv old.txt new.txt
Renamed.
```

#### `cp <source> <destination>`
Copy file.

**Example:**
```
> cp original.txt backup.txt
Copied.
```

---

### Network Commands

#### `wifi`
Interactive WiFi connection wizard.

**Flow:**
1. Prompts for SSID
2. Prompts for password (hidden with asterisks)
3. Attempts connection (20 attempts)
4. Displays IP address on success
5. Automatically syncs time

**Example Session:**
```
> wifi
Enter SSID: MyNetwork
Enter Password: ********
Connecting to: MyNetwork
..................
Connected!
IP: 192.168.1.42
Syncing time...
Time synced!
2025-01-15  14:30:45
```

#### `scanwifi` / `wifiscan`
Scan for nearby WiFi networks.

**Output:**
```
Scanning WiFi...
0: MyNetwork (-45 dBm) Secured
1: Guest_WiFi (-67 dBm) Open
2: Office5G (-72 dBm) Secured
```

**Information:**
- Network index
- SSID
- Signal strength (dBm)
- Security status

#### `curl <url> [-v]`
Fetch web content via HTTP GET.

**Features:**
- Displays HTTP status code
- Shows response headers
- Prints response body (truncated at 1500 bytes)
- Measures request duration
- Automatic redirect following
- Binary content detection
- Verbose mode with `-v` flag for detailed debugging

**Example:**
```
> curl http://example.com
HTTP/1.1 200
--- Headers ---
Content-Type: text/html
Content-Length: 1256
--- Info ---
Content-Length: 1256
Time: 342ms
--- Body (1256 bytes) ---
<!DOCTYPE html>
<html>...
```

**Verbose Mode:**
```
> curl -v http://example.com
[Detailed connection information]
...
```

**Supported Protocols:** HTTP only 

#### `ping <host>`
Send ICMP echo requests with statistics.

**Example:**
```
> ping 8.8.8.8
Pinging 8.8.8.8...
Ping statistics:
Min: 21 ms
Avg: 23 ms
Max: 27 ms
Packet loss: 0%
```

**Parameters:**
- 3 ping attempts
- Displays min/average/max round-trip time
- Shows packet loss percentage
- Improved timeout handling

#### `dns <host>` / `nslookup <host>`
Perform DNS lookup for hostname.

**Example:**
```
> dns google.com
Resolving: google.com
IP: 142.250.185.46
```

#### `ifconfig` / `ipconfig`
Display network interface information.

**Output:**
```
Network Information:
IP Address: 192.168.1.42
Subnet Mask: 255.255.255.0
Gateway: 192.168.1.1
DNS: 8.8.8.8
Signal: -45 dBm
```

---

### Time Commands

#### `time` / `date`
Display current date and time.

**Format:** `YYYY-MM-DD HH:MM:SS`

**Example:**
```
> time
2025-01-15  14:30:45
```

**Requirement:** Time must be synchronized automatically via `wifi` (or manually via the deprecated `synctime` command if necessary).

#### `synctime` / `ntpupdate`
> **Deprecated.** Time synchronization is handled automatically by the `wifi` command.  
> This command remains available only for debugging or recovery.

Synchronizes the system time with an NTP server.

**Example:**
```
> synctime
Syncing time...
..........
Time synced!
2025-01-15  14:30:45
```

**Configuration:**
- NTP Server: `pool.ntp.org`
- Timezone offset: GMT+4 (configurable)
- Max attempts: 20

#### `calendar` / `cal`
Display monthly calendar.

**Example:**
```
> calendar
January 2025
Mo Tu We Th Fr Sa Su
       1  2  3  4  5
 6  7  8  9 10 11 12
13 14 [15] 16 17 18 19
20 21 22 23 24 25 26
27 28 29 30 31
```

**Features:**
- Current day highlighted in brackets
- Starts week on Monday
- Handles leap years

#### `timer <seconds>`
Start countdown timer.

**Example:**
```
> timer 10
Timer started for 10 seconds.
Press ENTER to cancel...
10 seconds remaining...
9 seconds remaining...
...
Timer finished!
BEEP!
BEEP!
BEEP!
```

**Controls:**
- Press ENTER to cancel early

#### `stopwatch` / `sw`
Start elapsed time measurement.

**Example:**
```
> stopwatch
Stopwatch started.
Press ENTER to stop...
0h 0m 5s
0h 0m 6s
...
Stopped at: 0h 1m 23s 456ms
```

**Features:**
- Updates every second on display
- Millisecond precision on stop

#### `alarm <HH:MM> [message]`
Set one-time alarm.

**Examples:**
```
> alarm 14:30
Alarm set for 14:30

> alarm 14:30 Meeting time
Alarm set for 14:30
Message: Meeting time
```

**Notes:**
- 24-hour format required
- Optional custom message
- Only one alarm active at a time
- Alarm triggers once then deactivates

**Check Alarm:**
```
> alarm
Alarm set for 14:30
```

---

### Utility Commands

#### `calc <expression>`
Evaluate mathematical expressions.

**Basic Operations:**
```
> calc 2 + 2
Result: 4

> calc 15 * 3 - 7
Result: 38

> calc 2^10
Result: 1024
```

**Functions:**
```
> calc sqrt(144)
Result: 12

> calc sin(pi/2)
Result: 1.000000

> calc log(100)
Result: 2.000000
```

**Complex Expressions:**
```
> calc (5 + 3) * 2^3 / 4
Result: 16

> calc sqrt(2) * cos(pi/4)
Result: 1.000000
```

**Operators (Precedence):**
1. `^` - Exponentiation (right-associative)
2. `*`, `/`, `%` - Multiplication, division, modulo
3. `+`, `-` - Addition, subtraction

**Constants:**
- `pi` = 3.14159265358979
- `e` = 2.71828182845905

**Functions:**
| Category | Functions |
|----------|-----------|
| Trigonometric | sin, cos, tan, asin, acos, atan |
| Hyperbolic | sinh, cosh, tanh |
| Logarithmic | log (base 10), ln (natural) |
| Exponential | exp, sqrt |
| Rounding | abs, ceil, floor, round |

#### `graph <expression> [colour]` / `plot <expression> [colour]`

Graph a mathematical function of `x`.

**Rules:**

* One variable: `x`
* No spaces in expression
* No `=` sign
* Range: `x,y ∈ [-10,10]`
* Colour is optional (defaults to blue)

**Supported Functions:**

| Category       | Functions                       |
| -------------- | ------------------------------- |
| Trigonometric  | sin, cos, tan, asin, acos, atan |
| Hyperbolic     | sinh, cosh, tanh                |
| Logarithmic    | log (base 10), ln (natural)     |
| Exponential    | exp, sqrt                       |
| Rounding/Other | abs, ceil, floor, round         |

**Supported Colours:**

| Colour  |
| ------- |
| red     |
| green   |
| blue    |
| black   |
| yellow  |
| cyan    |
| magenta |
| orange  |
| purple  |

**Example:**

```
> graph sin(x) red
```

**Complex Example:**

```
> graph e^(sin(x)+sin(2*x))/(1+x^2) cyan
```

**Exit:** Press `ENTER` to return.

#### `hex <number>`
Convert decimal to hexadecimal.

**Example:**
```
> hex 255
Decimal: 255
Hexadecimal: 0xFF
```

#### `bin <number>`
Convert decimal to binary.

**Example:**
```
> bin 42
Decimal: 42
Binary: 0b101010
```

#### `base64 encode <text>`
Encode text to Base64.

**Example:**
```
> base64 encode Hello World
Encoded: SGVsbG8gV29ybGQ=
```

#### `base64 decode <text>`
Decode Base64 text.

**Example:**
```
> base64 decode SGVsbG8gV29ybGQ=
Decoded: Hello World
```

#### `echo <text>`
Print text to display.

**Example:**
```
> echo Hello, MiniOS!
Hello, MiniOS!
```

---

### Display Commands

#### `themes`
List available color themes.

**Output:**
```
Available themes:
0: matrix *
1: cyan
2: classic
3: blue
4: light
5: dark-orange
6: purple
7: red-night
```

**Note:** Asterisk (*) indicates current theme.

#### `theme <name|number>`
Change display theme.

**Examples:**
```
> theme 2
Theme set: classic

> theme purple
Theme set: purple
```

#### `screensaver <mode>` / `ss <mode>`
Run animated screensaver.
> **Experimental feature.** Screensaver modes may cause input latency.

**Modes:**
1. **Wave Patterns** - Sinusoidal cyan/blue waves
2. **Rainbow** - Flowing rainbow gradient
3. **Spiral** - Polar coordinate animation
4. **Matrix** - Falling green characters
5. **Fire** - Heat gradient simulation
6. **Starfield** - Twinkling stars
7. **Tunnel** - Perspective warp effect

**Example:**
```
> screensaver 4
[Displays Matrix animation]
Press ENTER to exit...
```

**Controls:**
- Press ENTER to exit
- Runs at ~20 FPS

---

### OS Management Commands

#### `ps` / `processes` / `top`
List running processes.

**Output:**
```
PROCESS LIST
----------------------------------
1: init         P:1 RDY 120s
2: shell        P:2 RUN 120s
3: alarm        P:1 RDY 120s
4: watchdog     P:0 SLP 120s
5: scheduler    P:3 RUN 120s
----------------------------------
```

**Columns:**
- PID: Process ID
- Name: Process name (max 12 chars)
- P: Priority (0-3, higher = more important)
- State: RUN, RDY, BLK, SLP, END
- Uptime: Time since creation

#### `sysstat` / `stat`
Show detailed system statistics.

**Output:**
```
==================================
     SYSTEM STATISTICS
==================================
Uptime:    00d 02:15:34
Free RAM:  245632 bytes
Total RAM: 327680 bytes
CPU Usage: 25.1%
Processes: 5/16
==================================
```

#### `kill <pid>`
Terminate process by ID.

**Example:**
```
> kill 3
Killed process 'alarm' (PID: 3)
```

**Warning:** 
- Cannot kill critical system processes (shell, scheduler)
- Killed processes cannot be recovered

---

## Development Guide

### Build Environment

#### Prerequisites

- **PlatformIO** (recommended) or **Arduino IDE**
- **ESP32 Board Package** (v2.0.0+)
- **Python 3.7+** (for PlatformIO)

#### PlatformIO Setup

1. Install PlatformIO:
```bash
pip install platformio
```

2. Clone repository:
```bash
git clone https://github.com/VuqarAhadli/MiniOS-ESP.git
cd MiniOS-ESP
```

3. Build project:
```bash
pio run
```

4. Upload to ESP32:
```bash
pio run --target upload
```

5. Monitor serial:
```bash
pio device monitor --baud 115200
```

#### Arduino IDE Setup

1. Install ESP32 board support:
   - File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - Tools → Board → Board Manager
   - Search "esp32" and install

2. Install libraries:
   - Adafruit GFX Library
   - Adafruit ST7789 Library
   - ESP32Ping

3. Configure board:
   - Tools → Board → ESP32 Dev Module
   - Tools → Flash Size → 4MB
   - Tools → Partition Scheme → Default 4MB with SPIFFS

4. Open `src/main.cpp` and upload

### Project Structure

```
MiniOS-ESP/
├── src/                    # Source code
│   ├── main.cpp           # Entry point and FreeRTOS setup
│   ├── commands.cpp       # Command implementations
│   ├── display.cpp        # TFT display driver
│   ├── filesystem.cpp     # SPIFFS operations
│   ├── network.cpp        # WiFi and HTTP
│   ├── theme.cpp          # Theme management
│   ├── timeutils.cpp      # Time and alarm functions
│   ├── kernel.cpp         # Process management
│   └── config.cpp         # Configuration 
│
├── include/               # Header files
│   ├── commands.h
│   ├── display.h
│   ├── filesystem.h
│   ├── network.h
│   ├── theme.h
│   ├── timeutils.h
│   ├── kernel.h
│   └── config.h          # Configuration constants
│
├── lib/                  # External libraries
│   └── Adafruit_ST7789/
│
├── platformio.ini        # Build configuration
├── README.md
└── .gitignore
```

### Configuration

#### `config.h` & `config.cpp` - System Configuration

```cpp
// Network
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET = 4 * 3600;  // GMT+4
const int DAYLIGHT_OFFSET = 0;

// Display
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
#define MAX_Y    230  // Text area height

// System
const char* OS_VERSION = "MiniOS-ESP v2.0.4";
#define HISTORY_SIZE 10  // Command history entries
```

#### `platformio.ini` - Build Configuration

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

lib_deps = 
    adafruit/Adafruit GFX Library
    adafruit/Adafruit ST7789
    marian-craciunescu/ESP32Ping

board_build.partitions = min_spiffs.csv
```

### Adding New Commands

1. **Declare command function** in `commands.h`:
```cpp
void myNewCommand(std::string arg);
```

2. **Implement function** in `commands.cpp`:
```cpp
void myNewCommand(std::string arg) {
    printLine("Executing: " + arg);
    // Your command logic here
}
```

3. **Add parser** in `runCommand()`:
```cpp
// Commands are case-insensitive
std::string cmd_lower = cmd;
std::transform(cmd_lower.begin(), cmd_lower.end(), cmd_lower.begin(), ::tolower);

if (cmd_lower.find("mynew ") == 0) {
    myNewCommand(cmd.substr(6));
}
else if (cmd_lower == "mynew") {
    printLine("Usage: mynew <argument>");
}
```

4. **Add help entry** in `showHelp()`:
```cpp
printLine("  mynew <arg>   - My new command");
```

5. **Add aliases (optional)**:
```cpp
else if (cmd_lower == "mn") {  // alias for mynew
    cmd_lower = "mynew";
}
```

**Important:** As of v2.0.3, all command handlers must accept `std::string` inputs instead of `String`. Avoid dynamic allocation inside tight loops and use `.c_str()` only at hardware or serial output boundaries.

### Creating New Themes

Add theme to `theme.cpp`:

```cpp
Theme themes[] = {
    // ... existing themes ...
    {0x0000, 0xFFFF, "my-theme"}  // bg, fg, name
};
```

**RGB565 Color Calculator:**
```cpp
uint16_t color = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
```

### Memory Management

**Heap Monitoring:**
```cpp
uint32_t free = ESP.getFreeHeap();
uint32_t min = ESP.getMinFreeHeap();
uint32_t max = ESP.getMaxAllocHeap();
```

**Best Practices:**
- Use stack for temporary data
- Free dynamically allocated memory
- Monitor heap fragmentation
- Use `std::string` instead of Arduino `String` (reduces fragmentation)
- Prefer fixed-size buffers for critical paths

**FreeRTOS Task Guidelines:**
- Minimum stack size: 1024 bytes
- Shell process: 16384 bytes (increased in v2.0.1 to prevent stack overflow)
- Always call `vTaskDelay()` in loops
- Use mutexes for shared resources

### Debugging

**Serial Debug Output:**
```cpp
Serial.println("Debug: " + std::to_string(value));
```

**Memory Tracking:**
```cpp
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

**Task Monitoring:**
```cpp
UBaseType_t stackHighWater = uxTaskGetStackHighWaterMark(NULL);
Serial.printf("Stack remaining: %d bytes\n", stackHighWater);
```

**Common Debug Pins:**
- Use GPIO 12-15 for logic analyzer
- Avoid GPIO 0, 2, 15 (boot strapping)

---

## Troubleshooting

### Display Issues

#### Screen is blank/white

**Causes:**
- Incorrect wiring
- Wrong pin configuration
- Insufficient power

**Solutions:**
1. Verify pin connections match `config.h`
2. Check power supply (3.3V, sufficient current)
3. Test with example sketch from Adafruit library
4. Reduce `TFT_BLK` brightness or control via PWM

#### Colors are wrong

**Solutions:**
- Try `tft.invertDisplay(true)` in `initDisplay()`
- Verify ST7789 initialization parameters
- Check if display is BGR or RGB model

#### Text is garbled

**Solutions:**
- Reduce SPI speed: `tft.setSPISpeed(40000000)` to `20000000`
- Check for loose connections
- Add decoupling capacitor

#### Screen overflow / duplicated prompts

**Fixed in v2.0.2:** Display now automatically clears when output reaches the bottom. If you experience issues:
- Ensure you're running v2.0.2 or later
- Check cursor positioning logic in your custom commands

### WiFi Problems

#### Cannot connect

**Diagnostics:**
1. Run `scanwifi` to verify network visibility
2. Check SSID and password for typos
3. Verify router supports 2.4GHz (ESP32 doesn't support 5GHz)
4. Check WiFi security type (WPA/WPA2 supported, WPA3 may fail)

**Solutions:**
- Ensure router is within range
- Temporarily disable MAC filtering
- Check ESP32 antenna orientation
- Try connecting to phone hotspot for testing

#### Connection drops

**Solutions:**
- Reduce power-saving: `WiFi.setSleep(false)`
- Check router stability
- Increase signal strength
- Verify power supply stability

#### Visual rendering issues (v2.0.4)

**Fixed in v2.0.4:** WiFi function visual rendering issue resolved. Update to latest version if experiencing display problems during WiFi connection.

### Time Sync Failures

#### Time not syncing

**Requirements:**
- Active WiFi connection
- Internet access
- UDP port 123 open

**Solutions:**
1. Verify WiFi: `WiFi.status() == WL_CONNECTED`
2. Test NTP server: `ping pool.ntp.org`
3. Try alternative server: Edit `NTP_SERVER` in `config.h`
4. Check firewall settings

### Filesystem Errors

#### "SPIFFS Failed"

**Causes:**
- Partition not formatted
- Corrupted filesystem
- Flash memory failure
- Incompatible partition layout (when upgrading from v1.x)

**Solutions:**
1. Format SPIFFS:
```cpp
SPIFFS.format();
```

2. Re-upload filesystem:
```bash
pio run --target uploadfs
```

3. Check partition scheme in `platformio.ini`

4. **For v2.0.3+ upgrades:** Perform full flash erase if upgrading from v1.x

#### File operations fail

**Solutions:**
- Check available space: `SPIFFS.totalBytes() - SPIFFS.usedBytes()`
- Verify filename (no illegal characters)
- Close files after operations
- Limit to 5 open files simultaneously

**v2.0.3 improvements:** File write and append operations now have more predictable behavior and improved error handling when target files do not exist.

### Memory Issues

#### Heap overflow / Crash

**Symptoms:**
- System reboots unexpectedly
- "Guru Meditation Error"
- Stack overflow messages

**Solutions:**
1. Check free heap: `ESP.getFreeHeap()`
2. Increase task stack sizes in `main.cpp`
3. Use `std::string` instead of Arduino `String` (v2.0.3+)
4. Use static buffers instead of dynamic allocation
5. Free allocated memory promptly

**v2.0.3 improvements:** Migration to `std::string` significantly reduces heap fragmentation during long uptime sessions.

**Heap Monitoring:**
```cpp
if (ESP.getFreeHeap() < 10000) {
    Serial.println("WARNING: Low memory!");
}
```

### Serial Communication

#### No serial output

**Solutions:**
- Verify baud rate: 115200
- Check USB cable (must support data)
- Try different USB port
- Update CH340/CP2102 drivers
- Press EN button on ESP32

#### Garbled text

**Solutions:**
- Set correct baud rate in terminal
- Ensure no other program is using serial port
- Check for EMI from display cables

### Boot Issues

#### ESP32 won't start

**Boot Strapping Pins:**
- GPIO 0: Must be HIGH during boot (LOW for flash mode)
- GPIO 2: Must be LOW during boot
- GPIO 15: Must be LOW during boot

**Solutions:**
1. Press and hold BOOT button, press RST, release both
2. Check for short circuits
3. Verify power supply (3.3V, >500mA)
4. Disconnect peripherals and test bare ESP32

#### Bootloop

**Causes:**
- Corrupt firmware
- Watchdog timeout
- Stack overflow

**Solutions:**
1. Erase flash completely:
```bash
esptool.py --port /dev/ttyUSB0 erase_flash
```

2. Re-upload firmware
3. Verify `platformio.ini` board settings
4. Increase task stack sizes (shell: 16384 bytes as of v2.0.1)

---

### Command Errors

#### "Unknown command"

**Causes:**

* Typographical error
* Command not supported
* Case mismatch (fixed in v2.0.2)

**Solutions:**

* Check spelling carefully
* Use `help` to list available commands
* **v2.0.2+:** Commands are now case-insensitive
* Try command aliases (e.g., `rm` for `delete`, `dir` for `ls`)

---

#### "Usage: ..."

**Cause:**

* Incorrect argument count or order

**Solutions:**

* Review command syntax in the **Command Reference**
* Ensure proper spacing between arguments
* Verify required and optional parameters
* Check for proper argument parsing (improved in v2.0.2 and v2.0.4)

---

### Performance Issues

#### Slow display updates

**Causes:**

* Excessive screen redraws
* High SPI traffic
* Unbatched draw calls

**Solutions:**

* Reduce unnecessary text output
* Use `tft.startWrite()` / `tft.endWrite()` for batching
* Increase SPI speed (only if stable)
* **v2.0.2+:** Automatic screen clearing reduces unnecessary redraws

---

#### Screensaver lag

**Causes:**

* Heavy floating-point math
* Complex per-frame calculations

**Solutions:**

* Reduce frame rate in the `screensaver()` function
* Simplify mathematical operations
* Use lookup tables for trigonometric functions

---

#### Graph rendering issues (v2.0.4)

**Fixed in v2.0.4:** Division-by-zero error in grapher output that caused canvas interference has been resolved. Update to latest version if experiencing graphing issues.

---

### Best Practices

#### Code Style

* Use descriptive variable names
* Comment complex algorithms
* Follow existing naming conventions
* Keep functions under 50 lines
* Use `const` for constants
* **v2.0.3+:** Use `std::string` for all string operations

#### Resource Management

* Always close files after use
* Free dynamically allocated memory
* Use RAII patterns where possible
* Monitor heap usage in long-running tasks
* Avoid excessive string concatenation in loops

#### Error Handling

* Always check return values
* Provide clear user feedback
* Log errors to serial output
* Fail gracefully whenever possible
* **v2.0.2+:** Improved error handling across network commands

---

### License

BSD 3 Clause — see repository for full text.

---

### Contributing

Contributions are welcome. Please:

1. Fork the repository
2. Create a feature branch
3. Test changes thoroughly
4. Follow coding standards (especially `std::string` usage in v2.0.3+)
5. Submit a pull request with a clear description

---

## Changelog

### v2.0.4 

**Core Improvements:**
* Fixed multiple command parser bugs
* Improved and restructured argument handling logic for better reliability

**Network Stack:**
* Resolved visual issue in the `wifi` function that caused incorrect rendering

**Display & UI:**
* Fixed division-by-zero error in the grapher output that caused interference with the canvas

**Technical Changes:**
* Removed the deprecated `pug` function
  * Frees up internal space for future system upgrades
  * Reduces unnecessary code overhead

### v2.0.3

**Major Architectural Changes:**

**Migration to `std::string`:**
* Replaced all usages of `arduino::String` with `std::string`
* Significantly reduces heap fragmentation during long uptime sessions
* Enables safer and more expressive string handling using standard C++ features
* Improves compatibility with modern C++ tooling and static analysis

**Shell & System Enhancements:**

**New / Updated Commands:**
* `username <name>` - Introduces local shell identity customization (session-only, non-persistent)

**Filesystem Improvements:**
* More predictable behavior during file write and append operations
* Improved error handling when target files do not exist
* Internal refactor to fully align filesystem logic with `std::string` migration

**Important Compatibility Note:**
* Systems upgrading from v1.x must reformat SPIFFS
* Partition layout changes require either:
  * `SPIFFS.format()`, or
  * A full flash erase using PlatformIO or Arduino tooling

**Developer-Facing Changes:**

**Coding Standards (Enforced in v2.0.3):**
* All command handlers must:
  * Accept `std::string` inputs
  * Avoid dynamic allocation inside tight loops
  * Use `.c_str()` only at hardware or serial output boundaries

### v2.0.2

**Core Improvements:**

**Command System Overhaul:**
* Refactored command parser with structured argument handling
* Commands are now case-insensitive (`PING`, `ping`, `Ping`, etc.)
* Added extensive command alias support for Unix-like familiarity:
  * `rm` → `delete`
  * `dir` → `ls`
  * `free` → `mem`
  * `restart` → `reboot`
  * `neofetch`, `fastfetch` → `fetch`
  * `wifiscan` → `scanwifi`
  * `ipconfig`, `ifconfig` → Network info
  * `dns`, `nslookup` → DNS lookup
  * `date` → `time`
  * `ntpupdate` → `synctime`
  * `cal` → `calendar`
  * `sw` → `stopwatch`
  * `ss` → `screensaver`
  * `plot` → `graph`
  * `top` → `ps`
  * `h` → `help`

**Network Stack Enhancements:**

**`curl` Improvements:**
* Added verbose mode via `-v` flag
* Improved HTTP status handling (including `429` and `504`)
* Automatic redirect following
* Binary content detection
* Response truncation for large payloads (1500 bytes)
* More descriptive error reporting

**WiFi Enhancements:**
* Ability to scan WiFi networks without connecting
* Added disconnect functionality
* Connection status display with RSSI information

**`ping` Utility:**
* Added min/average/max round-trip time statistics
* Improved timeout handling
* Packet loss percentage reporting

**New Network Commands:**
* DNS lookup: `nslookup` / `dns`
* Network info: `ifconfig` / `ipconfig`

**Display & UI Improvements:**
* Improved screen clearing logic
  * Automatically clears when output reaches bottom of display
  * Prevents duplicated shell prompts
  * More reliable cursor positioning

**Help System Enhancements:**
* Reworked help documentation structure
* Commands grouped by category
* Aliases displayed inline
* Clearer usage examples
* Consistent formatting across all help outputs

**Bug Fixes:**
* Fixed Base64 command argument parsing
* Fixed screen overflow handling
* Fixed cursor positioning after screen clear
* Fixed empty command execution handling
* Corrected ESP32Ping library usage (ESP8266 version was previously used)

**Technical Changes:**
* Improved error handling across all commands
* Streamlined internal code structure
* Removed unnecessary inline comments for cleaner source layout

### v2.0.1

* Added `graph` function for mathematical function plotting
* Shell process size increased to 16 KB to prevent stack overflow
* `wifi` command fixed
* `fetch` command bug with color block display fixed and cursor handling corrected

### v2.0.0

* Added FreeRTOS kernel
* Implemented process management
* Enhanced screensavers with 7 modes (experimental)
* Improved output formatting

### v1.2.0

* Improved calculator with advanced functions
* Added Base64 encoding/decoding
* Improved WiFi connection handling
* Added system statistics display

### v1.1.0

* Added alarm system
* Calendar view
* Timer and stopwatch
* Multiple themes
* Improved file operations

### v1.0.0

* Initial release
* Basic file system
* WiFi connectivity
* Simple calculator
* Display support

---

## Acknowledgments

* Adafruit — GFX and ST7789 libraries
* Espressif — ESP32 Arduino Core
* FreeRTOS — Real-time operating system kernel

---

**Author:** Vuqar Ahadli  
**Repository:** [https://github.com/VuqarAhadli/MiniOS-ESP](https://github.com/VuqarAhadli/MiniOS-ESP)  
**Documentation Version:** 2.0.4  
**Last Updated:** February 2026
