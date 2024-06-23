#include "forms.h"

#include <functional>
#include <unordered_map>

#include "error.h"
#include "eval_env.h"
#include "value.h"

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", &defineForm},
    {"quote", &quoteForm},
    {"if", &ifForm},
    {"and", &andForm},
    {"or", &orForm},
    {"lambda", &lambdaForm},
    {"quasiquote", &quasiquoteForm},
    {"unquote", &unquoteForm},
    {"cond", &condForm},
    {"begin", &beginForm},
    {"let", &letForm}};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args[0]->isPair()) {
        if (args.size() < 2) {
            throw LispError("define requires at least 2 arguments.");
        }

        auto head = args[0]->toVector();
        if (head.empty() || !head[0]->asSymbol()) {
            throw LispError(
                "Function who want to be defined,its name must be a symbol.");
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
            std::make_shared<LambdaValue>(lambdaArgs, lambdaBody,env.shared_from_this());
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

ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    // 递归处理args[0]，如果遇到unquote，就对其内部进行求值
    std::function<ValuePtr(ValuePtr)> process = [&](ValuePtr arg) -> ValuePtr {
        if (arg->isPair()) {  // 如果是列表，需要检查是否包含unquote
            auto vec = arg->toVector();
            std::vector<ValuePtr> resultVec;
            for (auto& item : vec) {
                if (item->isPair()) {  // 如果元素是列表，检查是否是unquote
                    auto itemVec = item->toVector();
                    if (!itemVec.empty() && itemVec[0]->isSymbol() &&
                        *itemVec[0]->asSymbol() == "unquote") {
                        if (itemVec.size() != 2) {
                            throw LispError(
                                "unquote requires exactly 1 argument");
                        }
                        // 对unquote内部进行求值
                        resultVec.push_back(env.eval(itemVec[1]));
                    } else {
                        // 如果不是unquote，递归处理
                        resultVec.push_back(process(item));
                    }
                } else {
                    // 如果元素不是列表，直接添加
                    resultVec.push_back(item);
                }
            }
            // 将处理后的列表重新构建为PairValue
            return make_shared<ListValue>(
                resultVec);  // 使用正确的构造函数或工厂方法
        } else {
            // 如果不是列表，直接返回
            return arg;
        }
    };
    return process(args[0]);
}

ValuePtr unquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("unquote form requires exactly 1 argument");
    }
    // 直接对参数求值
    return env.eval(args[0]);
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
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
    return env.eval(
        args.back());  // 若全部为真且参数不为空，则返回最后一个表达式的值
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
    if (args.size() < 2) {
        throw LispError("lambda form requires at least 2 arguments");
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
    std::vector<ValuePtr> body(args.begin() + 1, args.end());

    return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
}

ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    for (const auto& clause : args) {
        auto clauseVec = clause->toVector();
        if (clauseVec.empty()) throw LispError("Empty cond clause.");

        auto condition = clauseVec.front();
        // 检查是否是else分支
        bool isElse = condition->isSymbol() && condition->toString() == "else";
        if (isElse || env.eval(condition)->toString() != "#f") {
            // 如果是else分支或条件求值为真
            if (clauseVec.size() == 1) {
                // 如果只有条件，没有表达式，则返回条件的求值结果
                return env.eval(condition);
            } else {
                ValuePtr lastEvalResult = std::make_shared<NilValue>();
                for (size_t i = 1; i < clauseVec.size(); ++i) {
                    lastEvalResult = env.eval(clauseVec[i]);
                }
                return lastEvalResult;  // 返回最后一个表达式的求值结果
            }
        }
    }
    return std::make_shared<NilValue>();  // 如果所有条件都不满足，返回Nil
}

ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    ValuePtr lastEvalResult;
    for (const auto& expr : args) {
        lastEvalResult = env.eval(expr);
    }
    return lastEvalResult;  // 返回最后一个表达式的求值结果
}

ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("let form requires at least 2 arguments");
    }
    auto bindingsVec = args[0]->toVector();
    std::vector<std::string> paramNames;
    std::vector<ValuePtr> values;
    for (const auto& binding : bindingsVec) {
        auto bindingVec = binding->toVector();
        if (bindingVec.size() != 2) {
            throw LispError("Invalid binding in let");
        }
        auto paramName = bindingVec[0]->asSymbol();
        if (!paramName) {
            throw LispError("Binding name must be a symbol");
        }
        paramNames.push_back(*paramName);
        values.push_back(env.eval(bindingVec[1]));
    }
    EvalEnv letEnv(
        env.shared_from_this());  // 创建一个新的环境，父环境是当前环境
    for (size_t i = 0; i < paramNames.size(); ++i) {
        letEnv.symbolTable[paramNames[i]] = values[i];
    }
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    ValuePtr lastEvalResult;
    for (const auto& expr : body) {
        lastEvalResult = letEnv.eval(expr);
    }
    return lastEvalResult;  // 返回最后一个表达式的求值结果
}
