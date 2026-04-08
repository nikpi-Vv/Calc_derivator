#include "evaluator.hpp"

#include <cmath>
#include <limits>
#include <string>

namespace {
void ensureFinite(double value, const std::string& context) {
    if (!std::isfinite(value)) {
        throw EvalError("Domain error: " + context);
    }
}
}  // namespace

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
            case BinaryOpType::Power: {
                double value = std::pow(left, right);
                ensureFinite(value, "pow(" + std::to_string(left) + ", " + std::to_string(right) + ")");
                return value;
            }
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
        if (arg < -1.0 || arg > 1.0) {
            throw EvalError("Domain error: asin(" + std::to_string(arg) + ")");
        }
        return std::asin(arg);
    }
    if (name == "acos") {
        if (arg < -1.0 || arg > 1.0) {
            throw EvalError("Domain error: acos(" + std::to_string(arg) + ")");
        }
        return std::acos(arg);
    }
    if (name == "atan") {
        return std::atan(arg);
    }
    if (name == "exp") {
        double value = std::exp(arg);
        ensureFinite(value, "exp(" + std::to_string(arg) + ")");
        return value;
    }
    if (name == "log") {
        if (arg < 0.0) {
            throw EvalError("Domain error: log(" + std::to_string(arg) + ")");
        }
        return std::log(arg); // log(0) -> -inf
    }
    if (name == "sqrt") {
        if (arg < 0.0) {
            throw EvalError("Domain error: sqrt(" + std::to_string(arg) + ")");
        }
        return std::sqrt(arg);
    }

    throw EvalError("Unknown function: " + name);
}
