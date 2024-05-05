#include "value.h"
#include <sstream>
#include <iomanip>

std::string BooleanValue::toString() const {
    return value ? "#t" : "#f";
}

std::string NumericValue::toString() const {
    std::ostringstream oss;
    if (value == static_cast<int>(value)) {
        oss << static_cast<int>(value);
    } else {
        oss << value;
    }
    return oss.str();
}

std::string StringValue::toString() const {
    std::ostringstream oss;
    oss << std::quoted(value);
    return oss.str();
}

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return value;
}

std::string PairValue::toString() const {
    std::ostringstream oss;
    oss << "(";
    oss << left->toString();
    if (typeid(*right) == typeid(PairValue)) {
        auto& pair = static_cast<const PairValue&>(*right);
        oss << " " << pair.getLeft()->toString();
        if (typeid(*pair.getRight()) != typeid(NilValue)) {
            oss << " . " << pair.getRight()->toString();
        }
    } else if (typeid(*right) != typeid(NilValue)) {
        oss << " . " << right->toString();
    }
    oss << ")";
    return oss.str();
}