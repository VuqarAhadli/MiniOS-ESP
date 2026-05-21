#include "calc.h"
#include "display.h"
#include <math.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>






int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^') return 3;
    return 0;
}

bool isRightAssociative(char op) {
    return (op == '^');
}

float applyOp(float a, float b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) {
                printLine("Error: Division by zero");
                return 0;
            }
            return a / b;
        case '%': 
            if (b == 0) {
                printLine("Error: Modulo by zero");
                return 0;
            }
            return (int)a % (int)b;
        case '^': return pow(a, b);
    }
    return 0;
}

bool isFunction(const std::string& expr, int pos, const std::string& func) {
    int len = func.length();
    if (pos + len > (int)expr.length()) return false;
    std::string substr = expr.substr(pos, len);
    std::transform(substr.begin(), substr.end(), substr.begin(), ::tolower);
    return substr == func;
}

std::string getFunctionName(const std::string& expr, int pos) {
    std::string functions[] = {
        "sqrt", "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "log", "ln", "exp",
        "abs", "ceil", "floor", "round"
    };
    for (int i = 0; i < 17; i++) {
        if (isFunction(expr, pos, functions[i])) {
            return functions[i];
        }
    }
    return "";
}

float applyFunction(const std::string& func, float value) {
    if (func == "sqrt") return sqrt(value);
    if (func == "sin") return sin(value);
    if (func == "cos") return cos(value);
    if (func == "tan") return tan(value);
    if (func == "asin") return asin(value);
    if (func == "acos") return acos(value);
    if (func == "atan") return atan(value);
    if (func == "sinh") return sinh(value);
    if (func == "cosh") return cosh(value);
    if (func == "tanh") return tanh(value);
    if (func == "log") return log10(value);
    if (func == "ln") return log(value);
    if (func == "exp") return exp(value);
    if (func == "abs") return abs(value);
    if (func == "ceil") return ceil(value);
    if (func == "floor") return floor(value);
    if (func == "round") return round(value);
    return value;
}

void calc(const std::string& expression_in) {
    std::string expression = expression_in;
    size_t pos = expression.find("pi");
    while (pos != std::string::npos) {
        expression.replace(pos, 2, std::to_string(PI));
        pos = expression.find("pi", pos + std::to_string(PI).length());
    }
    pos = expression.find("e");
    while (pos != std::string::npos) {
        expression.replace(pos, 1, std::to_string(calc_consts::CALC_E));
        pos = expression.find("e", pos + std::to_string(calc_consts::CALC_E).length());
    }
    
    if (expression.length() == 0) {
        printLine("Error: Empty expression");
        return;
    }
    
    float values[50];
    char ops[50];
    std::string functions[50];
    int vTop = -1;
    int oTop = -1;
    int fTop = -1;
    
    int n = expression.length();
    
    for (int i = 0; i < n; i++) {
        std::string funcName = getFunctionName(expression, i);
        if (funcName.length() > 0) {
            i += funcName.length();
            functions[++fTop] = funcName;
            continue;
        }
        
        if (expression[i] == '(') {
            ops[++oTop] = '(';
        }
        else if (expression[i] == ')') {
            while (oTop >= 0 && ops[oTop] != '(') {
                float b = values[vTop--];
                float a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = applyOp(a, b, op);
            }
            if (oTop >= 0) oTop--;
            
            if (fTop >= 0) {
                float val = values[vTop--];
                val = applyFunction(functions[fTop--], val);
                values[++vTop] = val;
            }
        }
        else if (isdigit(expression[i]) || expression[i] == '.') {
            float num = 0;
            float decimal = 0;
            bool hasDecimal = false;
            int decimalPlaces = 0;
            
            while (i < n && (isdigit(expression[i]) || expression[i] == '.')) {
                if (expression[i] == '.') {
                    hasDecimal = true;
                } else {
                    if (hasDecimal) {
                        decimal = decimal * 10 + (expression[i] - '0');
                        decimalPlaces++;
                    } else {
                        num = num * 10 + (expression[i] - '0');
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
        else if (expression[i] == '-' && 
                (i == 0 || expression[i-1] == '(' || expression[i-1] == '+' || 
                 expression[i-1] == '-' || expression[i-1] == '*' || 
                 expression[i-1] == '/' || expression[i-1] == '^')) {
            values[++vTop] = 0;
            ops[++oTop] = '-';
        }
        else if (expression[i] == '+' || expression[i] == '-' || 
                 expression[i] == '*' || expression[i] == '/' || 
                 expression[i] == '%' || expression[i] == '^') {
            while (oTop >= 0 && ops[oTop] != '(' && 
                   (precedence(ops[oTop]) > precedence(expression[i]) ||
                    (precedence(ops[oTop]) == precedence(expression[i]) && 
                     !isRightAssociative(expression[i])))) {
                float b = values[vTop--];
                float a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = applyOp(a, b, op);
            }
            ops[++oTop] = expression[i];
        }
        else {
            printLine("Error: Invalid character '" + std::string(1, expression[i]) + "'");
            return;
        }
    }
    
    while (oTop >= 0) {
        if (ops[oTop] == '(') {
            printLine("Error: Mismatched parentheses");
            return;
        }
        float b = values[vTop--];
        float a = values[vTop--];
        char op = ops[oTop--];
        values[++vTop] = applyOp(a, b, op);
    }
    
    if (vTop != 0) {
        printLine("Error: Invalid expression");
        return;
    }
    
    float result = values[vTop];
    
    if (result == (int)result && abs(result) < 1000000) {
        printLine("Result: " + std::to_string((int)result));
    } else {
        char buf[32];
        sprintf(buf, "Result: %.6f", result);
        printLine(buf);
    }
}
