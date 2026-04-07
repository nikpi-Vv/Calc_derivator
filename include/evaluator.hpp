#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "ast.hpp"

class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg)
        : std::runtime_error(msg) {}
};

using VariableTable = std::unordered_map<std::string, double>;

class Evaluator {
public:
    double evaluate(const Node& node, const VariableTable& variables) const;

private:
    double evaluateFunction(const std::string& name, double arg) const;
};