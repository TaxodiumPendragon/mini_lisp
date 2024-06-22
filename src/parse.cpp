#include "parse.h"

#include <iostream>
#include <memory>

#include "./error.h"
#include "./value.h"

ValuePtr Parser::parseTails() {
    if (parseToken.front()->getType() == TokenType::RIGHT_PAREN) {
        parseToken.pop_front();               // 弹出这个词法标记
        return std::make_shared<NilValue>();  // 返回空表
    }
    auto car = this->parse();
    if (parseToken.front()->getType() == TokenType::DOT) {
        parseToken.pop_front();  // 弹出这个词法标记
        auto cdr = this->parse();
        if (parseToken.front()->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected ')'");
        }
        parseToken.pop_front();  // 再弹出一个词法标记，它应当是 ')'
        return std::make_shared<PairValue>(car, cdr);  // 返回对子 (car, cdr)
    } else {
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);  // 返回对子 (car, cdr)
    }
}

Parser::Parser(std::deque<TokenPtr> tokens) : parseToken(std::move(tokens)) {}

ValuePtr Parser::parse() {
    auto token = std::move(parseToken.front());
    parseToken.pop_front();
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    } else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    } else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    } else if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(value);  //
    } else if (token->getType() == TokenType::LEFT_PAREN) {
        return parseTails();
    } else if (token->getType() == TokenType::QUOTE) {
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()));  // 返回对子 (quote, (parse,
                                                 // nil))
    } else if (token->getType() == TokenType::QUASIQUOTE) {
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quasiquote"),
            std::make_shared<PairValue>(this->parse(),
                                        std::make_shared<NilValue>()));
    } else if (token->getType() == TokenType::UNQUOTE) {
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("unquote"),
            std::make_shared<PairValue>(this->parse(),
                                        std::make_shared<NilValue>()));
    }
    throw SyntaxError("Unimplemented");
}