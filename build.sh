#!/bin/bash
#
# MiniOS-ESP Build Script
# Supports: PlatformIO, Makefile, CMake
# Usage: ./build.sh [target] [build_type] [--help]
#

set -e  # Exit on error

# Colours
RED=$'\033[0;31m'
GREEN=$'\033[0;32m'
YELLOW=$'\033[1;33m'
BLUE=$'\033[0;34m'
NC=$'\033[0m' # RESET

# Default values
TARGET="esp32"
BUILD_TYPE="release"
BUILD_SYSTEM="platformio"


# Project info
PROJECT_NAME="MiniOS-ESP"
PROJECT_VERSION="2.1.3"

# Functions

print_banner() {
    echo ""
    echo -e "${BLUE}--- ${GREEN}$PROJECT_NAME ${YELLOW}v$PROJECT_VERSION ${RED}Build Script ${BLUE}---${NC}"
    echo ""
}

print_usage() {
    cat <<EOF
${BLUE}Usage:${NC}
    ./build.sh [TARGET] [BUILD_TYPE] [OPTIONS]

${BLUE}Targets:${NC}
    esp32       Build for ESP32 Standard (default)
    esp32-s3    Build for ESP32-S3
    all         Build for both targets

${BLUE}Build Types:${NC}
    release     Optimized release build (default)
    debug       Debug build with symbols
    clean       Clean build artifacts
    distclean   Full clean including dependencies

${BLUE}Options:${NC}
    --platformio    Use PlatformIO build system (default)
    --make          Use Makefile build system (NOT YET)
    --cmake         Use CMake build system (NOT YET)
    --help          Show this help message
    --version       Show version
    --config        Show build configuration
    --upload        Build and upload to device
    --monitor       Open serial monitor
    --verbose       Verbose build output
    --check         Check dependencies

${BLUE}Examples:${NC}
    ./build.sh esp32            # Build ESP32 release
    ./build.sh esp32-s3 debug   # Build S3 debug
    ./build.sh all              # Build both targets
    ./build.sh esp32 --upload   # Build and upload
    ./build.sh --help           # Show this message

${BLUE}Environment Variables:${NC}
    BUILD_SYSTEM    platformio
    TARGET          esp32|esp32-s3 (default: esp32)
    BUILD_TYPE      release|debug  (default: release)
    VERBOSE         1 for verbose output
    SKIP_DEPS       1 to skip dependency check
EOF
}

print_error() {
    echo -e "[${RED}ERROR${NC}] $1" >&2
}

print_success() {
    echo -e "[${GREEN}SUCCESS${NC}] $1"
}

print_info() {
    echo -e "[${BLUE}INFO${NC}] $1"
}

print_warning() {
    echo -e "[${YELLOW}WARNING${NC}] $1"
}



command_exists() {
    command -v "$1" > /dev/null 2>&1
}

check_deps() {
    print_info "Checking dependencies..."
    local deps_ok=true

    case "$BUILD_SYSTEM" in
        platformio)
            if command_exists platformio; then
                print_success "PlatformIO found: $(platformio --version)"
            else
                print_error "PlatformIO not found. Install with: pip install platformio or install VS code extension"
                deps_ok=false
            fi
            ;;
        # make)
        #     if command_exists make; then
        #         print_success "Make found: $(make --version | head -n1)"
        #     else
        #         print_error "Make not found"
        #         deps_ok=false
        #     fi
        #     if command_exists arm-none-eabi-gcc; then
        #         print_success "ARM toolchain found"
        #     else
        #         print_warning "ARM toolchain not found (arm-none-eabi-gcc)"
        #         print_info "Install with:"
        #         print_info "  macOS: brew install arm-none-eabi-gcc"
        #         print_info "  Linux: sudo apt-get install gcc-arm-none-eabi"
        #         deps_ok=false
        #     fi
        #     ;;
        # cmake)
        #     if [ -z "$IDF_PATH" ]; then
        #         print_error "ESP-IDF not found. Set IDF_PATH or run: . \$IDF_PATH/export.sh"
        #         deps_ok=false
        #     else
        #         print_success "ESP-IDF found: $IDF_PATH"
        #     fi
        #     if command_exists cmake; then
        #         print_success "CMake found: $(cmake --version | head -n1)"
        #     else
        #         print_error "CMake not found"
        #         deps_ok=false
        #     fi
        #     ;;
    esac

    if [ "$deps_ok" = false ]; then
        print_error "Some dependencies are missing"
        return 1
    fi
    
    print_success "All dependencies satisfied"
    return 0
}

