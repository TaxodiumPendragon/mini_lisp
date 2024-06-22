#include "forms.h"

#include <unordered_map>

#include "error.h"
#include "eval_env.h"
#include "value.h"

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", &defineForm}, {"quote", &quoteForm}, {"if", &ifForm},
    {"and", &andForm},       {"or", &orForm},       {"lambda", &lambdaForm}};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args[0]->isPair()) {
        if (args.size() < 2) {
            throw LispError("define requires at least 2 arguments.");
        }

        auto head = args[0]->toVector();
        if (head.empty() || !head[0]->asSymbol()) {
            throw LispError("Function name must be a symbol.");
        }

        auto funcName = head[0]->asSymbol();
        std::vector<std::string> lambdaArgs;
        for (auto it = head.begin() + 1; it != head.end(); ++it) {
            if (auto symbol = (*it)->asSymbol()) {
                lambdaArgs.push_back(*symbol);
            } else {
                throw LispError("Lambda argument names must be symbols.");
            }
        }

        std::vector<ValuePtr> lambdaBody(args.begin() + 1, args.end());
        ValuePtr lambdaValue =
            std::make_shared<LambdaValue>(lambdaArgs, lambdaBody);
        env.symbolTable[*funcName] = lambdaValue;

        // return lambdaValue;
        return std::make_shared<NilValue>();  // 定义操作成功后返回Nil
    } else {
        if (auto name = args[0]->asSymbol()) {
            ValuePtr value = env.eval(args[1]);
            env.symbolTable[*name] = value;
            return std::make_shared<NilValue>();  // 定义操作成功后返回Nil
        } else {
            throw LispError("Unimplemented");
        }
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("quote form requires exactly 1 argument");
    }
    return args[0];
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 3) {
        throw LispError("if form requires exactly 3 arguments");
    }
    ValuePtr result = env.eval(args[0]);
    if (result->toString().compare("#f") == 0) {
        return env.eval(args[2]);
    } else {
        return env.eval(args[1]);  // 真分支
    }
}

ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.empty()) return std::make_shared<BooleanValue>(true);
    for (auto& arg : args) {
        ValuePtr result = env.eval(arg);
        if (result->toString().compare("#f") == 0) {
            return std::make_shared<BooleanValue>(false);
        }
    }
    return env.eval(args.back());  // 若全部为真且参数不为空，则返回最后一个表达式的值
}

ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    for (auto& arg : args) {
        ValuePtr result = env.eval(arg);
        if (result->toString().compare("#f") != 0) {
            return result;
        }
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) {
        throw LispError("lambda form requires exactly 2 arguments");
    }
    // 第一个参数应该是参数列表
    auto paramsVec = args[0]->toVector();
    std::vector<std::string> params;
    for (const auto& param : paramsVec) {
        if (auto symbol = param->asSymbol()) {
            params.push_back(*symbol);
        } else {
            throw LispError("Lambda parameters must be symbols.");
        }
    }

    // 第二个参数是函数体
    std::vector<ValuePtr> body = {args[1]};

    return std::make_shared<LambdaValue>(params, body);
}
