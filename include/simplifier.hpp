#pragma once

#include <string>

#include "ast.hpp"

class Simplifier {
public:
    NodePtr simplify(const Node& node) const;

private:
    NodePtr clone(const Node& node) const;

    bool tryGetNumber(const Node& node, double& value) const;
    bool isZero(const Node& node) const;
    bool isOne(const Node& node) const;
    bool sameTree(const Node& a, const Node& b) const;

    std::string formatNumber(double value) const;

    NodePtr makeNumber(const std::string& value) const;
    NodePtr makeNumber(double value) const;
    NodePtr makeVariable(const std::string& name) const;
    NodePtr makeUnary(UnaryOpType op, NodePtr operand) const;
    NodePtr makeBinary(BinaryOpType op, NodePtr left, NodePtr right) const;
    NodePtr makeFunction(const std::string& name, NodePtr argument) const;
};