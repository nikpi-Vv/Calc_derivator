#include "evaluator.hpp"

#include <cmath>
#include <string>

double Evaluator::evaluate(const Node& node, const VariableTable& variables) const {
    if (const auto* number = dynamic_cast<const NumberNode*>(&node)) {
        return std::stod(number->value());
    }

    if (const auto* variable = dynamic_cast<const VariableNode*>(&node)) {
        auto it = variables.find(variable->name());
        if (it == variables.end()) {
            throw EvalError("Unknown variable: " + variable->name());
        }
        return it->second;
    }

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        double value = evaluate(*unary->operand(), variables);

        switch (unary->op()) {
            case UnaryOpType::Plus:
                return value;
            case UnaryOpType::Minus:
                return -value;
        }

        throw EvalError("Unknown unary operator");
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        double left = evaluate(*binary->left(), variables);
        double right = evaluate(*binary->right(), variables);

        switch (binary->op()) {
            case BinaryOpType::Add:
                return left + right;
            case BinaryOpType::Subtract:
                return left - right;
            case BinaryOpType::Multiply:
                return left * right;
            case BinaryOpType::Divide:
                return left / right;
            case BinaryOpType::Power:
                return std::pow(left, right);
        }

        throw EvalError("Unknown binary operator");
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        double arg = evaluate(*call->argument(), variables);
        return evaluateFunction(call->function_name(), arg);
    }

    throw EvalError("Unknown AST node");
}

double Evaluator::evaluateFunction(const std::string& name, double arg) const {
    if (name == "sin") {
        return std::sin(arg);
    }
    if (name == "cos") {
        return std::cos(arg);
    }
    if (name == "tan") {
        return std::tan(arg);
    }
    if (name == "asin") {
        return std::asin(arg);
    }
    if (name == "acos") {
        return std::acos(arg);
    }
    if (name == "atan") {
        return std::atan(arg);
    }
    if (name == "exp") {
        return std::exp(arg);
    }
    if (name == "log") {
        return std::log(arg);
    }
    if (name == "sqrt") {
        return std::sqrt(arg);
    }

    throw EvalError("Unknown function: " + name);
}