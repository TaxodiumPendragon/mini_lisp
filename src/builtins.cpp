#include "builtins.h"

#include <cmath>

#include "error.h"
#include "eval_env.h"
#include "value.h"

std::unordered_map<std::string,
                   ValuePtr (*)(const std::vector<ValuePtr>&, EvalEnv& env)>
    builtins = {
        {"+", add},
        {"print", print},
        {"display", display},
        {"exit", exit_b},
        {"newline", newline},
        {"apply", apply},
        {"displayln", displayln},
        {"error", error},
        {"eval", eval},
        //
        {"null?", null_q},
        {"number?", number_q},
        {"pair?", pair_q},
        {"car", car},
        {"cdr", cdr},
        {"cons", cons},
        {"length", length},
        {"list", list},
        {"append", append},
        {"map", b_map},
        {"filter", b_filter},
        {"reduce", b_reduce},
        {"-", subtract},
        {"*", multiply},
        {"/", divide},
        {"abs", abs_f},
        {"expt", exp},
        {"quotient", quotient},
        {"modulo", modulo},
        {"remainder", remainder},
        {"zero?", zero_q},
        {"atom?", atom_q},
        {"boolean?", boolean_q},
        {"integer?", integer_q},
        {"list?", list_q},
        {"procedure?", procedure_q},
        {"string?", string_q},
        {"symbol?", symbol_q},  //
        {"eq?", eq},
        {"equal?", equal},
        {"<", less},
        {">", greater},
        {"<=", less_equal},
        {">=", greater_equal},
        {"even?", even},
        {"odd?", odd},
        {"not", b_not},
        {"=", equal_sym},
        // 添加其他内置过程
};

// 类型检查库

ValuePtr null_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("null? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isNil());
}

ValuePtr number_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("number? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isNumber());
}

ValuePtr pair_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("pair? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isPair());
}

ValuePtr atom_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("atom? expects 1 argument.");
    auto& param = params.front();
    return std::make_shared<BooleanValue>(
        param->isBoolean() || param->isNumber() || param->isString() ||
        param->isSymbol() || param->isNil());
}

ValuePtr boolean_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("boolean? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isBoolean());
}

ValuePtr integer_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("integer? expects 1 argument.");
    auto& param = params.front();
    return std::make_shared<BooleanValue>(param->isNumber() &&
                                          param->asNumber() ==
                                              std::floor(param->asNumber()));
}

ValuePtr list_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("list? expects 1 argument.");
    auto& param = params.front();
    if (param->isNil()) return std::make_shared<BooleanValue>(true);
    if (!param->isPair()) return std::make_shared<BooleanValue>(false);
    auto current = param;
    while (current->isPair()) {
        auto pair = std::dynamic_pointer_cast<PairValue>(current);
        current = pair->getRight();
    }
    return std::make_shared<BooleanValue>(current->isNil());
}

ValuePtr procedure_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("procedure? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isProcedure());
}

ValuePtr string_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("string? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isString());
}

ValuePtr symbol_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("symbol? expects 1 argument.");
    return std::make_shared<BooleanValue>(params.front()->isSymbol());
}

// 对子与列表操作库

ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty() || !params.front()->isPair()) {
        throw LispError("car expects a non-empty list.");
    }
    auto pair = std::dynamic_pointer_cast<PairValue>(params.front());
    return pair->getLeft();
}

ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty() || !params.front()->isPair()) {
        throw LispError("cdr expects a non-empty list.");
    }
    auto pair = std::dynamic_pointer_cast<PairValue>(params.front());
    return pair->getRight();
}

ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("cons expects 2 arguments.");
    }
    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.front()->isNil() && params.size() == 1) {
        return std::make_shared<NumericValue>(0);
    }
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

ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    ValuePtr result = std::make_shared<NilValue>();
    for (auto it = params.rbegin(); it != params.rend(); ++it) {
        result = std::make_shared<PairValue>(*it, result);
    }
    return result;
}

ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::vector<ValuePtr> result;  // 初始化一个空向量用于存储结果
    for (const auto& param : params) {
        if (param->isNil())
            continue;
        else if (!param->isPair()) {
            // 如果参数不是PairValue，直接添加到结果向量中
            result.push_back(param);
        } else {
            // 如果参数是PairValue，将其转换为向量并合并到结果向量中
            auto vectorFromPair = param->toVector();
            result.insert(result.end(), vectorFromPair.begin(),
                          vectorFromPair.end());
        }
    }
    // 将结果向量转换为列表Value并返回
    if (result.empty()) {
        return std::make_shared<NilValue>();  // 如果结果为空，返回NilValue
    }
    return list(result, env);  // 返回构建的ListValue对象f
}

