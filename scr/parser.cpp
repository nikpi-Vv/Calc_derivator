#include "parser.hpp"

const Token& Parser::peek() const {
    return tokens_[pos_];
}

const Token& Parser::get() {
    return tokens_[pos_++];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        get();
        return true;
    }
    return false;
}

NodePtr Parser::parse() {
    NodePtr result = parseExpression();

    if (peek().type != TokenType::End) {
        throw ParseError("Unexpected token after end of expression");
    }

    return result;
}

NodePtr Parser::parseExpression() {
    NodePtr left = parseTerm();

    while (true) {
        if (match(TokenType::Plus)) {
            NodePtr right = parseTerm();
            left = std::make_unique<BinaryOpNode>(
                BinaryOpType::Add,
                std::move(left),
                std::move(right)
            );
        } else if (match(TokenType::Minus)) {
            NodePtr right = parseTerm();
            left = std::make_unique<BinaryOpNode>(
                BinaryOpType::Subtract,
                std::move(left),
                std::move(right)
            );
        } else {
            break;
        }
    }

    return left;
}

NodePtr Parser::parseTerm() {
    NodePtr left = parseUnary();

    while (true) {
        if (match(TokenType::Star)) {
            NodePtr right = parseUnary();
            left = std::make_unique<BinaryOpNode>(
                BinaryOpType::Multiply,
                std::move(left),
                std::move(right)
            );
        } else if (match(TokenType::Slash)) {
            NodePtr right = parseUnary();
            left = std::make_unique<BinaryOpNode>(
                BinaryOpType::Divide,
                std::move(left),
                std::move(right)
            );
        } else {
            break;
        }
    }

    return left;
}

NodePtr Parser::parseUnary() {
    if (match(TokenType::Plus)) {
        return std::make_unique<UnaryOpNode>(
            UnaryOpType::Plus,
            parseUnary()
        );
    }

    if (match(TokenType::Minus)) {
        return std::make_unique<UnaryOpNode>(
            UnaryOpType::Minus,
            parseUnary()
        );
    }

    return parsePower();
}

NodePtr Parser::parsePower() {
    NodePtr left = parsePrimary();

    if (match(TokenType::Caret)) {
        NodePtr right = parseUnary();
        return std::make_unique<BinaryOpNode>(
            BinaryOpType::Power,
            std::move(left),
            std::move(right)
        );
    }

    return left;
}

NodePtr Parser::parsePrimary() {
    if (match(TokenType::LParen)) {
        NodePtr inside = parseExpression();

        if (!match(TokenType::RParen)) {
            throw ParseError("Expected ')'");
        }

        return inside;
    }

    if (peek().type == TokenType::Number) {
        std::string value = peek().lexeme;
        get();
        return std::make_unique<NumberNode>(value);
    }

    if (peek().type == TokenType::Identifier) {
        std::string name = peek().lexeme;
        get();

        if (match(TokenType::LParen)) {
            NodePtr arg = parseExpression();

            if (!match(TokenType::RParen)) {
                throw ParseError("Expected ')' after function argument");
            }

            return std::make_unique<FunctionCallNode>(
                name,
                std::move(arg)
            );
        }

        return std::make_unique<VariableNode>(name);
    }

    throw ParseError("Expected number, variable, function or '('");
}