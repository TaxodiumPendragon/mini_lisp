#include "eval_env.h"

#include <algorithm>
#include <iterator>
#include <unordered_map>

#include ".\builtins.h"
#include ".\error.h"
#include ".\value.h"

EvalEnv::EvalEnv() {
    for (const auto& builtin : builtins) {
        symbolTable[builtin.first] =
            std::make_shared<BuiltinProcValue>(builtin.second);
    }
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(), std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (auto builtinProc = std::dynamic_pointer_cast<BuiltinProcValue>(proc)) {
        // 调用内置过程
        return builtinProc->getFunc()(args);
    } else {
        throw LispError("Unimplemented");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    using namespace std::literals;
    if (expr->isSelfEvaluating() || expr->isProcedure()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (auto name = expr->asSymbol()) {
        auto it = symbolTable.find(*name);
        if (it != symbolTable.end()) {
            return it->second;
        } else {
            throw LispError("Variable " + *name + " not defined.");
        }
    } else {
        std::vector<ValuePtr> v = expr->toVector();
        if (v[0]->asSymbol() == "define"s) {
            if (auto name = v[1]->asSymbol()) {
                symbolTable[*name] = eval(v[2]);
                return std::make_shared<NilValue>();
            } else {
                throw LispError("Malformed define.");
            }
        } else {
            // 处理非特殊形式的列表表达式
            // TODO 暂时不太清楚怎么判断不是特殊形式
            ValuePtr proc = eval(v[0]);
            std::vector<ValuePtr> args;
            std::transform(v.begin() + 1, v.end(), std::back_inserter(args),
                           [this](ValuePtr arg) { return this->eval(arg); });
            return apply(proc, args);
        }
    }
}