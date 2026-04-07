#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "differentiator.hpp"
#include "simplifier.hpp"
#include "printer.hpp"

namespace {
    std::string toLower(std::string s) {
        for (char& c : s) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return s;
    }

    VariableTable readVariables(std::istream& in, int n) {
        std::vector<std::string> names(n);
        std::vector<double> values(n);

        for (int i = 0; i < n; ++i) {
            std::getline(in, names[i]);
            names[i] = toLower(names[i]);
        }

        for (int i = 0; i < n; ++i) {
            std::string line;
            std::getline(in, line);
            values[i] = std::stod(line);
        }

        VariableTable vars;
        for (int i = 0; i < n; ++i) {
            vars[names[i]] = values[i];
        }

        return vars;
    }
}

int main() {
    try {
        std::string command;
        std::getline(std::cin, command);
        command = toLower(command);

        std::string nLine;
        std::getline(std::cin, nLine);
        int n = std::stoi(nLine);

        VariableTable variables = readVariables(std::cin, n);

        std::string expression;
        std::getline(std::cin, expression);

        Lexer lexer(expression);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(std::move(tokens));
        NodePtr tree = parser.parse();

        if (command == "evaluate") {
            Evaluator evaluator;
            double value = evaluator.evaluate(*tree, variables);
            std::cout << value;
            return 0;
        }

        if (command == "derivative") {
            if (variables.empty()) {
                throw std::runtime_error("No variable provided for differentiation");
            }

            const std::string& diffVariable = variables.begin()->first;

            Differentiator differentiator;
            NodePtr derivativeTree = differentiator.differentiate(*tree, diffVariable);

            Simplifier simplifier;
            NodePtr simplifiedTree = simplifier.simplify(*derivativeTree);

            Printer printer;
            std::cout << printer.print(*simplifiedTree);
            return 0;
        }

        if (command == "evaluate_deriv8ative") {
            if (variables.empty()) {
                throw std::runtime_error("No variable provided for differentiation");
            }

            const std::string& diffVariable = variables.begin()->first;

            Differentiator differentiator;
            NodePtr derivativeTree = differentiator.differentiate(*tree, diffVariable);

            Simplifier simplifier;
            NodePtr simplifiedTree = simplifier.simplify(*derivativeTree);

            Evaluator evaluator;
            double value = evaluator.evaluate(*simplifiedTree, variables);

            std::cout << value;
            return 0;
        }

        throw std::runtime_error("Unknown command");
    }
    catch (...) {
        std::cout << "ERROR";
        return 0;
    }
}