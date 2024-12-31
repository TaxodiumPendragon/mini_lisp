#include "eval_env.h"

#include <algorithm>
#include <iterator>
#include <unordered_map>

#include "value.h"
#include "builtins.h"
#include "error.h"
#include "forms.h"
#include "value.h"

EvalEnv::EvalEnv() {
    for (const auto& builtin : builtins) {
        symbolTable[builtin.first] =
            std::make_shared<BuiltinProcValue>(builtin.second);
    }
}

EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent) : parent(parent) {
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
        return builtinProc->getFunc()(args, *this);
    } else if (auto lambdaProc = std::dynamic_pointer_cast<LambdaValue>(proc)) {
        return lambdaProc->apply(args);  // 调用 Lambda 过程
    } else {
        throw LispError("Unimplemented");
    }
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(
    const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    auto childEnv = std::make_shared<EvalEnv>(
        shared_from_this());  // 使用当前环境作为父环境
    if (params.size() != args.size()) {
        throw LispError("Parameter and argument counts do not match.");
    }
    for (size_t i = 0; i < params.size(); ++i) {
        childEnv->symbolTable[params[i]] = args[i];  // 绑定参数名到实参值
    }
    return childEnv;
}

ValuePtr EvalEnv::lookupBinding(const std::string& name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second;
    } else if (parent != nullptr) {
        return parent->lookupBinding(name);  // 向上追溯
    } else {
        throw LispError("Variable " + name + " not defined.");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating() || expr->isProcedure()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (auto name = expr->asSymbol()) {
        return lookupBinding(*name);
    } else {
        std::vector<ValuePtr> v = expr->toVector();
        auto n = v[0]->asSymbol();
        auto it = SPECIAL_FORMS.find(*n);
        if (it != SPECIAL_FORMS.end()) {
            std::vector<ValuePtr> args(v.begin() + 1, v.end());
            return it->second(args, *this);
        } else {
            // 处理非特殊形式的列表表达式
            ValuePtr proc = eval(v[0]);
            std::vector<ValuePtr> args;
            std::transform(v.begin() + 1, v.end(), std::back_inserter(args),
                           [this](ValuePtr arg) { return this->eval(arg); });
            return apply(proc, args);
        }
    }
}