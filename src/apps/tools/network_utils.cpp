#include "network_utils.h"
#include "display.h"
#include "kernel.h"
#include <string>
#include "network.h"

#if !defined(DEVICE_RP2350)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

std::string formatBytes(int bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    return std::to_string(bytes / (1024 * 1024)) + " MB";
}

bool isBinaryContent(const std::string& contentType) {
    return contentType.find("text/") == std::string::npos &&
           contentType.find("json") == std::string::npos &&
           contentType.find("xml") == std::string::npos;
}

void curlURL(const std::string& url) {
    CurlOptions opts;
    opts.url = url;
    opts.verbose = false;
    curlWithOptions(opts);
}

void curlURLVerbose(const std::string& url) {
    CurlOptions opts;
    opts.url = url;
    opts.verbose = true;
    curlWithOptions(opts);
}

#if !defined(DEVICE_RP2350)
void curlWithOptions(CurlOptions opts) {
    if (!isConnected()) {
        printLine("curl: not connected to WiFi");
        logKernelMessage("[NETWORK] curl: not connected to WiFi");

        printLine("Run 'wifi' first");
        return;
    }

    if (opts.url.substr(0, 7) != "http://" && opts.url.substr(0, 8) != "https://") {
        printLine("curl: invalid URL (must start with http:// or https://)");
        return;
    }

    HTTPClient http;
    http.begin(opts.url.c_str());
    http.setTimeout(opts.timeout);
    
    if (opts.followRedirects) {
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    }
    
    http.setUserAgent(opts.userAgent.c_str());
    
    for (int i = 0; i < opts.headerCount; i++) {
        int colonPos = opts.headers[i].find(':');
        if (colonPos > 0) {
            std::string headerName = opts.headers[i].substr(0, colonPos);
            std::string headerValue = opts.headers[i].substr(colonPos + 1);
            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            headerValue.erase(headerValue.find_last_not_of(" \t") + 1);
            http.addHeader(headerName.c_str(), headerValue.c_str());
        }
    }
    
    if (opts.verbose) {
        printLine("> " + opts.method + " " + opts.url);
        printLine("> User-Agent: " + opts.userAgent);
    }
    
    unsigned long startTime = millis();
    int code;
    
    if (opts.method == "POST") {
        code = http.POST(opts.data.c_str());
    } else if (opts.method == "PUT") {
        code = http.PUT(opts.data.c_str());
    } else if (opts.method == "DELETE") {
        code = http.sendRequest("DELETE");
    } else {
        code = http.GET();
    }
    
    unsigned long duration = millis() - startTime;
    
    if (code <= 0) {
        printLine("curl: connection failed");
        printLine("Error: " + std::string(http.errorToString(code).c_str()));
        http.end();
        return;
    }

    if (opts.verbose) {
        printLine("< HTTP/1.1 " + std::to_string(code) + " " + getStatusText(code));
        printLine("< Request-Time: " + std::to_string(duration) + "ms");
    } else {
        printLine("HTTP " + std::to_string(code) + " - " + std::to_string(duration) + "ms");
    }
    
    if (opts.verbose) {
        if (http.hasHeader("Content-Type")) {
            printLine("< Content-Type: " + std::string(http.header("Content-Type").c_str()));
        }
        if (http.hasHeader("Content-Length")) {
            printLine("< Content-Length: " + std::string(http.header("Content-Length").c_str()));
        }
        if (http.hasHeader("Server")) {
            printLine("< Server: " + std::string(http.header("Server").c_str()));
        }
        printLine("");
    }
    
    if (code == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        String contentType_s = http.header("Content-Type");
        std::string contentType = std::string(contentType_s.c_str());
        
        if (isBinaryContent(contentType)) {
            printLine("Binary content (" + contentType + ")");
            printLine("Size: " + formatBytes(contentLength));
            printLine("Cannot display binary data");
        } else {
           
            String payload_s = http.getString();
            std::string payload = std::string(payload_s.c_str());
            int len = payload.length();
            
            if (len > 1500) {
                printLine(payload.substr(0, 1500));
                printLine("");
                printLine("... (+" + std::to_string(len - 1500) + " bytes)");
                printLine("Response truncated at 1500 bytes");
            } else if (len > 0) {
                printLine(payload);
            } else {
                printLine("(empty response)");
            }
        }
    }
    else if (code >= 300 && code < 400) {
        if (http.hasHeader("Location")) {
            printLine("Redirect to: " + std::string(http.header("Location").c_str()));
        }
    }
    else if (code >= 400) {
        
        printLine("Error " + std::to_string(code) + ": " + getStatusText(code));
        String payload_s = http.getString();
        std::string payload = std::string(payload_s.c_str());
        if (payload.length() > 300) {
            printLine(payload.substr(0, 300) + "...");
        } else if (payload.length() > 0) {
            printLine(payload);
        }
    }

    http.end();
}
#else
void curlWithOptions(CurlOptions) {
    printLine("curl is not supported on this device.");
}
#endif

#if !defined(DEVICE_RP2350)
std::string httpGet(const std::string& url) {
    if (!isConnected()) return "";
    
    HTTPClient http;
    http.begin(url.c_str());
    http.setTimeout(10000);
    
    int code = http.GET();
    std::string result = "";
    
    if (code == HTTP_CODE_OK) {
        result = std::string(http.getString().c_str());
    }
    
    http.end();
    return result;
}

int httpPost(const std::string& url, const std::string& data) {
    if (!isConnected()) return -1;
    
    HTTPClient http;
    http.begin(url.c_str());
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);
    
    int code = http.POST(data.c_str());
    http.end();
    
    return code;
}

#else

std::string httpGet(const std::string&) {
    return "";
}

int httpPost(const std::string&, const std::string&) {
    return -1;
}

#endif

void dnsLookup(const std::string& hostname) {
    if (!isConnected()) {
        printLine("dns: not connected to WiFi");
        logKernelMessage("[NETWORK] dns: not connected to WiFi");

        return;
    }
    
#if !defined(DEVICE_RP2350)
    printLine("Looking up: " + hostname);
    
    IPAddress ip;
    if (WiFi.hostByName(hostname.c_str(), ip)) {
        printLine("IP Address: " + std::string(ip.toString().c_str()));
    } else {
        printLine("DNS lookup failed");
    }
#else
    printLine("dns lookup is not supported on this device.");
#endif
}

std::string getStatusText(int code) {
    switch(code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        default: return "";
    }
}