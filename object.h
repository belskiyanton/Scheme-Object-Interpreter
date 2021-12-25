#pragma once

#include <cmath>
#include <memory>
#include <vector>
#include "error.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;

    virtual std::shared_ptr<Object> Eval() {
        return shared_from_this();
    }

    virtual std::string ToString() {
        return "";
    }
};

class Boolean : public Object {
public:
    Boolean(std::string s) {
        if (s == "#t") {
            val_ = true;
        } else if (s == "#f") {
            val_ = false;
        } else {
            throw NameError("Incorrect bool initialization");
        }
    }

    Boolean(bool x) : val_(x) {
    }

    bool GetValue() {
        return val_;
    }

    std::string ToString() override {
        if (val_) {
            return "#t";
        }
        return "#f";
    }

private:
    bool val_;
};

template <class T>
std::shared_ptr<T> As(std::shared_ptr<Object>& obj);

template <class T>
bool Is(const std::shared_ptr<Object>& obj);

class Number : public Object {
public:
    Number(int val) : val_(val) {
    }

    int64_t GetValue() const {
        return val_;
    }

    std::string ToString() override {
        return std::to_string(val_);
    }

private:
    int64_t val_;
};

class Function {
public:
    virtual ~Function() = default;

    virtual std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>&) {
        return std::make_shared<Object>();
    }
};

class TextMessage : public Object {
public:
    TextMessage(std::string&& s) : data_(std::move(s)) {
    }

    std::string ToString() override {
        return data_;
    }

private:
    std::string data_;
};

class Dot : public Object {
public:
    Dot() {
    }

    std::string ToString() override {
        return ".";
    }
};

class Quote : public Object, public Function {
public:
    Quote() {
    }

    std::string ToString() override {
        return "\'";
    }

    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1) {
            throw RuntimeError("RuntimeError");
        }
        return std::make_shared<TextMessage>(args[0]->ToString());
    }
};

inline void NumbersOnlyOrError(std::vector<std::shared_ptr<Object>>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (!args[i]) {
            throw RuntimeError("RuntimeError");
        }
        args[i] = args[i]->Eval();
        if (!Is<Number>(args[i])) {
            throw RuntimeError("RuntimeError");
        }
    }
}

class Plus : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        NumbersOnlyOrError(args);
        int64_t ans = 0;
        for (size_t i = 0; i < args.size(); ++i) {
            ans += As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

class Multiply : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        int64_t ans = 1;
        NumbersOnlyOrError(args);
        for (size_t i = 0; i < args.size(); ++i) {
            ans *= As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

class Minus : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() < 2) {
            throw RuntimeError("RuntimeError");
        }
        NumbersOnlyOrError(args);
        int64_t ans = As<Number>(args[0])->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            ans -= As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

class Divide : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() < 2) {
            throw RuntimeError("RuntimeError");
        }
        NumbersOnlyOrError(args);
        int64_t ans = As<Number>(args[0])->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            ans /= As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

class IsNumber : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1 || !args[0]) {
            throw RuntimeError("RuntimeError");
        }
        return std::make_shared<Boolean>(Is<Number>(args[0]->Eval()));
    }
};

class EqualNumbers : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        bool ans = true;
        NumbersOnlyOrError(args);
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            ans &= (As<Number>(args[i])->GetValue() == As<Number>(args[i + 1])->GetValue());
        }
        return std::make_shared<Boolean>(ans);
    }
};

class IncreaseNumbers : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        bool ans = true;
        NumbersOnlyOrError(args);
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            ans &= (As<Number>(args[i])->GetValue() < As<Number>(args[i + 1])->GetValue());
        }
        return std::make_shared<Boolean>(ans);
    }
};

class IncreaseOrEqualNumbers : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        bool ans = true;
        NumbersOnlyOrError(args);
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            ans &= (As<Number>(args[i])->GetValue() <= As<Number>(args[i + 1])->GetValue());
        }
        return std::make_shared<Boolean>(ans);
    }
};

class DecreaseNumbers : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        bool ans = true;
        NumbersOnlyOrError(args);
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            ans &= (As<Number>(args[i])->GetValue() > As<Number>(args[i + 1])->GetValue());
        }
        return std::make_shared<Boolean>(ans);
    }
};

