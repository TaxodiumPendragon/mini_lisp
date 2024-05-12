#ifndef TOKEN_H
#define TOKEN_H

#include <memory>
#include <optional>
#include <ostream>
#include <string>

enum class TokenType {
    LEFT_PAREN,//左括号(
    RIGHT_PAREN,//右括号)
    QUOTE,//单引号
    QUASIQUOTE,
    UNQUOTE,
    DOT,//点.
    BOOLEAN_LITERAL,//布尔
    NUMERIC_LITERAL,//数字
    STRING_LITERAL,//字符串
    IDENTIFIER,//标识符(变量名)
};//使用enum来定义做type的常数

class Token;
using TokenPtr = std::unique_ptr<Token>;

class Token {
private:
    TokenType type;

protected:
    Token(TokenType type) : type{type} {}

public:
    virtual ~Token() = default;

    static TokenPtr fromChar(char c);
    static TokenPtr dot();

    TokenType getType() const {
        return type;
    }
    virtual std::string toString() const;
};

class BooleanLiteralToken : public Token {
private:
    bool value;

public:
    BooleanLiteralToken(bool value) : Token(TokenType::BOOLEAN_LITERAL), value{value} {}

    static std::unique_ptr<BooleanLiteralToken> fromChar(char c);

    bool getValue() const {
        return value;
    }
    std::string toString() const override;
};

class NumericLiteralToken : public Token {
private:
    double value;

public:
    NumericLiteralToken(double value) : Token(TokenType::NUMERIC_LITERAL), value{value} {}

    double getValue() const {
        return value;
    }
    std::string toString() const override;
};

class StringLiteralToken : public Token {
private:
    std::string value;

public:
    StringLiteralToken(const std::string& value) : Token(TokenType::STRING_LITERAL), value{value} {}

    const std::string& getValue() const {
        return value;
    }
    std::string toString() const override;
};

class IdentifierToken : public Token {
private:
    std::string name;

public:
    IdentifierToken(const std::string& name) : Token(TokenType::IDENTIFIER), name{name} {}

    const std::string& getName() const {
        return name;
    }
    std::string toString() const override;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif
