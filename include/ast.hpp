#pragma once
#include <memory>
#include <string>
/*AST нужен, чтобы после синтаксического анализа хранить выражение в 
структурированном виде. Это упрощает вычисление, дифференцирование, 
упрощение выражений и повторный вывод с правильным приоритетом операций.*/

enum class UnaryOpType {
    Plus,
    Minus
};

enum class BinaryOpType {
    Add,
    Substract,
    Multiply,
    Divide,
    Power
};

class Node {
public:
    virtual ~Node() = default;
};

using NodePtr = std::unique_ptr<Node>;

class NumberNode : public Node {
public:
    explicit NumberNode(std::string value)
        : value_(std::move(value)) {}

    const std::string& value() const {
        return value_;
    }
private:
    std::string value_;
};

class VariableNode : public Node {
public:
    explicit VariableNode(std::string name)
        : name_(std::move(name)) {}

    const std::string& name() const {
        return name_;
    }
private:
    std::string name_;
};

class UnaryOpNode : public Node {
public:
    UnaryOpNode(UnaryOpType op, NodePtr operand)
        : op_(op), operand_(std::move(operand)) {}

    UnaryOpType op() const {
        return op_;
    }

    const Node* operand() const {
        return operand_.get();
    }

    Node* operand() {
        return operand_.get();
    }
private: 
    UnaryOpType op_;
    NodePtr operand_;
};

class BinaryOpNode : public Node {
public:
    BinaryOpNode(BinaryOpType op, NodePtr left, NodePtr right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}

    BinaryOpType op() const {
        return op_;
    }

    const Node* left() const {
        return left_.get();
    }

    const Node* right() const {
        return right_.get();
    }

    Node* left() {
        return left_.get();
    }

    Node* right() {
        return right_.get();
    }

private:
    BinaryOpType op_;
    NodePtr left_;
    NodePtr right_;
};

class FunctionCallNode : public Node {
public:
    FunctionCallNode(std::string function_name, NodePtr argument)
        : function_name_(std::move(function_name)), argument_(std::move(argument)) {}

    const std::string& function_name() const {
        return function_name_;
    }

    const Node* argument() const {
        return argument_.get();
    }

    Node* argument() {
        return argument_.get();
    }

private:
    std::string function_name_;
    NodePtr argument_;
};