show_config() {
    echo ""

    echo -e "${BLUE}---        Build Configuration      ---${NC}"
    echo -e "${BLUE}|${NC} Project:      $PROJECT_NAME v$PROJECT_VERSION"
    echo -e "${BLUE}|${NC} Target:       $TARGET"
    echo -e "${BLUE}|${NC} Build Type:   $BUILD_TYPE"
    echo -e "${BLUE}|${NC} Build System: $BUILD_SYSTEM"
    echo -e "${BLUE}|${NC} Current Dir:  $(pwd)"
    echo ""
}

# Build with PlatformIO
build_platformio() {
    local env=""
    case "$TARGET" in
        esp32)
            env="esp32dev"
            ;;
        esp32-s3)
            env="esp32s3dev"
            ;;
        *)
            print_error "Unknown target: $TARGET"
            return 1
            ;;
    esac

    print_info "Building with PlatformIO for $TARGET..."
    
    local cmd="platformio run --environment $env"
    [ "$VERBOSE" = "1" ] && cmd="$cmd -vv"
    
    eval "$cmd" || return 1
    print_success "Build complete"
}

# Upload with PlatformIO
upload_platformio() {
    local env=""
    case "$TARGET" in
        esp32)
            env="esp32dev"
            ;;
        esp32-s3)
            env="esp32s3dev"
            ;;
        *)
            print_error "Unknown target: $TARGET"
            return 1
            ;;
    esac

    print_info "Uploading to device..."
    local cmd="platformio run --target upload --environment $env"
    [ "$VERBOSE" = "1" ] && cmd="$cmd -vv"
    
    eval "$cmd" || return 1
    print_success "Upload complete"
}

# Build with Makefile
# build_make() {
#     print_error "Makefile build system not available for this project"
#     print_info "Please use: ./build.sh --platformio (recommended)"
#     print_info "Or set up ESP-IDF and use: ./build.sh --cmake"
#     return 1
# }

# # Build with CMake (ESP-IDF)
# build_cmake() {
#     if [ -z "$IDF_PATH" ]; then
#         print_error "ESP-IDF not configured"
#         print_info "Install ESP-IDF from: https://github.com/espressif/esp-idf"
#         print_info "Then run: export IDF_PATH=~/esp/esp-idf && . \$IDF_PATH/export.sh"
#         return 1
#     fi
    
#     print_info "Building with ESP-IDF/CMake for $TARGET ($BUILD_TYPE)..."
    
#     # Create build directory if it doesn't exist
#     if [ ! -d "build" ]; then
#         print_info "Creating build directory..."
#         mkdir -p build
#     fi
    
#     cd build
    
#     # Configure based on target
#     local idf_target=""
#     case "$TARGET" in
#         esp32)
#             idf_target="esp32"
#             ;;
#         esp32-s3)
#             idf_target="esp32s3"
#             ;;
#     esac
    
#     print_info "Configuring for $idf_target..."
#     idf.py set-target $idf_target || { cd ..; return 1; }
    
#     # Build
#     print_info "Building..."
#     [ "$VERBOSE" = "1" ] && idf.py -v build || idf.py build
#     local build_result=$?
    
#     if [ $build_result -ne 0 ]; then
#         cd ..
#         return 1
#     fi
    
#     cd ..
#     print_success "Build complete"
# }

