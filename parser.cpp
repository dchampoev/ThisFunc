#include "parser.h"

// Splits arguments within parentheses
std::vector<std::string> splitArguments(const std::string& args) {
    std::vector<std::string> result;
    int bracketCount = 0;
    std::string current;
    for (char c : args) {
        if (c == '(') bracketCount++;
        if (c == ')') bracketCount--;
        if (c == ',' && bracketCount == 0) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    if (!current.empty()) result.push_back(current);
    return result;
}

// Trims whitespace
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}