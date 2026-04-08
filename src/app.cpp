#include "app.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "differentiator.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "simplifier.hpp"

namespace {
std::string toLower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

const std::unordered_set<std::string>& builtInFunctions() {
    static const std::unordered_set<std::string> names = {
        "sin", "cos", "tan", "asin", "acos", "atan", "exp", "log", "sqrt"
    };
    return names;
}

std::vector<std::string> readNames(std::istream& in, int n) {
    std::vector<std::string> names;
    names.reserve(n);

    while (static_cast<int>(names.size()) < n) {
        std::string line;
        if (!std::getline(in, line)) {
            throw std::runtime_error("Not enough variable names");
        }

        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            token = toLower(token);
            if (builtInFunctions().count(token) != 0U) {
                throw std::runtime_error("Variable name conflicts with built-in function: " + token);
            }
            names.push_back(token);
            if (static_cast<int>(names.size()) == n) {
                break;
            }
        }
    }

    return names;
}

std::vector<double> readValues(std::istream& in, int n) {
    std::vector<double> values;
    values.reserve(n);

    while (static_cast<int>(values.size()) < n) {
        std::string line;
        if (!std::getline(in, line)) {
            throw std::runtime_error("Not enough variable values");
        }

        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            values.push_back(std::stod(token));
            if (static_cast<int>(values.size()) == n) {
                break;
            }
        }
    }

    return values;
}
}  // namespace

int runApplication(std::istream& in, std::ostream& out) {
    try {
        std::string command;
        if (!std::getline(in, command)) {
            throw std::runtime_error("Empty input");
        }
        command = toLower(command);

        std::string nLine;
        if (!std::getline(in, nLine)) {
            throw std::runtime_error("Missing variable count");
        }
        int n = std::stoi(nLine);
        if (n < 0) {
            throw std::runtime_error("Variable count must be non-negative");
        }

        std::vector<std::string> names = readNames(in, n);
        std::vector<double> values = readValues(in, n);

        VariableTable variables;
        for (int i = 0; i < n; ++i) {
            variables[names[i]] = values[i];
        }

        std::string expression;
        if (!std::getline(in, expression)) {
            throw std::runtime_error("Missing expression");
        }

        Lexer lexer(expression);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(std::move(tokens));
        NodePtr tree = parser.parse();

        if (command == "evaluate") {
            Evaluator evaluator;
            out << std::setprecision(15) << evaluator.evaluate(*tree, variables);
            return 0;
        }

        if (command == "derivative") {
            if (n == 0) {
                throw std::runtime_error("No variable provided for differentiation");
            }

            Differentiator differentiator;
            NodePtr derivativeTree = differentiator.differentiate(*tree, names.front());

            Simplifier simplifier;
            NodePtr simplifiedTree = simplifier.simplify(*derivativeTree);

            Printer printer;
            out << printer.print(*simplifiedTree);
            return 0;
        }

        if (command == "evaluate_derivative") {
            if (n == 0) {
                throw std::runtime_error("No variable provided for differentiation");
            }

            Differentiator differentiator;
            NodePtr derivativeTree = differentiator.differentiate(*tree, names.front());

            Simplifier simplifier;
            NodePtr simplifiedTree = simplifier.simplify(*derivativeTree);

            Evaluator evaluator;
            out << std::setprecision(15) << evaluator.evaluate(*simplifiedTree, variables);
            return 0;
        }

        throw std::runtime_error("Unknown command");
    } catch (const std::exception& e) {
        out << "ERROR " << e.what();
        return 0;
    }
}