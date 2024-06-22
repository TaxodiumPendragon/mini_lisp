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
#endif