ValuePtr b_map(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) throw LispError("map expects 2 arguments.");
    auto func = args[0];
    auto list = args[1]->toVector();
    std::vector<ValuePtr> result;
    for (auto& elem : list) {
        std::vector<ValuePtr> funcArgs = {elem};
        result.push_back(env.apply(func, funcArgs));
    }
    return std::make_shared<ListValue>(result);
}

ValuePtr b_filter(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) throw LispError("filter expects 2 arguments.");
    auto pred = args[0];
    auto list = args[1]->toVector();
    std::vector<ValuePtr> result;
    for (auto& elem : list) {
        std::vector<ValuePtr> predArgs = {elem};
        if (env.apply(pred, predArgs)->asBoolean()) {
            result.push_back(elem);
        }
    }
    return std::make_shared<ListValue>(result);
}

ValuePtr b_reduce(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) throw LispError("reduce expects 2 arguments.");
    auto func = args[0];
    auto list = args[1]->toVector();
    ValuePtr result = list[0];
    list.erase(list.begin());
    for (auto& elem : list) {
        std::vector<ValuePtr> funcArgs = {result, elem};
        result = env.apply(func, funcArgs);
    }
    return result;
}

// 核心库

ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) throw LispError("apply expects 2 arguments.");
    auto& proc = params[0];
    auto list = params[1]->toVector();
    return env.apply(proc, list);
}

ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("display expects 1 argument.");
    if (params.front()->isString()) {
        auto str = params.front()->toString();
        std::cout << str.substr(1, str.length() - 2);  // 去掉字符串首尾的引号
    } else
        std::cout << "'" + params.front()->toString();
    return std::make_shared<NilValue>();
}

ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    display(params, env);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("error expects 1 argument.");
    throw LispError(params.front()->toString());
}

ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("eval expects 1 argument.");
    return env.eval(params.front());
}

ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (const auto& param : params) {
        std::cout << param->toString() << " ";
    }
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr exit_b(const std::vector<ValuePtr>& params, EvalEnv& env) {
    int e = 0;
    if (!params.empty()) {
        e = std::stoi(params.front()->toString());
    }
    std::exit(e);
}

ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

// 算术运算库
ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr subtract(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        throw LispError("subtract expects at least 1 argument.");
    }
    double result = params.front()->asNumber();
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

ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 1;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot multiply non-numeric values.");
        }
        result *= i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1) {
        throw LispError("divide expects at least 1 arguments.");
    }
    if (params.size() == 1) {
        double d = params.front()->asNumber();
        if (d == 0) {
            throw LispError("Division by zero.");
        }
        return std::make_shared<NumericValue>(1 / d);
    }
    double result = params.front()->asNumber();
    for (auto it = params.begin() + 1; it != params.end(); ++it) {
        double d = (*it)->asNumber();
        if (d == 0) {
            throw LispError("Division by zero.");
        }
        result /= d;
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr abs_f(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty() || !params.front()->isNumber()) {
        throw LispError("abs expects a numeric argument.");
    }
    return std::make_shared<NumericValue>(std::abs(params.front()->asNumber()));
}

