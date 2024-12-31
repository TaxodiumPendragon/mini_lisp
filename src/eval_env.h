#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Value;
using ValuePtr = std::shared_ptr<Value>;
class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
    std::shared_ptr<EvalEnv> parent;
    EvalEnv();

public:
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params,
                                         const std::vector<ValuePtr>& args);
    std::unordered_map<std::string, ValuePtr> symbolTable;
    // EvalEnv();
    EvalEnv(std::shared_ptr<EvalEnv> parent);
    static std::shared_ptr<EvalEnv> createGlobal() {
        return std::shared_ptr<EvalEnv>(new EvalEnv());
    }
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    ValuePtr lookupBinding(const std::string& name);
};

#endif