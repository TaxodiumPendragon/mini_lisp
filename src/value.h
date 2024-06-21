#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <memory>
#include <string>

class Value {
public:
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
    bool isNil();
    bool isSelfEvaluating();
};

using ValuePtr =
    std::shared_ptr<Value>;  // 把这个添加到 value.h，可以减少许多重复的代码。

class BooleanValue : public Value {
    bool value;

public:
    BooleanValue(bool value) : value(value) {}
    std::string toString() const override;
    ~BooleanValue() override = default;
};

class NumericValue : public Value {
    double value;

public:
    NumericValue(double value) : value(value) {}
    std::string toString() const override;
    ~NumericValue() override = default;
};

class StringValue : public Value {
    std::string value;

public:
    StringValue(const std::string& value) : value(value) {}
    std::string toString() const override;
    ~StringValue() override = default;
};

class NilValue : public Value {
public:
    std::string toString() const override;
    ~NilValue() override = default;
};

class SymbolValue : public Value {
private:
    std::string value;

public:
    SymbolValue(const std::string& name) : value(name) {}
    std::string toString() const override;
    ~SymbolValue() override = default;
};

class PairValue : public Value {
    std::shared_ptr<Value> left;
    std::shared_ptr<Value> right;

public:
    PairValue(std::shared_ptr<Value> left, std::shared_ptr<Value> right)
        : left(left), right(right) {}
    ~PairValue() override = default;
    std::shared_ptr<Value> getLeft() const {
        return left;
    }
    std::shared_ptr<Value> getRight() const {
        return right;
    }
    std::string toStringPure() const;
    std::string toString() const override;
};

#endif