class DecreaseOrEqualNumbers : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        bool ans = true;
        NumbersOnlyOrError(args);
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            ans &= (As<Number>(args[i])->GetValue() >= As<Number>(args[i + 1])->GetValue());
        }
        return std::make_shared<Boolean>(ans);
    }
};

class MaxNumber : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        int64_t ans;
        if (args.empty()) {
            throw RuntimeError("RuntimeError");
        }
        NumbersOnlyOrError(args);
        ans = As<Number>(args[0])->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            ans = std::max(ans, As<Number>(args[i])->GetValue());
        }
        return std::make_shared<Number>(ans);
    }
};

class MinNumber : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        int64_t ans;
        if (args.empty()) {
            throw RuntimeError("RuntimeError");
        }
        NumbersOnlyOrError(args);
        ans = As<Number>(args[0])->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            ans = std::min(ans, As<Number>(args[i])->GetValue());
        }
        return std::make_shared<Number>(ans);
    }
};

class Abs : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1) {
            throw RuntimeError("RuntimeError");
        }
        NumbersOnlyOrError(args);
        return std::make_shared<Number>(std::abs(As<Number>(args[0])->GetValue()));
    }
};

inline void EvalOrError(std::shared_ptr<Object>& arg) {
    if (arg) {
        arg = arg->Eval();
        return;
    }
    throw RuntimeError("RuntimeError");
}

inline void EvalVector(std::vector<std::shared_ptr<Object>>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        EvalOrError(args[i]);
    }
}

class IsBoolean : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1) {
            throw RuntimeError("RuntimeError");
        }
        EvalVector(args);
        return std::make_shared<Boolean>(Is<Boolean>(args[0]));
    }
};

class BooleanNot : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1) {
            throw RuntimeError("RuntimeError");
        }
        EvalVector(args);
        if (Is<Boolean>(args[0]) && !(As<Boolean>(args[0])->GetValue())) {
            return std::make_shared<Boolean>(true);
        }
        return std::make_shared<Boolean>(false);
    }
};

class And : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        for (size_t i = 0; i < args.size(); ++i) {
            EvalOrError(args[i]);
            if (Is<Boolean>(args[i]) && !As<Boolean>(args[i])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return args.back();
    }
};

class Or : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override {
        for (size_t i = 0; i < args.size(); ++i) {
            EvalOrError(args[i]);
            if (!Is<Boolean>(args[i]) || As<Boolean>(args[i])->GetValue()) {
                return args[i];
            }
        }
        return std::make_shared<Boolean>(false);
    }
};

class IsPair : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class IsNull : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class IsList : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class Cons : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class Car : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class Cdr : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class ToList : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class ListElem : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class ListTail : public Function {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>>& args) override;
};

class Symbol : public Object {
public:
    Symbol(std::string&& name) : name_(std::move(name)) {
    }

    std::string ToString() override {
        return name_;
    }

    const std::string& GetName() const {
        return name_;
    }

    std::shared_ptr<Object> Eval() override {
        if (name_ == "#t" || name_ == "#f") {
            return std::make_shared<Boolean>(name_);
        } else {
            return shared_from_this();
        }
    }

