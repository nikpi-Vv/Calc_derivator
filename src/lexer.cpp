#include "lexer.hpp"

#include <cctype>

namespace {
std::string toLower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}
}  // namespace

void Lexer::skipSpaces() {
    while (!eof() && isSpace(peek())) {
        get();
    }
}

Token Lexer::makeToken(TokenType t, std::size_t start) {
    return Token{t, s_.substr(start, i_ - start), start};
}

Token Lexer::lexIdentifier() {
    std::size_t start = i_;

    char c = peek();
    if (!(isAlpha(c) || c == '_')) {
        throw LexError("Internal lexer error: lexIdentifier called at non-identifier position");
    }
    get();

    while (!eof()) {
        char p = peek();
        if (isAlnum(p) || p == '_') {
            get();
        } else {
            break;
        }
    }

    Token token = makeToken(TokenType::Identifier, start);
    token.lexeme = toLower(token.lexeme);
    return token;
}

Token Lexer::lexNumber() {
    std::size_t start = i_;

    if (!isDigit(peek())) {
        throw LexError("Internal lexer error: lexNumber called at non-digit position");
    }

    if (peek() == '0') {
        get();
        if (isDigit(peek())) {
            throw LexError("Invalid number with leading zero at position " + std::to_string(start));
        }
    } else {
        while (isDigit(peek())) {
            get();
        }
    }

    if (peek() == '.') {
        get();
        if (!isDigit(peek())) {
            throw LexError("Invalid fractional part at position " + std::to_string(i_ - 1));
        }
        while (isDigit(peek())) {
            get();
        }
    }

    if (peek() == 'e' || peek() == 'E') {
        get();

        if (peek() == '+' || peek() == '-') {
            get();
        }

        if (!isDigit(peek())) {
            throw LexError("Invalid exponent at position " + std::to_string(i_ - 1));
        }

        while (isDigit(peek())) {
            get();
        }
    }

    return makeToken(TokenType::Number, start);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> out;

    while (true) {
        skipSpaces();
        std::size_t pos = i_;

        if (eof()) {
            out.push_back(Token{TokenType::End, "", i_});
            break;
        }

        char c = peek();

        if (isDigit(c)) {
            out.push_back(lexNumber());
            continue;
        }

        if (isAlpha(c) || c == '_') {
            out.push_back(lexIdentifier());
            continue;
        }

        switch (c) {
            case '+': get(); out.push_back(Token{TokenType::Plus, "+", pos}); break;
            case '-': get(); out.push_back(Token{TokenType::Minus, "-", pos}); break;
            case '*': get(); out.push_back(Token{TokenType::Star, "*", pos}); break;
            case '/': get(); out.push_back(Token{TokenType::Slash, "/", pos}); break;
            case '^': get(); out.push_back(Token{TokenType::Caret, "^", pos}); break;
            case '(': get(); out.push_back(Token{TokenType::LParen, "(", pos}); break;
            case ')': get(); out.push_back(Token{TokenType::RParen, ")", pos}); break;
            default:
                throw LexError(std::string("Unexpected character '") + c +
                               "' at position " + std::to_string(pos));
        }
    }

    return out;
}
