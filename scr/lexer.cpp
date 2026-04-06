#include "lexer.hpp"

void Lexer::skipSpaces() {
    while (!eof() && isSpace(peek())) {
        get();
    }
}

Token Lexer::makeToken(TokenType t, std::size_t start) {
    return Token{t, s_.substr(start, i_ - start), start};
}