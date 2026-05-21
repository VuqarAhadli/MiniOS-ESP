#include "display.h"
#include "theme.h"
#include "config.h"
#include "grapher.h"
#include "commands.h"
#include "tools/calc.h"
#include <cmath>
#include <string>


namespace Operation{
    /*
    The default applyOp() implementation in commands.cpp contains
    printLine() calls that interfere with the plotting pipeline.
    When plotting expressions involving division by a variable
    (e.g., sin(x)/x at x = 0), a division by zero error message is printed
    to the display, disrupting the rendered plot.
    */

    float applyOp(float a, float b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) { 
                return 0;
            }
            return a / b;
        case '%': 
            if (b == 0) {
                return 0;
            }
            return (int)a % (int)b;
        case '^': return pow(a, b);
    }
    return 0;
}
}

bool evaluateWithX(const std::string& expression, float xValue, float& result) {
    // We need to be careful not to replace 'x' in function names like 'exp'
    std::string expr = "";
    for (int i = 0; i < (int)expression.length(); i++) {
        if (expression[i] == 'x') {
            bool isStandalone = true;
            if (i > 0 && (isalpha(expression[i-1]) || isdigit(expression[i-1]))) {
                isStandalone = false;
            }
            if (i < (int)expression.length() - 1 && (isalpha(expression[i+1]) || isdigit(expression[i+1]))) {
                isStandalone = false;
            }
            
            if (isStandalone) {
                char buf[32];
                sprintf(buf, "(%.6f)", xValue);
                expr += buf;
            } else {
                expr += "x";
            }
        } else {
            expr += expression[i];
        }
    }
    
    expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());
    
    size_t pos = expr.find("pi");
    while (pos != std::string::npos) {
        expr.replace(pos, 2, std::to_string(PI));
        pos = expr.find("pi", pos + std::to_string(PI).length());
    }
    pos = expr.find("e");
    while (pos != std::string::npos) {
        expr.replace(pos, 1, std::to_string(calc_consts::CALC_E));
        pos = expr.find("e", pos + std::to_string(calc_consts::CALC_E).length());
    }
    
    if (expr.length() == 0) {
        return false;
    }
    
    float values[50];
    char ops[50];
    std::string functions[50];
    int vTop = -1;
    int oTop = -1;
    int fTop = -1;
    int n = expr.length();
    
    for (int i = 0; i < n; i++) {
        std::string funcName = getFunctionName(expr, i);
        if (funcName.length() > 0) {
            i += funcName.length();
            functions[++fTop] = funcName;
            continue;
        }
        
        if (expr[i] == '(') {
            ops[++oTop] = '(';
        }
        else if (expr[i] == ')') {
            while (oTop >= 0 && ops[oTop] != '(') {
                if (vTop < 1) return false;
                float b = values[vTop--];
                float a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = Operation::applyOp(a, b, op);
            }
            if (oTop >= 0) oTop--;
            
            if (fTop >= 0) {
                if (vTop < 0) return false;
                float val = values[vTop--];
                val = applyFunction(functions[fTop--], val);
                values[++vTop] = val;
            }
        }
        else if (isdigit(expr[i]) || expr[i] == '.') {
            float num = 0;
            float decimal = 0;
            bool hasDecimal = false;
            int decimalPlaces = 0;
            
            while (i < n && (isdigit(expr[i]) || expr[i] == '.')) {
                if (expr[i] == '.') {
                    if (hasDecimal) return false;
                    hasDecimal = true;
                } else {
                    if (hasDecimal) {
                        decimal = decimal * 10 + (expr[i] - '0');
                        decimalPlaces++;
                    } else {
                        num = num * 10 + (expr[i] - '0');
                    }
                }
                i++;
            }
            i--;
            
            if (hasDecimal && decimalPlaces > 0) {
                num += decimal / pow(10, decimalPlaces);
            }
            values[++vTop] = num;
        }
        else if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || 
                 expr[i-1] == '+' || expr[i-1] == '-' || 
                 expr[i-1] == '*' || expr[i-1] == '/' || 
                 expr[i-1] == '^')) {
            values[++vTop] = 0;
            ops[++oTop] = '-';
        }
        else if (expr[i] == '+' || expr[i] == '-' || 
                 expr[i] == '*' || expr[i] == '/' || 
                 expr[i] == '%' || expr[i] == '^') {
            while (oTop >= 0 && ops[oTop] != '(' && 
                   (precedence(ops[oTop]) > precedence(expr[i]) || 
                   (precedence(ops[oTop]) == precedence(expr[i]) && 
                    !isRightAssociative(expr[i])))) {
                if (vTop < 1) return false;
                float b = values[vTop--];
                float a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = Operation::applyOp(a, b, op);
            }
            ops[++oTop] = expr[i];
        }
        else {
            return false;
        }
    }
    
    while (oTop >= 0) {
        if (ops[oTop] == '(') return false;
        if (vTop < 1) return false;
        float b = values[vTop--];
        float a = values[vTop--];
        char op = ops[oTop--];
        values[++vTop] = Operation::applyOp(a, b, op);
    }
    
    if (vTop != 0) return false;
    
    result = values[vTop];
    return true;
}

