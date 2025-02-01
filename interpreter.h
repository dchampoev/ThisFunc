#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <cctype> // std::isdigit
#include "parser.h"
#include "lexer.h"

using Value = std::variant<double, std::vector<double>, std::string>;

class ThisFuncInterpreter {
private:
    struct Function {
        std::function<Value(const std::vector<Value>&)> implementation;
        size_t argCount;
        std::string expression;
    };

    std::unordered_map<std::string, Function> functions;
    std::unordered_map<std::string, std::vector<double>> declaredLists;

    Function* findFunction(const std::string& name);
    bool isSingleArgumentFunction(const std::string& functionName);
    double toDouble(const Value& value) const;
    const std::vector<double>& toList(const Value& value) const;

public:
    ThisFuncInterpreter();
    void declareFunction(const std::string& declaration);
    Value evaluate(const std::string& expression);
};

#endif // INTERPRETER_H