ValuePtr exp(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2 || !params[0]->isNumber() ||
        !params[1]->isNumber()) {
        throw LispError("expt expects two numeric arguments.");
    }
    double base = params[0]->asNumber();
    double ex = params[1]->asNumber();
    return std::make_shared<NumericValue>(std::pow(base, ex));
}

ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
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

ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("modulo expects 2 arguments.");
    }
    auto& x = params[0];
    auto& y = params[1];
    if (!x->isNumber() || !y->isNumber()) {
        throw LispError("modulo arguments must be numbers.");
    }
    double xVal = x->asNumber();
    double yVal = y->asNumber();
    if (yVal == 0) {
        throw LispError("b-modulo division by zero.");
    }
    double result = std::fmod(xVal, yVal);
    if ((result < 0) != (yVal < 0)) {
        result += yVal;
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv& env) {
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
ValuePtr eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("eq? expects 2 arguments.");
    }
    bool result = false;
    if (params[0]->isNumber() && params[1]->isNumber()) {
        result = params[0]->asNumber() == params[1]->asNumber();
    } else if (params[0]->isBoolean() && params[1]->isBoolean()) {
        result = params[0]->asBoolean() == params[1]->asBoolean();
    } else if (params[0]->isString() && params[1]->isString()) {
        result = params[0]->toString() == params[1]->toString();
    } else if (params[0]->isSymbol() && params[1]->isSymbol()) {
        result = params[0]->toString() == params[1]->toString();
    } else if (params[0]->isNil() && params[1]->isNil()) {
        result = true;
    } else if (params[0]->isPair() && params[1]->isPair()) {
        result = params[0] == params[1];
    } else if (params[0]->isProcedure() && params[1]->isProcedure()) {
        result = params[0] == params[1];
    }
    return std::make_shared<BooleanValue>(result);
}

ValuePtr b_not(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("not expects 1 argument.");
    auto& param = params.front();

    bool result = param->isBoolean();
    if (result) {
        result = !param->asBoolean();
    }
    return std::make_shared<BooleanValue>(result);
}

ValuePtr equal_sym(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) throw LispError("equal? expects 2 arguments.");
    return std::make_shared<BooleanValue>(params[0]->toString() ==
                                          params[1]->toString());
}

ValuePtr zero_q(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty() || !params.front()->isNumber()) {
        throw LispError("zero? expects a numeric argument.");
    }
    return std::make_shared<BooleanValue>(params.front()->asNumber() == 0);
}

ValuePtr equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("= expects 2 arguments.");
    }
    bool result = false;
    if (params[0]->isNumber() && params[1]->isNumber()) {
        result = params[0]->asNumber() == params[1]->asNumber();
    } else if (params[0]->isBoolean() && params[1]->isBoolean()) {
        result = params[0]->asBoolean() == params[1]->asBoolean();
    } else if (params[0]->isString() && params[1]->isString() ||
               params[0]->isPair() && params[1]->isPair() ||
               params[0]->isProcedure() && params[1]->isProcedure()) {
        result = params[0]->toString() == params[1]->toString();
    } else if (params[0]->isSymbol() && params[1]->isSymbol()) {
        result = params[0]->toString() == params[1]->toString();
    } else if (params[0]->isNil() && params[1]->isNil()) {
        result = true;
    }
    return std::make_shared<BooleanValue>(result);
}

ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("< expects 2 arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber() <
                                          params[1]->asNumber());
}

ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("> expects 2 arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber() >
                                          params[1]->asNumber());
}

ValuePtr less_equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("<= expects 2 arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber() <=
                                          params[1]->asNumber());
}

ValuePtr greater_equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError(">= expects 2 arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber() >=
                                          params[1]->asNumber());
}

ValuePtr even(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty() || !params.front()->isNumber() ||
        !(params.front()->isNumber() &&
          params.front()->asNumber() ==
              std::floor(params.front()->asNumber()))) {
        throw LispError("even? expects a numeric argument.");
    }
    return std::make_shared<BooleanValue>(
        (static_cast<int>(params.front()->asNumber()) % 2) == 0);
}

ValuePtr odd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty() || !params.front()->isNumber() ||
        !(params.front()->isNumber() &&
          params.front()->asNumber() ==
              std::floor(params.front()->asNumber()))) {
        throw LispError("even? expects a numeric argument.");
    }
    return std::make_shared<BooleanValue>(
        (static_cast<int>(params.front()->asNumber()) % 2) != 0);
}