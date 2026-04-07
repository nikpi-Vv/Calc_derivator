#pragma once

#include <memory>
#include <string>
#include <stdexcept>

#include "ast.hpp"

class DiffError : public std::runtime_error {
public:
    explicit DiffError(const std::string& msg)
        : std::runtime_error(msg) {}
};

class Differentiator {
public:
    NodePtr differentiate(const Node& node, const std::string& variable) const;

private:
    NodePtr clone(const Node& node) const;

    NodePtr differentiateFunction(const FunctionCallNode& call, const std::string& variable) const;

    NodePtr makeNumber(const std::string& value) const;
    NodePtr makeVariable(const std::string& name) const;

    NodePtr makeUnary(UnaryOpType op, NodePtr operand) const;
    NodePtr makeBinary(BinaryOpType op, NodePtr left, NodePtr right) const;
    NodePtr makeFunction(const std::string& name, NodePtr argument) const;
};