void funcToGraph(const std::string& expression, const std::string& lineColour) {
    screenLocked = true;
    
    tft.fillScreen(ST77XX_WHITE);
    tft.setCursor(0, 0);
    
    const int screenWidth = 320;
    const int screenHeight = 230;
    
    const float xMin = -10.0;
    const float xMax = 10.0;
    const float yMin = -10.0;
    const float yMax = 10.0;
    
    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;
    
    float xPixelsPerUnit = screenWidth / (xMax - xMin);
    float yPixelsPerUnit = screenHeight / (yMax - yMin);
    float pixelsPerUnit = (xPixelsPerUnit < yPixelsPerUnit) ? xPixelsPerUnit : yPixelsPerUnit;
    
    tft.setTextSize(1);
    for (int i = (int)xMin; i <= (int)xMax; i++) {
        int px = centerX + (int)(i * pixelsPerUnit);
        if (px >= 0 && px < screenWidth) {
            uint16_t gridColor = (i == 0) ? 0xBDF7 : 0xE71C;
            tft.drawLine(px, 0, px, screenHeight, gridColor);
        }
    }
    for (int i = (int)yMin; i <= (int)yMax; i++) {
        int py = centerY - (int)(i * pixelsPerUnit);
        if (py >= 0 && py < screenHeight) {
            uint16_t gridColor = (i == 0) ? 0xBDF7 : 0xE71C;
            tft.drawLine(0, py, screenWidth, py, gridColor);
        }
    }
    
    tft.drawLine(0, centerY, screenWidth, centerY, ST77XX_BLACK);
    tft.drawLine(0, centerY + 1, screenWidth, centerY + 1, ST77XX_BLACK);
    tft.drawLine(centerX, 0, centerX, screenHeight, ST77XX_BLACK);
    tft.drawLine(centerX + 1, 0, centerX + 1, screenHeight, ST77XX_BLACK);
    
    tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
    tft.setCursor(screenWidth - 15, centerY + 3);
    tft.print("x");
    tft.setCursor(centerX + 3, 2);
    tft.print("y");
    
    uint16_t color = ST77XX_BLUE;
    if (lineColour == "red") color = ST77XX_RED;
    else if (lineColour == "green") color = ST77XX_GREEN;
    else if (lineColour == "blue") color = ST77XX_BLUE;
    else if (lineColour == "black") color = ST77XX_BLACK;
    else if (lineColour == "yellow") color = ST77XX_YELLOW; /*colour parser*/
    else if (lineColour == "cyan") color = ST77XX_CYAN;
    else if (lineColour == "magenta") color = ST77XX_MAGENTA;
    else if (lineColour == "orange") color = ST77XX_ORANGE;
    else if (lineColour == "purple") color = 0x780F;
    
    float prevY = NAN;
    int prevPx = -1;
    int prevPy = -1;
    
    for (int px = 0; px < screenWidth; px++) {
        float mathX = (px - centerX) / pixelsPerUnit;
        
        float mathY;
        if (evaluateWithX(expression, mathX, mathY)) {
            if (isnan(mathY) || isinf(mathY)) {
                prevY = NAN;
                continue;
            }
            
            int py = centerY - (int)(mathY * pixelsPerUnit);
            
            if (py >= 0 && py < screenHeight) {
                if (!isnan(prevY) && prevPx >= 0) {
                    if (abs(py - prevPy) < screenHeight / 2) {
                        tft.drawLine(prevPx, prevPy, px, py, color);
                    } else {
                        tft.drawPixel(px, py, color);
                    }
                } else {
                    tft.drawPixel(px, py, color);
                }
                
                prevY = mathY;
                prevPx = px;
                prevPy = py;
            } else {
                prevY = NAN;
            }
        } else {
            prevY = NAN;
        }
    }
    
    
    
    tft.fillRect(0, 230, screenWidth, 10, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(5, 230);
    tft.print("Press ENTER to exit");
    
    
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                screenLocked = false;
                applyTheme();
                clearScreen();
                break;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}