    std::shared_ptr<Function> GetFunction() {
        if (name_ == "+") {
            return std::make_shared<Plus>();
        } else if (name_ == "-") {
            return std::make_shared<Minus>();
        } else if (name_ == "*") {
            return std::make_shared<Multiply>();
        } else if (name_ == "/") {
            return std::make_shared<Divide>();
        } else if (name_ == "=") {
            return std::make_shared<EqualNumbers>();
        } else if (name_ == ">") {
            return std::make_shared<DecreaseNumbers>();
        } else if (name_ == "<") {
            return std::make_shared<IncreaseNumbers>();
        } else if (name_ == ">=") {
            return std::make_shared<DecreaseOrEqualNumbers>();
        } else if (name_ == "<=") {
            return std::make_shared<IncreaseOrEqualNumbers>();
        } else if (name_ == "number?") {
            return std::make_shared<IsNumber>();
        } else if (name_ == "max") {
            return std::make_shared<MaxNumber>();
        } else if (name_ == "min") {
            return std::make_shared<MinNumber>();
        } else if (name_ == "abs") {
            return std::make_shared<Abs>();
        } else if (name_ == "not") {
            return std::make_shared<BooleanNot>();
        } else if (name_ == "and") {
            return std::make_shared<And>();
        } else if (name_ == "or") {
            return std::make_shared<Or>();
        } else if (name_ == "boolean?") {
            return std::make_shared<IsBoolean>();
        } else if (name_ == "quote" || name_ == "\'") {
            return std::make_shared<Quote>();
        } else if (name_ == "pair?") {
            return std::make_shared<IsPair>();
        } else if (name_ == "null?") {
            return std::make_shared<IsNull>();
        } else if (name_ == "list?") {
            return std::make_shared<IsList>();
        } else if (name_ == "cons") {
            return std::make_shared<Cons>();
        } else if (name_ == "car") {
            return std::make_shared<Car>();
        } else if (name_ == "cdr") {
            return std::make_shared<Cdr>();
        } else if (name_ == "list") {
            return std::make_shared<ToList>();
        } else if (name_ == "list-ref") {
            return std::make_shared<ListElem>();
        } else if (name_ == "list-tail") {
            return std::make_shared<ListTail>();
        }
        throw NameError("NameError");
    }

private:
    std::string name_;
};

class Cell : public Object {
public:
    template <typename T1, typename T2>
    Cell(T1 first, T2 second) : first_(first), second_(second) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    std::vector<std::shared_ptr<Object>> OpenCell() {
        std::vector<std::shared_ptr<Object>> ans;
        auto second = this->GetSecond();
        std::shared_ptr<Cell> now;
        while (Is<Cell>(second)) {
            now = As<Cell>(second);
            ans.push_back(now->GetFirst());
            second = (now->GetSecond());
        }
        if (second) {
            ans.push_back(second);
        }
        return ans;
    }

    std::vector<std::shared_ptr<Object>> ToVector() {
        std::vector<std::shared_ptr<Object>> ans;
        if (first_) {
            ans.push_back(first_);
        }
        auto second = this->GetSecond();
        std::shared_ptr<Cell> now;
        while (Is<Cell>(second)) {
            now = As<Cell>(second);
            ans.push_back(now->GetFirst());
            second = (now->GetSecond());
        }
        if (second) {
            ans.push_back(second);
        }
        return ans;
    }

    std::shared_ptr<Object> Eval() override {
        auto first = this->GetFirst();
        if (first) {
            first = first->Eval();
        }
        if (!Is<Symbol>(first)) {
            throw RuntimeError("RuntimeError");
        }
        if (As<Symbol>(first)->GetName() == "quote") {
            return this->GetSecond();
        }
        std::vector<std::shared_ptr<Object>> temp = OpenCell();
        return (*(As<Symbol>(first)->GetFunction()))(temp);
    }

    std::string ToString() override {
        std::vector<std::shared_ptr<Object>> temp = OpenCell();
        std::string ans = "(";
        if (first_) {
            ans += first_->ToString();
        }
        for (size_t i = 0; i < temp.size(); ++i) {
            ans.push_back(' ');
            ans += temp[i]->ToString();
        }
        ans += ")";
        return ans;
    }

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

inline std::shared_ptr<Object> IsPair::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError("RuntimeError");
    }
    if (!Is<Cell>(args[0])) {
        return std::make_shared<Boolean>(false);
    }
    EvalOrError(args[0]);
    std::vector<std::shared_ptr<Object>> inside = As<Cell>(args[0])->ToVector();
    if (inside.size() > 3 || inside.size() < 2) {
        return std::make_shared<Boolean>(false);
    }
    EvalVector(inside);
    if (Is<Dot>(inside[1]) && args.size() == 3) {
        return std::make_shared<Boolean>(true);
    }
    if (args.size() == 3) {
        return std::make_shared<Boolean>(false);
    }
    return std::make_shared<Boolean>(true);
}

inline std::shared_ptr<Object> IsNull::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError("RuntimeError");
    }
    if (!args[0]) {
        return std::make_shared<Boolean>(true);
    }
    EvalOrError(args[0]);
    if (!Is<Cell>(args[0])) {
        return std::make_shared<Boolean>(false);
    }
    if (!As<Cell>(args[0])->GetFirst()) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

