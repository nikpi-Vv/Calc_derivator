#include "simplifier.hpp"

#include <memory>
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

        switch (binary->op()) {
            case BinaryOpType::Add:
                if (isNumber(*left, "0")) return right;
                if (isNumber(*right, "0")) return left;
                return makeBinary(BinaryOpType::Add, std::move(left), std::move(right));

            case BinaryOpType::Subtract:
                if (isNumber(*right, "0")) return left;
                if (isNumber(*left, "0")) {
                    return makeUnary(UnaryOpType::Minus, std::move(right));
                }
                return makeBinary(BinaryOpType::Subtract, std::move(left), std::move(right));

            case BinaryOpType::Multiply:
                if (isNumber(*left, "0") || isNumber(*right, "0")) {
                    return makeNumber("0");
                }
                if (isNumber(*left, "1")) return right;
                if (isNumber(*right, "1")) return left;
                return makeBinary(BinaryOpType::Multiply, std::move(left), std::move(right));

            case BinaryOpType::Divide:
                if (isNumber(*left, "0")) return makeNumber("0");
                if (isNumber(*right, "1")) return left;
                return makeBinary(BinaryOpType::Divide, std::move(left), std::move(right));

            case BinaryOpType::Power:
                if (isNumber(*right, "1")) return left;
                if (isNumber(*right, "0")) return makeNumber("1");
                if (isNumber(*left, "1")) return makeNumber("1");
                if (isNumber(*left, "0")) return makeNumber("0");
                return makeBinary(BinaryOpType::Power, std::move(left), std::move(right));
        }
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        return makeFunction(call->function_name(), simplify(*call->argument()));
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

bool Simplifier::isNumber(const Node& node, const std::string& value) const {
    const auto* number = dynamic_cast<const NumberNode*>(&node);
    return number != nullptr && number->value() == value;
}

NodePtr Simplifier::makeNumber(const std::string& value) const {
    return std::make_unique<NumberNode>(value);
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