# Clean build artifacts
clean_build() {
    case "$BUILD_SYSTEM" in
        platformio)
            print_info "Cleaning with PlatformIO..."
            platformio run --target clean
            ;;
        # make)
        #     print_error "Makefile build system not available"
        #     return 1
        #     ;;
        # cmake)
        #     print_info "Cleaning CMake/ESP-IDF build..."
        #     if [ -d "build" ]; then
        #         cd build
        #         idf.py fullclean || rm -rf .
        #         cd ..
        #     fi
        #     ;;
    esac
    print_success "Clean complete"
}

# Full clean
distclean() {
    case "$BUILD_SYSTEM" in
        platformio)
            print_info "Full clean with PlatformIO..."
            platformio run --target clean
            rm -rf .pio
            ;;
        # make)
        #     print_error "Makefile build system not available"
        #     return 1
        #     ;;
        # cmake)
        #     print_info "Full clean ESP-IDF build..."
        #     rm -rf build
        #     ;;
    esac
    print_success "Full clean complete"
}

# Open serial monitor
monitor() {
    case "$BUILD_SYSTEM" in
        platformio)
            print_info "Opening serial monitor (115200 baud)..."
            platformio device monitor --baud 115200
            ;;
        # cmake)
        #     print_info "Opening serial monitor (115200 baud)..."
        #     idf.py monitor
        #     ;;
        # *)
        #     print_warning "Serial monitor requires PlatformIO or ESP-IDF"
        #     print_info "Install PlatformIO: pip install platformio"
        #     print_info "Or set up ESP-IDF: https://github.com/espressif/esp-idf"
        #     ;;
    esac
}


main() {
    print_banner



    echo "Select device:"
    echo "1) esp32"
    echo "2) esp32-s3"
    echo "3) all"
    read -rp "Choice [1-3]: " device_choice

    case "$device_choice" in
        1) TARGET="esp32" ;;
        2) TARGET="esp32-s3" ;;
        3) TARGET="all" ;;
        *) print_error "Invalid choice"; exit 1 ;;
    esac


    echo ""
    echo "Select build type:"
    echo "1) release"
    echo "2) debug"
    echo "3) clean"
    echo "4) distclean"
    read -rp "Choice [1-4]: " type_choice

    case "$type_choice" in
        1) BUILD_TYPE="release" ;;
        2) BUILD_TYPE="debug" ;;
        3)
            print_banner
            show_config
            clean_build
            exit 0
            ;;
        4)
            print_banner
            show_config
            distclean
            exit 0
            ;;
        *) print_error "Invalid choice"; exit 1 ;;
    esac


    echo ""
    echo "Select build system:"
    echo "1) PlatformIO"
    echo "2) (disabled) Make"
    echo "3) (disabled) CMake"
    read -rp "Choice [1-3]: " sys_choice

    case "$sys_choice" in
        1) BUILD_SYSTEM="platformio" ;;
        2|3)
            print_error "Not implemented"
            exit 1
            ;;
        *) print_error "Invalid choice"; exit 1 ;;
    esac


    echo ""
    echo "Extra options:"
    read -rp "Verbose build? (y/n): " verbose_choice
    [[ "$verbose_choice" == "y" ]] && VERBOSE=1

    read -rp "Upload after build? (y/n): " upload_choice
    [[ "$upload_choice" == "y" ]] && UPLOAD=1

    read -rp "Open monitor after? (y/n): " monitor_choice
    [[ "$monitor_choice" == "y" ]] && MONITOR=1


    echo ""
    show_config

    # dependency check
    if [ "$SKIP_DEPS" != "1" ]; then
        check_deps || exit 1
    fi


    # handle "all"
    if [ "$TARGET" = "all" ]; then
        print_info "Building ESP32..."
        TARGET="esp32"
        build_platformio || exit 1

        print_info "Building ESP32-S3..."
        TARGET="esp32-s3"
        build_platformio || exit 1
    else
        build_platformio || exit 1
    fi


    # upload
    if [ "$UPLOAD" = "1" ] && [ "$TARGET" != "all" ]; then
        upload_platformio || exit 1
    fi


    # monitor
    if [ "$MONITOR" = "1" ]; then
        monitor
    fi

    echo ""
    print_success "All done!"
}

# Run main function
main "$@"