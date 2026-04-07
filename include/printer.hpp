#pragma once

#include <string>

#include "ast.hpp"

class Printer {
public:
    std::string print(const Node& node) const;

private:
    std::string printNode(const Node& node, int parentPriority, bool isRightChild) const;

    int priority(const Node& node) const;
    int priority(BinaryOpType op) const;
    int priority(UnaryOpType op) const;

    std::string binaryOpToString(BinaryOpType op) const;
    std::string unaryOpToString(UnaryOpType op) const;
};