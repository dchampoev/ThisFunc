#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// Extracts placeholders (e.g., #0, #1) from an expression
std::vector<int> extractPlaceholders(const std::string& expression);

#endif // LEXER_H