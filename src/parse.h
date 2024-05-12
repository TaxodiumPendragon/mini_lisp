#ifndef PARSE_H  // 保护代码
#define PARSE_H

#include <deque>
#include <iostream>

#include "token.h"
#include "value.h"

class Parser {
    std::deque<TokenPtr> parseToken;

public:
    Parser(std::deque<TokenPtr> tokens);
    ValuePtr parse();
};

#endif