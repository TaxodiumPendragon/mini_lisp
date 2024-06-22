#include "eval_env.h"

#include <unordered_map>

#include ".\error.h"
#include ".\value.h"

ValuePtr EvalEnv::eval(ValuePtr expr) {
    using namespace std::literals;
    if (expr->isSelfEvaluating()) {
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
            throw LispError("Unimplemented");
        }
    }
}