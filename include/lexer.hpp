#pragma once
#include <string>
#include <vector>
#include <stdexcept>//исключения
#include <cctype>//проверка символов

enum class TokenType { //enum-набор возможных значений, 
    //исп: TokenType t = TokenType::Number;
    Number,
    Identifier,

    Plus, Minus, Start, Slash, Caret,
    LParen, RParen,

    End
}; // ← после определения типа

struct Token { //контейнер данных
    TokenType type;
    std::string lexeme;
    std::size_t pos;
};

class LexError : public std::runtime_error {
public:
    explicit LexError(const std::string& msg) : std::runtime_error(msg) {}
};

class Lexer{
public:
    explicit Lexer(std::string input)
        : s_(std::move(input)), i_(0) {}
    
        std::vector<Token> tokenize();
private:
    //базовые утилиты (от копипаста)
    bool eof() const { return i_ >= s_.size(); }    
    char peek() const { return eof() ? '\0' : s_[i_]; }
    char peekNext() const { return (i_ + 1 < s_.size()) ? s_[i_ + 1] : '\0'; }
    char get() { return eof() ? '\0' : s_[i_++]; }

    static bool isDigit(char c) {
        return std::isdigit(static_cast<unsigned char>(c)) !=0;
    }
    static bool isAlpha(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) != 0;
    }
    static bool isAlnum(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) != 0;
    }
    static bool isSpace(char c) {
        return std::isspace(static_cast<unsigned char>(c)) != 0;
    }

    void skipSpaces();

    Token lexNumber();
    Token lexIdentifier();

    Token makeToken(TokenType t, std::size_t start);

private:
    std::string s_;
    std::size_t i_;
};
