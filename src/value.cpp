#include "value.h"

#include <iomanip>
#include <sstream>

bool Value::isNil() {
    return dynamic_cast<NilValue*>(this) != nullptr;
}

bool Value::isSelfEvaluating() {
    return dynamic_cast<BooleanValue*>(this) != nullptr ||
           dynamic_cast<NumericValue*>(this) != nullptr ||
           dynamic_cast<StringValue*>(this) != nullptr;
}
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

std::string PairValue::toStringPure() const {
    std::ostringstream oss;
    oss << left->toString();
    if (typeid(*right) == typeid(PairValue)) {
        auto& pair = static_cast<const PairValue&>(*right);
        if (typeid(*pair.getRight()) == typeid(NilValue)) {
            oss << " " << pair.getLeft()->toString();
        } else {
            oss << " " << pair.toStringPure();
        }
    } else if (typeid(*right) != typeid(NilValue)) {
        oss << " . " << right->toString();
    }
    return oss.str();
}

std::string PairValue::toString() const {
    return "(" + toStringPure() + ")";
}