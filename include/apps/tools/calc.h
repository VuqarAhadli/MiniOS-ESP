#ifndef CALC_H
#define CALC_H

#include <cstddef>

namespace calc_consts {
	static constexpr double CALC_E = 2.71828182845904523536;
}

#include <string> 

int precedence(char op);
bool isRightAssociative(char op);
float applyOp(float a, float b, char op);
bool isFunction(const std::string& expr, int pos, const std::string& func);
std::string getFunctionName(const std::string& expr, int pos);
float applyFunction(const std::string& func, float value);
void calc(const std::string& expression);

#endif