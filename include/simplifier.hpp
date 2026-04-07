#pragma once

#include "ast.hpp"

class Simplifier {
public:
    NodePtr simplify(const Node& node) const;

private:
    NodePtr clone(const Node& node) const;

    bool isNumber(const Node& node, const std::string& value) const;

    NodePtr makeNumber(const std::string& value) const;
    NodePtr makeVariable(const std::string& name) const;
    NodePtr makeUnary(UnaryOpType op, NodePtr operand) const;
    NodePtr makeBinary(BinaryOpType op, NodePtr left, NodePtr right) const;
    NodePtr makeFunction(const std::string& name, NodePtr argument) const;
};