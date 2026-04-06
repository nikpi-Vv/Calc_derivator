#include "lexer.hpp"

void Lexer::skipSpaces() {
    while (!eof() && isSpace(peek())) {
        get();
    }
}

Token Lexer::makeToken(TokenType t, std::size_t start) {
    return Token{t, s_.substr(start, i_ - start), start};
}

Token Lexer::lexIdentifier() {
    // FSM: ID_START -> ID_BODY
    std::size_t start = i_;

    // первый символ: буква или '_'
    char c = peek();
    if (!(isAlpha(c) || c == '_')) {
        throw LexError("Internal lexer error: lexIdentifier called at non-identifier position");
    }
    get();

     // остальные: буква/цифра/'_'
     while (!eof()) {
        char p = peek();
        if (isAlnum(p) || p == '_') get();
        else break;
     }

     return makeToken(TokenType::Identifier, start);
}

Token Lexer::lexNumber() {
    // Поддержка: INT, FRAC, EXP (12e-10)
    // Строго: запрещаем ведущие нули вида 0123

    std::size_t start = i_;

    // N_INT: читаем целую часть (должна начинаться с цифры)
    if (!isDigit(peek())) {
        throw LexError("Internal lexer error: lexNumber called at non-digit position");
    }

    //ведущий ноль
    if(peek() == '0') {
        get(); // съели "0"

        // если после 0 сразу цифра -> запрещаем (0123)
        if (isDigit(peek())) {
            throw LexError("Invalid number with leading zero at position " + std::to_string(start));
        }
    } else {
        while (isDigit(peek())) get();
    }

    //N_EXP
    if(peek() == 'e' || peek() == 'E') {
        get(); // 'e'/'E'

        // N_EXP_SIGN (необязательно)
        if (peek() == '+' || peek() == '-') {
            get();
        }
    

    // должны быть цифры экспоненты
        if (!isDigit(peek())) {
            throw LexError("Invalid exponent at position " + std::to_string(i_ - 1));
        }
        while (isDigit(peek())) get();
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

        //NUMBER
        if (isdigit(c)) {
            out.push_back(lexNumber());
            continue;
        }

        // IDENTIFIER (имя переменной или функции)
        if (isAlpha(c) || c == '_') {
            out.push_back(lexIdentifier());
            continue;
        }

        // single-char tokens
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