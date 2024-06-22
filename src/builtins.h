#ifndef BUILTINS_H
#define BUILTINS_H
#include <unordered_map>
#include <vector>

#include "value.h"

extern std::unordered_map<std::string,
                          ValuePtr (*)(const std::vector<ValuePtr>&)>
    builtins;

ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);
ValuePtr display(const std::vector<ValuePtr>& params);
ValuePtr exit_b(const std::vector<ValuePtr>& params);
ValuePtr newline(const std::vector<ValuePtr>& params);
ValuePtr null_q(const std::vector<ValuePtr>& params);
ValuePtr number_q(const std::vector<ValuePtr>& params);
ValuePtr pair_q(const std::vector<ValuePtr>& params);
ValuePtr car(const std::vector<ValuePtr>& params);
ValuePtr cdr(const std::vector<ValuePtr>& params);
ValuePtr cons(const std::vector<ValuePtr>& params);
ValuePtr length(const std::vector<ValuePtr>& params);
ValuePtr list(const std::vector<ValuePtr>& params);
ValuePtr subtract(const std::vector<ValuePtr>& params);
ValuePtr multiply(const std::vector<ValuePtr>& params);
ValuePtr divide(const std::vector<ValuePtr>& params);
ValuePtr abs_f(const std::vector<ValuePtr>& params);
ValuePtr exp(const std::vector<ValuePtr>& params);
ValuePtr quotient(const std::vector<ValuePtr>& params);
ValuePtr remainder(const std::vector<ValuePtr>& params);
ValuePtr zero_q(const std::vector<ValuePtr>& params);
ValuePtr equal(const std::vector<ValuePtr>& params);
ValuePtr less(const std::vector<ValuePtr>& params);
ValuePtr greater(const std::vector<ValuePtr>& params);
ValuePtr less_equal(const std::vector<ValuePtr>& params);
ValuePtr greater_equal(const std::vector<ValuePtr>& params);
#endif