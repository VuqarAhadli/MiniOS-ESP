#include "filesystem.h"
#include "display.h"
#include "kernel.h"
#if !defined(DEVICE_RP2350)
#include <FS.h>
#include <SPIFFS.h>
#endif
#include <string>

#if !defined(DEVICE_RP2350)

bool initFilesystem() {
    if (!SPIFFS.begin(true)) {
        printLine("[FS] SPIFFS Failed.");
        logKernelMessage("[FS] SPIFFS Failed.");
        return false;
    }
    
    
    size_t total = SPIFFS.totalBytes();
    size_t used = SPIFFS.usedBytes();
    printLine("SPIFFS: " + std::to_string(used) + "/" + std::to_string(total) + " bytes");
    
    return true;
}

void writeFile(const std::string& name_in, const std::string& data) {
    std::string name = name_in;
   
    if (name.substr(0, 1) != "/") {
        name = "/" + name;
    }
    
    File f = SPIFFS.open(name.c_str(), FILE_WRITE);
    if (!f) {
        printLine("[FS] Error opening file.");
        logKernelMessage("[FS] Error opening file.");
        return;
    }
    
    size_t written = f.print(data.c_str());
    f.flush();  
    f.close();
    
    if (written > 0) {
        printLine("Written " + std::to_string(written) + " bytes.");
    } else {
        printLine("Error: 0 bytes written.");
    }
}

void appendFile(const std::string& name_in, const std::string& data) {
    std::string name = name_in;
    
    if (name.substr(0, 1) != "/") {
        name = "/" + name;
    }
    
    File f = SPIFFS.open(name.c_str(), FILE_APPEND);
    if (!f) {
        printLine("[FS] Error opening file.");
        logKernelMessage("[FS] Error opening file.");
        return;
    }
    
    size_t written = f.print(data.c_str());
    f.flush(); 
    f.close();
    
    if (written > 0) {
        printLine("Appended " + std::to_string(written) + " bytes.");
    } else {
        printLine("Error: 0 bytes appended.");
    }
}

void readFile(const std::string& name_in) {
    std::string name = name_in;
    
    if (name.substr(0, 1) != "/") {
        name = "/" + name;
    }
    
    File f = SPIFFS.open(name.c_str());
    if (!f) {
        printLine("[FS] Error reading file.");
        logKernelMessage("[FS] Error reading file.");
        return;
    }
    
    if (f.available()) {
        printLine("File: " + name);
        while (f.available()) {
            String line = f.readStringUntil('\n');
            printLine(std::string(line.c_str()));
        }
    } else {
        printLine("File is empty.");
    }
    
    f.close();
}

void deleteFile(const std::string& name_in) {
    std::string name = name_in;
    
    if (name.substr(0, 1) != "/") {
        name = "/" + name;
    }
    
    if (SPIFFS.remove(name.c_str())) {
        printLine("File deleted.");
    } else {
        printLine("[FS] Error deleting file.");
        logKernelMessage("[FS] Error deleting file.");
    }
}

void listFiles() {
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    printLine("SPIFFS: " + std::to_string(usedBytes) + "/" + std::to_string(totalBytes) + " bytes");
    printLine("Files:");
    
    File root = SPIFFS.open("/");
    if (!root) {
        printLine("[FS] Failed to open root");
        logKernelMessage("[FS] Failed to open root");
        return;
    }
    
    bool found = false;
    
    while (true) {
        File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        found = true;
        std::string fileName(file.name());
        
        
        if (!fileName.empty() && fileName[0] == '/') {
            fileName = fileName.substr(1);
        }
        
        if (fileName.length() > 0) {
            printLine("  " + fileName + " - " + std::to_string(file.size()) + " bytes");
        }
        
        file.close();
    }
    
    if (!found) {
        printLine("  (no files)");
    }
    
    root.close();
}

bool renameFile(const std::string& oldName_in, const std::string& newName_in) {
    std::string oldName = oldName_in;
    std::string newName = newName_in;
   
    if (oldName.substr(0, 1) != "/") {
        oldName = "/" + oldName;
    }
    if (newName.substr(0, 1) != "/") {
        newName = "/" + newName;
    }
    
    if (SPIFFS.rename(oldName.c_str(), newName.c_str())) {
        printLine("Renamed.");
        return true;
    }
    
    printLine("Rename failed.");
    return false;
}

bool copyFile(const std::string& src_in, const std::string& dst_in) {
    std::string src = src_in;
    std::string dst = dst_in;
    
    if (src.substr(0, 1) != "/") {
        src = "/" + src;
    }
    if (dst.substr(0, 1) != "/") {
        dst = "/" + dst;
    }
    
    File in = SPIFFS.open(src.c_str());
    if (!in) {
        printLine("[FS] Error reading src file.");
        logKernelMessage("[FS] Error reading src file.");
        return false;
    }
    
    File out = SPIFFS.open(dst.c_str(), FILE_WRITE);
    if (!out) {
        in.close();
        printLine("[FS] Error opening dst file.");
        logKernelMessage("[FS] Error opening dst file.");
        return false;
    }
    
    uint8_t buf[64];
    while (in.available()) {
        int len = in.read(buf, sizeof(buf));
        out.write(buf, len);
    }
    
    in.close();
    out.flush();  
    out.close();
    
    printLine("Copied.");
    return true;
}
#else

bool initFilesystem() {
    printLine("[FS] Filesystem not supported on this device.");
    logKernelMessage("[FS] Filesystem not supported on this device.");
    return false;
}

void writeFile(const std::string& name, const std::string& data) {
    printLine("[FS] writeFile unsupported on this device.");
    logKernelMessage("[FS] writeFile unsupported on this device.");
}

void appendFile(const std::string& name, const std::string& data) {
    printLine("[FS] appendFile unsupported on this device.");
    logKernelMessage("[FS] appendFile unsupported on this device.");
}

void readFile(const std::string& name) {
    printLine("[FS] readFile unsupported on this device.");
    logKernelMessage("[FS] readFile unsupported on this device.");
}

void deleteFile(const std::string& name) {
    printLine("[FS] deleteFile unsupported on this device.");
    logKernelMessage("[FS] deleteFile unsupported on this device.");
}

void listFiles() {
    printLine("[FS] listFiles unsupported on this device.");
    logKernelMessage("[FS] listFiles unsupported on this device.");
}

bool renameFile(const std::string& oldName, const std::string& newName) {
    printLine("[FS] renameFile unsupported on this device.");
    logKernelMessage("[FS] renameFile unsupported on this device.");
    return false;
}

bool copyFile(const std::string& src, const std::string& dst) {
    printLine("[FS] copyFile unsupported on this device.");
    logKernelMessage("[FS] copyFile unsupported on this device.");
    return false;
}

#endif
