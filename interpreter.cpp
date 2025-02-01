#include "interpreter.h"

ThisFuncInterpreter::Function* ThisFuncInterpreter::findFunction(const std::string& name) {
        auto it = functions.find(name);
        return it != functions.end() ? &it->second : nullptr;
    }

    bool ThisFuncInterpreter::isSingleArgumentFunction(const std::string& functionName) {
        Function* function = findFunction(functionName);
        if (!function) {
            throw std::runtime_error("Unknown function: " + functionName);
        }

        // Extract placeholders from the function's expression
        std::vector<int> placeholders = extractPlaceholders(function->expression);

        // Ensure all placeholders refer to the same index
        if (!placeholders.empty()) {
            int firstPlaceholder = placeholders[0];
            for (int placeholder : placeholders) {
                if (placeholder != firstPlaceholder) {
                    return false; // Mixed placeholders (e.g., #0 and #1) found
                }
            }
        }

        return true; // All placeholders are consistent
    }


    double ThisFuncInterpreter::toDouble(const Value& value) const {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        throw std::runtime_error("Expected a scalar value, but got a list");
    }

    const std::vector<double>& ThisFuncInterpreter::toList(const Value& value) const {
        if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        }
        throw std::runtime_error("Expected a list, but got a scalar value");
    }

    ThisFuncInterpreter::ThisFuncInterpreter() {
        functions["add"] = {[](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("add requires exactly two arguments");
            return Value(std::get<double>(args[0]) + std::get<double>(args[1]));
        }, 2, ""};

        functions["sub"] = {[](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("sub requires exactly two arguments");
            return Value(std::get<double>(args[0]) - std::get<double>(args[1]));
        }, 2, ""};

        functions["mul"] = {[](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("mul requires exactly two arguments");
            return Value(std::get<double>(args[0]) * std::get<double>(args[1]));
        }, 2, ""};

        functions["div"] = {[](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("div requires exactly two arguments");
            if (std::get<double>(args[1]) == 0) throw std::runtime_error("Division by zero");
            return Value(std::get<double>(args[0]) / std::get<double>(args[1]));
        }, 2, ""};

        // pow function: Exponentiation
        functions["pow"] = {[](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("pow requires exactly two arguments");
            return Value(std::pow(std::get<double>(args[0]), std::get<double>(args[1])));
        }, 2, ""};

        // sqrt function: Square root
        functions["sqrt"] = {[](const std::vector<Value>& args) {
            if (args.size() != 1) throw std::runtime_error("sqrt requires exactly one argument");
            double value = std::get<double>(args[0]);
            if (value < 0) throw std::runtime_error("sqrt requires a non-negative argument");
            return Value(std::sqrt(value));
        }, 1, ""};

        // Trigonometric functions
        functions["sin"] = {[](const std::vector<Value>& args) {
            if (args.size() != 1) throw std::runtime_error("sin requires exactly one argument");
            return Value(std::sin(std::get<double>(args[0])));
        }, 1, ""};

        functions["cos"] = {[](const std::vector<Value>& args) {
            if (args.size() != 1) throw std::runtime_error("cos requires exactly one argument");
            return Value(std::cos(std::get<double>(args[0])));
        }, 1, ""};

        // Logical operations
        functions["nand"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("nand requires exactly two arguments");

            // Resolve both arguments
            double first = toDouble(std::holds_alternative<std::string>(args[0]) ? evaluate(std::get<std::string>(args[0])) : args[0]);
            double second = toDouble(std::holds_alternative<std::string>(args[1]) ? evaluate(std::get<std::string>(args[1])) : args[1]);

            return Value(static_cast<double>(!(first && second))); // Non-zero is treated as true
        }, 2, ""};

        functions["le"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("le requires exactly two arguments");

            // Resolve both arguments
            double first = toDouble(std::holds_alternative<std::string>(args[0]) ? std::get<std::string>(args[0]) : args[0]);
            double second = toDouble(std::holds_alternative<std::string>(args[1]) ? std::get<std::string>(args[1]) : args[1]);

            return Value(first <= second ? 1.0 : 0.0); // Return 1.0 for true, 0.0 for false
        }, 2, ""};

        functions["eq"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("eq requires exactly two arguments");

            // Resolve both arguments
            double first = toDouble(std::holds_alternative<std::string>(args[0]) ? std::get<std::string>(args[0]) : args[0]);
            double second = toDouble(std::holds_alternative<std::string>(args[1]) ? std::get<std::string>(args[1]) : args[1]);

            return Value(first == second ? 1.0 : 0.0); // Return 1.0 for true, 0.0 for false
        }, 2, ""};


        // Conditional operation (if)
        functions["if"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 3) throw std::runtime_error("if requires exactly three arguments");

            // Evaluate the condition
            double condition = toDouble(evaluate(std::get<std::string>(args[0])));

            // Depending on the condition, evaluate only the relevant branch
            if (condition != 0) {
                // Evaluate the `then` branch
                return evaluate(std::get<std::string>(args[1]));
            } else {
                // Evaluate the `else` branch
                return evaluate(std::get<std::string>(args[2]));
            }
        }, 3, ""};

        // List functions
        functions["list"] = {[this](const std::vector<Value>& args) {
            std::vector<double> list;
            for (const auto& arg : args) {
                list.push_back(toDouble(arg));
            }
            return Value(list); // Return the list directly
        }, 0, ""};

        functions["head"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 1) throw std::runtime_error("head requires exactly one argument");
            const auto& list = toList(args[0]);
            if (list.empty()) throw std::runtime_error("Cannot get head of an empty list");
            return Value(list[0]); // Return the first element as a scalar
        }, 1, ""};

        functions["tail"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 1) throw std::runtime_error("tail requires exactly one argument");

            const auto& list = toList(args[0]); // Ensure the argument is a list
            if (list.empty()) throw std::runtime_error("Cannot get tail of an empty list");

            // Return a new list excluding the first element
            return Value(std::vector<double>(list.begin() + 1, list.end()));
        }, 1, ""};

        functions["map"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("map requires exactly two arguments");

            // First argument: function reference
            if (!std::holds_alternative<std::string>(args[0])) {
                throw std::runtime_error("The first argument of map must be the name of a single-argument function");
            }
            std::string functionName = std::get<std::string>(args[0]);

            // Use helper to check if the function is single-argument
            if (!isSingleArgumentFunction(functionName)) {
                throw std::runtime_error("The first argument of map must be a single-argument function");
            }

            // Second argument: list
            const auto& list = toList(args[1]);

            // Apply the function to each element of the list
            std::vector<double> result;
            for (double elem : list) {
                std::vector<Value> singleArg = {Value(elem)};
                Value transformedValue = functions[functionName].implementation(singleArg);
                result.push_back(toDouble(transformedValue));
            }

            // Return the transformed list
            return Value(result); 
        }, 2, ""};

        functions["filter"] = {[this](const std::vector<Value>& args) {
            if (args.size() != 2) throw std::runtime_error("filter requires exactly two arguments");

            // First argument: predicate function
            if (!std::holds_alternative<std::string>(args[0])) {
                throw std::runtime_error("The first argument of filter must be the name of a single-argument predicate function");
            }
            std::string functionName = std::get<std::string>(args[0]);

            // Use helper to check if the function is single-argument
            if (!isSingleArgumentFunction(functionName)) {
                throw std::runtime_error("The first argument of filter must be a single-argument predicate function");
            }

            // Second argument: list
            const auto& list = toList(args[1]);

            // Apply the predicate function to filter the list
            std::vector<double> result;
            for (double elem : list) {
                std::vector<Value> singleArg = {Value(elem)};
                Value predicateResult = functions[functionName].implementation(singleArg);

                // Include the element if the predicate evaluates to true
                if (toDouble(predicateResult) != 0) { // Non-zero is treated as true
                    result.push_back(elem);
                }
            }

            //Returns the filtered list
            return result;
        }, 2, ""};
    }

    void ThisFuncInterpreter::declareFunction(const std::string& declaration) {
    size_t arrowPos = declaration.find("<-");
    if (arrowPos == std::string::npos) {
        throw std::runtime_error("Invalid function declaration syntax");
    }

    std::string functionName = trim(declaration.substr(0, arrowPos));
    std::string expression = trim(declaration.substr(arrowPos + 2));

    // Handle list declarations
    if (expression.find("list(") == 0) {
        size_t openParen = expression.find('(');
        size_t closeParen = expression.rfind(')');
        if (openParen == std::string::npos || closeParen == std::string::npos || closeParen <= openParen) {
            throw std::runtime_error("Invalid list declaration syntax");
        }
        std::string arguments = expression.substr(openParen + 1, closeParen - openParen - 1);
        std::vector<std::string> argStrings = splitArguments(arguments);
        std::vector<double> evaluatedArgs;
        for (const std::string &arg : argStrings) {
            evaluatedArgs.push_back(std::stod(arg));
        }
        declaredLists[functionName] = evaluatedArgs;
        functions[functionName] = {[functionName, this](const std::vector<Value>&) {
            const auto& list = declaredLists[functionName];
            return Value(list); // Return the list
        }, 0, ""};
    }

    // Handle parameterized functions (with recursion support)
    else if (expression.find("#") != std::string::npos) {
        // Temporarily register the function with a placeholder implementation
        functions[functionName] = {
            {},  // Placeholder implementation to allow self-references
            static_cast<size_t>(std::count(expression.begin(), expression.end(), '#')),
            expression // Store the original expression for debugging or validation
        };

        // Define the actual implementation
        functions[functionName].implementation = [this, functionName, expression](const std::vector<Value>& args) {
            // Substitute placeholders (#0, #1, etc.) with actual arguments
            std::string substitutedExpression = expression;
            for (size_t i = 0; i < args.size(); ++i) {
                std::string placeholder = "#" + std::to_string(i);
                size_t pos;
                while ((pos = substitutedExpression.find(placeholder)) != std::string::npos) {
                    substitutedExpression.replace(
                        pos, placeholder.size(), std::to_string(toDouble(args[i])));
                }
            }

            // Evaluate the substituted expression (supports recursion via functions map)
            return this->evaluate(substitutedExpression);
        };
    }

    // Handle constant functions
    else {
        double constantValue = std::stod(expression);
        functions[functionName] = {
            [constantValue](const std::vector<Value>&) {
                return Value(constantValue); // Return the scalar constant
            },
        0, ""};
    }
}


    Value ThisFuncInterpreter::evaluate(const std::string &expression) {
    std::string trimmedExpr = trim(expression);

    // Check if the expression is a number
    if (std::isdigit(trimmedExpr[0]) || (trimmedExpr[0] == '-' && std::isdigit(trimmedExpr[1]))) {
        return Value(std::stod(trimmedExpr));
    }

    // Check if the expression is a declared list
    auto it = declaredLists.find(trimmedExpr);
    if (it != declaredLists.end()) {
        return Value(it->second); // Return the list directly
    }

    // Check if the expression is a declared function
    Function* function = findFunction(trimmedExpr);
    if (function && trimmedExpr.find('(') == std::string::npos) {
        return Value(trimmedExpr); // Return the function name as a string if it's not a function call
    }

    // Parse function call syntax
    size_t openParen = trimmedExpr.find('(');
    size_t closeParen = trimmedExpr.rfind(')');

    if (openParen == std::string::npos || closeParen == std::string::npos || closeParen <= openParen) {
        throw std::runtime_error("Invalid function syntax");
    }

    std::string functionName = trim(trimmedExpr.substr(0, openParen));
    std::string arguments = trimmedExpr.substr(openParen + 1, closeParen - openParen - 1);

    function = findFunction(functionName);
    if (!function) {
        throw std::runtime_error("Unknown function: " + functionName);
    }

    // Parse arguments
    std::vector<std::string> argStrings = splitArguments(arguments);
    std::vector<Value> evaluatedArgs;

    if (functionName == "if") {
        // Pass arguments as raw strings for lazy evaluation
        for (const std::string &arg : argStrings) {
            evaluatedArgs.push_back(Value(arg)); // Store unevaluated expressions as strings
        }
    } else {
        // Evaluate arguments normally for other functions
        for (const std::string &arg : argStrings) {
            evaluatedArgs.push_back(evaluate(arg)); // Recursively evaluate arguments
        }
    }

    // Call the function
    return function->implementation(evaluatedArgs);
}