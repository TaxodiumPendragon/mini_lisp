#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <string>

class Value {
public:
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
};

class BooleanValue : public Value {
    bool value;

public:
    BooleanValue(bool value) : value(value) {}
    virtual std::string toString() const override;
};

class NumericValue : public Value {
    double value;

public:
    NumericValue(double value) : value(value) {}
    virtual std::string toString() const override;
};

class StringValue : public Value {
    std::string value;

public:
    StringValue(const std::string& value) : value(value) {}
    virtual std::string toString() const override;
};

class NilValue : public Value {
public:
    virtual std::string toString() const override;
};

class SymbolValue : public Value {
private:
    std::string value;

public:
    SymbolValue(const std::string& name) : value(name) {}
    virtual std::string toString() const override;
};

class PairValue : public Value {
    std::shared_ptr<Value> left;
    std::shared_ptr<Value> right;

public:
    PairValue(std::shared_ptr<Value> left, std::shared_ptr<Value> right)
        : left(left), right(right) {}
        std::shared_ptr<Value> getLeft() const { return left; }
    std::shared_ptr<Value> getRight() const { return right; }
    virtual std::string toString() const override;
};

#endif