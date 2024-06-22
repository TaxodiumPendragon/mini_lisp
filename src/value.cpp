#include "value.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

std::vector<std::shared_ptr<Value>> Value::toVector() {
    std::vector<ValuePtr> result;
    auto pairValue = dynamic_cast<PairValue*>(this);
    if (pairValue == nullptr) {
        throw std::runtime_error(
            "RuntimeError: Value who want to be list is not a PairValue");
    }
    auto left = pairValue->getLeft();
    auto right = pairValue->getRight();
    if (left != nullptr) {
        result.push_back(left);
    }
    if (right != nullptr && (typeid(*right) != typeid(NilValue))) {
        auto rightVector = right->toVector();
        result.insert(result.end(), rightVector.begin(), rightVector.end());
    }
    return result;
}

std::optional<std::string> Value::asSymbol() {
    auto symbolValue = dynamic_cast<SymbolValue*>(this);
    if (symbolValue == nullptr) {
        return std::nullopt;
    }
    return symbolValue->toString();
}

bool Value::isNil() {
    return dynamic_cast<NilValue*>(this) != nullptr;
}

bool Value::isSelfEvaluating() {
    return dynamic_cast<BooleanValue*>(this) != nullptr ||
           dynamic_cast<NumericValue*>(this) != nullptr ||
           dynamic_cast<StringValue*>(this) != nullptr;
}

bool Value::isNumber() {
    return dynamic_cast<NumericValue*>(this) != nullptr;
}

bool Value::isProcedure() const {
    return false;
}

bool Value::isPair() {
    return dynamic_cast<PairValue*>(this) != nullptr;
}

double Value::asNumber() {
    auto numericValue = dynamic_cast<NumericValue*>(this);
    if (numericValue == nullptr) {
        throw std::runtime_error("RuntimeError: Value is not a NumericValue");
    }
    return numericValue->getValue();
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

double NumericValue::getValue() const {
    return value;
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

ValuePtr ListValue::operator[](size_t index) const {
    if (index >= values.size() || index < 0) {
        throw std::out_of_range("The Index of ListValue out of range");
    }
    return values[index];
}

std::string ListValue::toString() const {
    std::string result = "(";
    for (const auto& value : values) {
        result += value->toString() + " ";
    }
    result += ")";
    return result;
}

void ListValue::append(ValuePtr value) {
    values.push_back(value);
}

std::string BuiltinProcValue::toString() const {
    return "#procedure";
}

bool BuiltinProcValue::isProcedure() const {
    return true;
}

BuiltinFuncType* BuiltinProcValue::getFunc() const {
    return func;
}

std::string LambdaValue::toString() const {
    return "#procedure";
}