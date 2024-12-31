#ifndef BUILTINS_H
#define BUILTINS_H
#include <unordered_map>
#include <vector>

#include "value.h"

extern std::unordered_map<
    std::string, ValuePtr (*)(const std::vector<ValuePtr>&, EvalEnv& env)>
    builtins;

ValuePtr null_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr number_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr pair_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr atom_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr boolean_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr integer_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr list_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr procedure_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr string_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr symbol_q(const std::vector<ValuePtr>& params, EvalEnv& env);
//
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr b_map(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr b_filter(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr b_reduce(const std::vector<ValuePtr>& args, EvalEnv& env);
//核心
ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr exit_b(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env);
//
ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr subtract(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr abs_f(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr exp(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv& env);
//
ValuePtr eq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr b_not(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr equal_sym(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr zero_q(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr equal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr less_equal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr greater_equal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr even(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr odd(const std::vector<ValuePtr>& params, EvalEnv& env);
#endif