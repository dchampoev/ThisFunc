#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

// Splits arguments within parentheses
std::vector<std::string> splitArguments(const std::string& args);

// Trims whitespace
std::string trim(const std::string& str);

#endif // PARSER_H