# MiniOS-ESP

A lightweight command-line operating system for ESP32 with ST7789 display support.

## Features

- **File System**: Read, write, and delete files using SPIFFS
- **WiFi Connectivity**: Connect to WiFi networks
- **Time Synchronization**: NTP time sync
- **HTTP Client**: Fetch web content with curl-like commands
- **Calculator**: Simple arithmetic calculator
- **Themes**: Multiple color themes for the display
- **Serial Command Interface**: Control everything via serial commands

## Hardware Requirements

- ESP32 development board
- ST7789 TFT Display (240x320)
- Wiring:
  - TFT_CS → GPIO 5
  - TFT_DC → GPIO 2
  - TFT_RST → GPIO 4

## Setup

1. **Clone the repository**
   ```bash
   git clone https://github.com/VuqarAhadli/MiniOS-ESP.git
   cd MiniOS-ESP
   ```

2. **Configure WiFi credentials**
   ```bash
   cp include/config.h.example include/config.h
   ```
   Edit `include/config.h` and add your WiFi SSID and password.

3. **Upload to ESP32**
   - Using PlatformIO: `pio run --target upload`
   - Using Arduino IDE: Open `src/main.cpp` and upload

4. **Open Serial Monitor**
   - Set baud rate to 115200
   - Start typing commands!

## Available Commands

| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show all commands | `help` |
| `os` | Display MiniOS logo | `os` |
| `version` | Show version info | `version` |
| `wifi` | Connect to WiFi | `wifi` |
| `time` | Show current time | `time` |
| `synctime` | Sync time with NTP | `synctime` |
| `write <file> <text>` | Write text to file | `write notes.txt Hello World` |
| `read <file>` | Read file contents | `read notes.txt` |
| `delete <file>` | Delete a file | `delete notes.txt` |
| `curl <url>` | Fetch URL content | `curl https://example.com` |
| `calc <expression>` | Calculate math | `calc 15 + 27 * 2` |
| `themes` | List available themes | `themes` |
| `theme <name\|number>` | Set theme | `theme matrix` or `theme 0` |

## Themes

Available themes:
- `matrix` (0) - Green on black **Default**
- `cyan` (1) - Cyan on black
- `classic` (2) - White on black
- `blue` (3) - Yellow on blue
- `light` (4) - Black on white
- `dark-orange` (5) - Orange on dark
- `purple` (6) - Magenta on dark
- `red night` (7) - Red on dark

## Project Structure

```
MiniOS-ESP/
├── src/
│   ├── main.cpp          # Main program loop
│   ├── commands.cpp      # Command implementations
│   ├── commands.h
│   ├── display.cpp       # TFT display functions
│   ├── display.h
│   ├── filesystem.cpp    # SPIFFS file operations
│   ├── filesystem.h
│   ├── network.cpp       # WiFi and HTTP
│   ├── network.h
│   ├── theme.cpp         # Theme management
│   └── theme.h
├── include/
│   └──  config.h          # Your configuration 
├── platformio.ini
├── README.md
└── .gitignore
```

## Dependencies

- Adafruit GFX Library
- Adafruit ST7789 Library
- ESP32 Arduino Core

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License
- MIT License


## Author

VuqarAhadli - [GitHub](https://github.com/VuqarAhadli)

## Version

MiniOS-ESP v1.0
