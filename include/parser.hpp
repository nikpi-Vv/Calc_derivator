#pragma once
#include <vector>
#include <stdexcept>

#include "lexer.hpp"
#include "ast.hpp"

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg)
        : std::runtime_error(msg) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : tokens_(std::move(tokens)), pos_(0) {}

    NodePtr parse();

private:
    const Token& peek() const;
    const Token& get();
    bool match(TokenType type);

    NodePtr parseExpression();
    NodePtr parseTerm();
    NodePtr parseUnary();
    NodePtr parsePower();
    NodePtr parsePrimary();

private:
    std::vector<Token> tokens_;
    std::size_t pos_;
};