#include "builtins.h"

#include <cmath>

#include "error.h"
#include "value.h"

std::unordered_map<std::string, ValuePtr (*)(const std::vector<ValuePtr>&)>
    builtins = {
        {"+", add},
        {"print", print},
        {"display", display},
        {"exit", exit_b},
        {"newline", newline},
        {"null?", null_q},
        {"number?", number_q},
        {"pair?", pair_q},
        {"car", car},
        {"cdr", cdr},
        {"cons", cons},
        {"length", length},
        {"list", list},
        {"-", subtract},
        {"*", multiply},
        {"/", divide},
        {"abs", abs_f},
        {"expt", exp},
        {"quotient", quotient},
        {"remainder", remainder},
        {"zero?", zero}
        // 添加其他内置过程
};

// 类型检查库
ValuePtr null_q(const std::vector<ValuePtr>& params) {
    if (params.empty()) throw LispError("null? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isNil());
}

ValuePtr number_q(const std::vector<ValuePtr>& params) {
    if (params.empty()) throw LispError("number? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isNumber());
}

ValuePtr add(const std::vector<ValuePtr>& params) {
    int result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr pair_q(const std::vector<ValuePtr>& params) {
    if (params.empty()) throw LispError("pair? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isPair());
}

// 对子与列表操作库

ValuePtr car(const std::vector<ValuePtr>& params) {
    if (params.empty() || !params.front()->isPair()) {
        throw LispError("car expects a non-empty list.");
    }
    auto pair = std::dynamic_pointer_cast<PairValue>(params.front());
    return pair->getLeft();
}

ValuePtr cdr(const std::vector<ValuePtr>& params) {
    if (params.empty() || !params.front()->isPair()) {
        throw LispError("cdr expects a non-empty list.");
    }
    auto pair = std::dynamic_pointer_cast<PairValue>(params.front());
    return pair->getRight();
}

ValuePtr cons(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("cons expects 2 arguments.");
    }
    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr length(const std::vector<ValuePtr>& params) {
    if (params.empty() || !params.front()->isPair()) {
        throw LispError("length expects a list.");
    }
    int count = 0;
    auto current = params.front();
    while (current->isPair()) {
        auto pair = std::dynamic_pointer_cast<PairValue>(current);
        current = pair->getRight();
        ++count;
    }
    return std::make_shared<NumericValue>(count);
}

ValuePtr list(const std::vector<ValuePtr>& params) {
    ValuePtr result = std::make_shared<NilValue>();
    for (auto it = params.rbegin(); it != params.rend(); ++it) {
        result = std::make_shared<PairValue>(*it, result);
    }
    return result;
}

// 核心库
ValuePtr print(const std::vector<ValuePtr>& params) {
    for (const auto& param : params) {
        std::cout << param->toString() << " ";
    }
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr display(const std::vector<ValuePtr>& params) {
    for (const auto& param : params) {
        std::cout << param->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr exit_b(const std::vector<ValuePtr>& params) {
    int e = 0;
    if (!params.empty()) {
        e = std::stoi(params.front()->toString());
    }
    std::exit(e);
}

ValuePtr newline(const std::vector<ValuePtr>& params) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

// 算术运算库
ValuePtr subtract(const std::vector<ValuePtr>& params) {
    if (params.empty()) {
        throw LispError("subtract expects at least 1 argument.");
    }
    int result = params.front()->asNumber();
    if (params.size() == 1) {  // 负号操作
        return std::make_shared<NumericValue>(-result);
    }
    for (auto it = params.begin() + 1; it != params.end(); ++it) {
        if (!(*it)->isNumber()) {
            throw LispError("All arguments must be numbers.");
        }
        result -= (*it)->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr multiply(const std::vector<ValuePtr>& params) {
    int result = 1;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot multiply non-numeric values.");
        }
        result *= i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr divide(const std::vector<ValuePtr>& params) {
    if (params.size() < 2) {
        throw LispError("divide expects at least 2 arguments.");
    }
    int result = params.front()->asNumber();
    for (auto it = params.begin() + 1; it != params.end(); ++it) {
        int d = (*it)->asNumber();
        if (d == 0) {
            throw LispError("Division by zero.");
        }
        result /= d;
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr abs_f(const std::vector<ValuePtr>& params) {
    if (params.empty() || !params.front()->isNumber()) {
        throw LispError("abs expects a numeric argument.");
    }
    return std::make_shared<NumericValue>(std::abs(params.front()->asNumber()));
}

ValuePtr exp(const std::vector<ValuePtr>& params) {
    if (params.size() != 2 || !params[0]->isNumber() ||
        !params[1]->isNumber()) {
        throw LispError("expt expects two numeric arguments.");
    }
    int base = params[0]->asNumber();
    int ex = params[1]->asNumber();
    return std::make_shared<NumericValue>(std::pow(base, ex));
}

ValuePtr quotient(const std::vector<ValuePtr>& params) {
    if (params.size() != 2 || !params[0]->isNumber() ||
        !params[1]->isNumber()) {
        throw LispError("quotient expects two numeric arguments.");
    }
    int dividend = params[0]->asNumber();
    int d = params[1]->asNumber();
    if (d == 0) {
        throw LispError("Division by zero in quotient.");
    }
    return std::make_shared<NumericValue>(dividend / d);
}

ValuePtr remainder(const std::vector<ValuePtr>& params) {
    if (params.size() != 2 || !params[0]->isNumber() ||
        !params[1]->isNumber()) {
        throw LispError("remainder expects two numeric arguments.");
    }
    int dividend = params[0]->asNumber();
    int d = params[1]->asNumber();
    if (d == 0) {
        throw LispError("Division by zero in remainder.");
    }
    return std::make_shared<NumericValue>(dividend % d);
}

// 比较库
ValuePtr zero(const std::vector<ValuePtr>& params) {
    if (params.empty() || !params.front()->isNumber()) {
        throw LispError("zero? expects a numeric argument.");
    }
    return std::make_shared<BooleanValue>(params.front()->asNumber() == 0);
}