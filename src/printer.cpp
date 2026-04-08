#include "printer.hpp"

#include <stdexcept>

std::string Printer::print(const Node& node) const {
    return printNode(node, -1, false);
}

std::string Printer::printNode(const Node& node, int parentPriority, bool /*isRightChild*/) const {
    if (const auto* number = dynamic_cast<const NumberNode*>(&node)) {
        return number->value();
    }

    if (const auto* variable = dynamic_cast<const VariableNode*>(&node)) {
        return variable->name();
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        return call->function_name() + "(" + printNode(*call->argument(), -1, false) + ")";
    }

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        std::string result = unaryOpToString(unary->op()) +
                             printNode(*unary->operand(), priority(unary->op()), true);

        if (priority(unary->op()) < parentPriority) {
            return "(" + result + ")";
        }
        return result;
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        int currentPriority = priority(binary->op());

        std::string left = printNode(*binary->left(), currentPriority, false);
        std::string right = printNode(*binary->right(), currentPriority, true);

        bool needParensAroundRight = false;
        if (const auto* rightBinary = dynamic_cast<const BinaryOpNode*>(binary->right())) {
            int rightPriority = priority(rightBinary->op());

            if (binary->op() == BinaryOpType::Subtract && rightPriority == currentPriority) {
                needParensAroundRight = true;
            }

            if (binary->op() == BinaryOpType::Divide && rightPriority == currentPriority) {
                needParensAroundRight = true;
            }

            if (binary->op() == BinaryOpType::Power && rightPriority == currentPriority) {
                needParensAroundRight = true;
            }
        }

        if (needParensAroundRight) {
            right = "(" + right + ")";
        }

        std::string result = left + " " + binaryOpToString(binary->op()) + " " + right;

        bool needParens = false;
        if (currentPriority < parentPriority) {
            needParens = true;
        }

        if (needParens) {
            return "(" + result + ")";
        }

        return result;
    }

    throw std::runtime_error("Unknown AST node in printer");
}

int Printer::priority(const Node& node) const {
    if (dynamic_cast<const NumberNode*>(&node)) return 100;
    if (dynamic_cast<const VariableNode*>(&node)) return 100;
    if (dynamic_cast<const FunctionCallNode*>(&node)) return 100;

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        return priority(unary->op());
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        return priority(binary->op());
    }

    return -1;
}

int Printer::priority(BinaryOpType op) const {
    switch (op) {
        case BinaryOpType::Add:
        case BinaryOpType::Subtract:
            return 10;

        case BinaryOpType::Multiply:
        case BinaryOpType::Divide:
            return 20;

        case BinaryOpType::Power:
            return 30;
    }

    return -1;
}

int Printer::priority(UnaryOpType op) const {
    switch (op) {
        case UnaryOpType::Plus:
        case UnaryOpType::Minus:
            return 25;
    }

    return -1;
}

std::string Printer::binaryOpToString(BinaryOpType op) const {
    switch (op) {
        case BinaryOpType::Add:
            return "+";
        case BinaryOpType::Subtract:
            return "-";
        case BinaryOpType::Multiply:
            return "*";
        case BinaryOpType::Divide:
            return "/";
        case BinaryOpType::Power:
            return "^";
    }

    throw std::runtime_error("Unknown binary operator");
}

std::string Printer::unaryOpToString(UnaryOpType op) const {
    switch (op) {
        case UnaryOpType::Plus:
            return "+";
        case UnaryOpType::Minus:
            return "-";
    }

    throw std::runtime_error("Unknown unary operator");
}