inline std::shared_ptr<Object> IsList::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError("RuntimeError");
    }
    EvalOrError(args[0]);
    if (!Is<Cell>(args[0])) {
        return std::make_shared<Boolean>(false);
    }
    std::vector<std::shared_ptr<Object>> inside = As<Cell>(args[0])->ToVector();
    for (size_t i = 0; i < inside.size(); ++i) {
        if (!inside[i] || Is<Dot>(inside[i])) {
            return std::make_shared<Boolean>(false);
        }
    }
    return std::make_shared<Boolean>(true);
}

inline std::shared_ptr<Object> Cons::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 2) {
        throw RuntimeError("RuntimeError");
    }
    EvalVector(args);
    return std::make_shared<Cell>(
        args[0],
        std::make_shared<Cell>(std::make_shared<Dot>(),
                               std::make_shared<Cell>(args[1], std::shared_ptr<Object>(nullptr))));
}

inline std::shared_ptr<Cell> EvalOneCellOrError(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError("RuntimeError");
    }
    EvalOrError(args[0]);
    if (!Is<Cell>(args[0])) {
        throw RuntimeError("RuntimeError");
    }
    return As<Cell>(args[0]);
}

inline std::shared_ptr<Object> Car::operator()(std::vector<std::shared_ptr<Object>>& args) {
    auto arg = EvalOneCellOrError(args);
    auto ans = arg->GetFirst();
    if (ans) {
        return ans;
    }
    throw RuntimeError("RuntimeError");
}

inline std::shared_ptr<Object> Cdr::operator()(std::vector<std::shared_ptr<Object>>& args) {
    auto arg = EvalOneCellOrError(args);
    if (!arg->GetFirst()) {
        throw RuntimeError("RuntimeError");
    }
    auto ans = arg->GetSecond();
    if (ans) {
        auto ans_cell = As<Cell>(ans);
        if (Is<Dot>(ans_cell->GetFirst())) {
            ans = ans_cell->GetSecond();
            EvalOrError(ans);
            return ans;
        }
        return ans;
    }
    return std::make_shared<Cell>(std::shared_ptr<Object>(nullptr),
                                  std::shared_ptr<Object>(nullptr));
}

inline std::shared_ptr<Object> ToList::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.empty()) {
        return std::make_shared<Cell>(std::shared_ptr<Object>(nullptr),
                                      std::shared_ptr<Object>(nullptr));
    }
    EvalVector(args);
    auto now = std::make_shared<Cell>(args.back(), std::shared_ptr<Object>(nullptr));
    for (size_t i = 1; i < args.size(); ++i) {
        now = std::make_shared<Cell>(args[args.size() - 1 - i], now);
    }
    return now;
}

inline std::shared_ptr<Object> ListElem::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 2) {
        throw RuntimeError("RuntimeError");
    }
    EvalVector(args);
    if (!Is<Cell>(args[0]) || !Is<Number>(args[1])) {
        throw RuntimeError("RuntimeError");
    }
    std::vector<std::shared_ptr<Object>> inside = As<Cell>(args[0])->ToVector();
    int64_t max_size = inside.size();
    if (As<Number>(args[1])->GetValue() >= max_size) {
        throw RuntimeError("RuntimeError");
    }
    return inside[As<Number>(args[1])->GetValue()];
}

inline std::shared_ptr<Object> ListTail::operator()(std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 2) {
        throw RuntimeError("RuntimeError");
    }
    EvalVector(args);
    if (!Is<Cell>(args[0]) || !Is<Number>(args[1])) {
        throw RuntimeError("RuntimeError");
    }
    auto now = args[0];
    for (int i = 0; i < As<Number>(args[1])->GetValue(); ++i) {
        if (Is<Cell>(now)) {
            now = As<Cell>(now)->GetSecond();
        } else {
            throw RuntimeError("RuntimeError");
        }
    }
    if (now) {
        return now;
    }
    return std::make_shared<Cell>(std::shared_ptr<Object>(nullptr),
                                  std::shared_ptr<Object>(nullptr));
}

template <class T>
std::shared_ptr<T> As(std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return !!std::dynamic_pointer_cast<T>(obj);
}