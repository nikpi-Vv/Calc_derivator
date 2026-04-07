#include "simplifier.hpp"

#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>
#include <utility>

NodePtr Simplifier::simplify(const Node& node) const {
    if (const auto* number = dynamic_cast<const NumberNode*>(&node)) {
        return makeNumber(number->value());
    }

    if (const auto* variable = dynamic_cast<const VariableNode*>(&node)) {
        return makeVariable(variable->name());
    }

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        NodePtr operand = simplify(*unary->operand());

        double operandValue = 0.0;
        if (tryGetNumber(*operand, operandValue)) {
            if (unary->op() == UnaryOpType::Plus) {
                return makeNumber(operandValue);
            }
            if (unary->op() == UnaryOpType::Minus) {
                return makeNumber(-operandValue);
            }
        }

        if (unary->op() == UnaryOpType::Plus) {
            return operand;
        }

        if (unary->op() == UnaryOpType::Minus) {
            if (const auto* innerUnary = dynamic_cast<const UnaryOpNode*>(operand.get())) {
                if (innerUnary->op() == UnaryOpType::Minus) {
                    return simplify(*innerUnary->operand());
                }
            }

            return makeUnary(UnaryOpType::Minus, std::move(operand));
        }
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        NodePtr left = simplify(*binary->left());
        NodePtr right = simplify(*binary->right());

        double leftValue = 0.0;
        double rightValue = 0.0;
        bool leftIsNumber = tryGetNumber(*left, leftValue);
        bool rightIsNumber = tryGetNumber(*right, rightValue);

        switch (binary->op()) {
            case BinaryOpType::Add:
                if (leftIsNumber && rightIsNumber) {
                    return makeNumber(leftValue + rightValue);
                }
                if (isZero(*left)) return right;
                if (isZero(*right)) return left;
                return makeBinary(BinaryOpType::Add, std::move(left), std::move(right));

            case BinaryOpType::Subtract:
                if (leftIsNumber && rightIsNumber) {
                    return makeNumber(leftValue - rightValue);
                }
                if (isZero(*right)) return left;
                if (isZero(*left)) {
                    return makeUnary(UnaryOpType::Minus, std::move(right));
                }
                if (sameTree(*left, *right)) {
                    return makeNumber("0");
                }
                return makeBinary(BinaryOpType::Subtract, std::move(left), std::move(right));

            case BinaryOpType::Multiply:
                if (leftIsNumber && rightIsNumber) {
                    return makeNumber(leftValue * rightValue);
                }
                if (isZero(*left) || isZero(*right)) {
                    return makeNumber("0");
                }
                if (isOne(*left)) return right;
                if (isOne(*right)) return left;
                if (leftIsNumber && leftValue == -1.0) {
                    return makeUnary(UnaryOpType::Minus, std::move(right));
                }
                if (rightIsNumber && rightValue == -1.0) {
                    return makeUnary(UnaryOpType::Minus, std::move(left));
                }
                return makeBinary(BinaryOpType::Multiply, std::move(left), std::move(right));

            case BinaryOpType::Divide:
                if (leftIsNumber && rightIsNumber) {
                    return makeNumber(leftValue / rightValue);
                }
                if (isZero(*left)) return makeNumber("0");
                if (isOne(*right)) return left;
                
                return makeBinary(BinaryOpType::Divide, std::move(left), std::move(right));

            case BinaryOpType::Power:
                if (leftIsNumber && rightIsNumber) {
                    return makeNumber(std::pow(leftValue, rightValue));
                }
                if (isZero(*right)) return makeNumber("1");
                if (isOne(*right)) return left;
                if (isOne(*left)) return makeNumber("1");
                if (isZero(*left)) return makeNumber("0");
                return makeBinary(BinaryOpType::Power, std::move(left), std::move(right));
        }
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        NodePtr arg = simplify(*call->argument());

        double argValue = 0.0;
        if (tryGetNumber(*arg, argValue)) {
            if (call->function_name() == "sin") return makeNumber(std::sin(argValue));
            if (call->function_name() == "cos") return makeNumber(std::cos(argValue));
            if (call->function_name() == "tan") return makeNumber(std::tan(argValue));
            if (call->function_name() == "asin") return makeNumber(std::asin(argValue));
            if (call->function_name() == "acos") return makeNumber(std::acos(argValue));
            if (call->function_name() == "atan") return makeNumber(std::atan(argValue));
            if (call->function_name() == "exp") return makeNumber(std::exp(argValue));
            if (call->function_name() == "log") return makeNumber(std::log(argValue));
            if (call->function_name() == "sqrt") return makeNumber(std::sqrt(argValue));
        }

        return makeFunction(call->function_name(), std::move(arg));
    }

    return clone(node);
}

