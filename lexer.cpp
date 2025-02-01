#include "lexer.h"

std::vector<int> extractPlaceholders(const std::string& expression) {
    std::vector<int> placeholders;
    size_t i = 0;

    while (i < expression.size()) {
        if (expression[i] == '#') { // Check for the '#' character
            size_t j = i + 1;
            std::string number;
            while (j < expression.size() && (expression[j] >= '0' && expression[j] <= '9')) { // Check for digits
                number += expression[j];
                ++j;
            }
            if (!number.empty()) {
                placeholders.push_back(std::stoi(number)); // Convert and store the number
            }
            i = j; // Skip past the number
        } else {
            ++i; // Move to the next character
        }
    }

    return placeholders;
}