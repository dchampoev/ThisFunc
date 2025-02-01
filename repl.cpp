#include "repl.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <string>

void runRepl() {
    ThisFuncInterpreter interpreter;
    std::string input;

    std::cout << "Welcome to thisFunc interpreter. Type 'exit' to quit.\n";
    while (true) {
        std::getline(std::cin, input);

        if (input == "exit") break;

        try {
            if (input.find("<-") != std::string::npos) {
                interpreter.declareFunction(input);
                std::cout << ">\n";
            } else {
                Value result = interpreter.evaluate(input);
                if (std::holds_alternative<double>(result)) {
                    std::cout << "> " << std::get<double>(result) << "\n";
                } else {
                    const auto& list = std::get<std::vector<double>>(result);
                    std::cout << "> [";
                    for (size_t i = 0; i < list.size(); ++i) {
                        std::cout << list[i];
                        if (i < list.size() - 1) std::cout << ", ";
                    }
                    std::cout << "]\n";
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void executeFile(const std::string& filename) {
    ThisFuncInterpreter interpreter;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        try {
            if (line.find("<-") != std::string::npos) {
                interpreter.declareFunction(line);
                std::cout << "> " << line << "\n";
            } else {
                Value result = interpreter.evaluate(line);
                if (std::holds_alternative<double>(result)) {
                    std::cout << "> " << std::get<double>(result) << "\n";
                } else {
                    const auto& list = std::get<std::vector<double>>(result);
                    std::cout << "> [";
                    for (size_t i = 0; i < list.size(); ++i) {
                        std::cout << list[i];
                        if (i < list.size() - 1) std::cout << ", ";
                    }
                    std::cout << "]\n";
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << " (line: " << line << ")\n";
        }
    }

    file.close();
}