NodePtr Simplifier::clone(const Node& node) const {
    if (const auto* number = dynamic_cast<const NumberNode*>(&node)) {
        return makeNumber(number->value());
    }

    if (const auto* variable = dynamic_cast<const VariableNode*>(&node)) {
        return makeVariable(variable->name());
    }

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        return makeUnary(unary->op(), clone(*unary->operand()));
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        return makeBinary(binary->op(), clone(*binary->left()), clone(*binary->right()));
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        return makeFunction(call->function_name(), clone(*call->argument()));
    }

    return nullptr;
}

bool Simplifier::tryGetNumber(const Node& node, double& value) const {
    const auto* number = dynamic_cast<const NumberNode*>(&node);
    if (number == nullptr) {
        return false;
    }

    value = std::stod(number->value());
    return true;
}

bool Simplifier::isZero(const Node& node) const {
    double value = 0.0;
    return tryGetNumber(node, value) && value == 0.0;
}

bool Simplifier::isOne(const Node& node) const {
    double value = 0.0;
    return tryGetNumber(node, value) && value == 1.0;
}

bool Simplifier::sameTree(const Node& a, const Node& b) const {
    if (typeid(a) != typeid(b)) {
        return false;
    }

    if (const auto* aNumber = dynamic_cast<const NumberNode*>(&a)) {
        const auto* bNumber = dynamic_cast<const NumberNode*>(&b);
        return aNumber->value() == bNumber->value();
    }

    if (const auto* aVariable = dynamic_cast<const VariableNode*>(&a)) {
        const auto* bVariable = dynamic_cast<const VariableNode*>(&b);
        return aVariable->name() == bVariable->name();
    }

    if (const auto* aUnary = dynamic_cast<const UnaryOpNode*>(&a)) {
        const auto* bUnary = dynamic_cast<const UnaryOpNode*>(&b);
        return aUnary->op() == bUnary->op() &&
               sameTree(*aUnary->operand(), *bUnary->operand());
    }

    if (const auto* aBinary = dynamic_cast<const BinaryOpNode*>(&a)) {
        const auto* bBinary = dynamic_cast<const BinaryOpNode*>(&b);
        return aBinary->op() == bBinary->op() &&
               sameTree(*aBinary->left(), *bBinary->left()) &&
               sameTree(*aBinary->right(), *bBinary->right());
    }

    if (const auto* aCall = dynamic_cast<const FunctionCallNode*>(&a)) {
        const auto* bCall = dynamic_cast<const FunctionCallNode*>(&b);
        return aCall->function_name() == bCall->function_name() &&
               sameTree(*aCall->argument(), *bCall->argument());
    }

    return false;
}

std::string Simplifier::formatNumber(double value) const {
    if (value == 0.0) {
        return "0";
    }

    std::ostringstream out;
    out << std::setprecision(15) << value;
    std::string s = out.str();

    if (s.find('.') != std::string::npos) {
        while (!s.empty() && s.back() == '0') {
            s.pop_back();
        }
        if (!s.empty() && s.back() == '.') {
            s.pop_back();
        }
    }

    return s;
}

NodePtr Simplifier::makeNumber(const std::string& value) const {
    return std::make_unique<NumberNode>(value);
}

NodePtr Simplifier::makeNumber(double value) const {
    return std::make_unique<NumberNode>(formatNumber(value));
}

NodePtr Simplifier::makeVariable(const std::string& name) const {
    return std::make_unique<VariableNode>(name);
}

NodePtr Simplifier::makeUnary(UnaryOpType op, NodePtr operand) const {
    return std::make_unique<UnaryOpNode>(op, std::move(operand));
}

NodePtr Simplifier::makeBinary(BinaryOpType op, NodePtr left, NodePtr right) const {
    return std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
}

NodePtr Simplifier::makeFunction(const std::string& name, NodePtr argument) const {
    return std::make_unique<FunctionCallNode>(name, std::move(argument));
}