# MiniOS v2.1.3 | Architectural update & RP2350 support

## Overview

This update is mainly focused on RP2350 microcontroller support. I also added a new experimental GUI-style settings menu. It is mostly vibe-coded for now so it does not work very well yet, but I will improve it ASAP.

I also added experimental wallpapers. I really liked the style, so in future updates I will probably make more wallpapers for each theme.

## New Features

- RP2350 support and new architectural changes to better separate devices from each other

> Pin config for RP2350 Pi Zero Waveshare

```c id="89x2ls"
#define TFT_CS 17
#define TFT_RST 20
#define TFT_DC 26
#define TFT_MOSI 19
#define TFT_SCLK 18
```

- New experimental settings menu
- Experimental wallpaper support

## Improvements

- **Improved `name` command** - now it works more like an actual app setup flow in the terminal and asks the user for a username
- **Display buffering improvements & terminal bugs addressed** - I made a small mistake in the `renderScreen()` function that caused the last line to sometimes not render properly

---

**Repository link:**
[https://github.com/VuqarAhadli](https://github.com/VuqarAhadli)

**Version:** 2.1.3
Author: Vugar Ahadli
