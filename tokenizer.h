#pragma once

#include "error.h"

#include <variant>
#include <vector>
#include <optional>
#include <istream>
#include <cctype>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : data_(in) {
        Next();
    }

    bool IsEnd() {
        return has_end_;
    }

    bool IsSomeSymbol(char c) {
        return c == '<' || c == '>' || c == '=' || c == '*' || c == '#' || c == '/';
    }

    bool IsOtherSymbol(char c) {
        return c == '?' || c == '!' || c == '-';
    }

    void Next() {
        has_end_ = true;
        {
            int c = data_->peek();
            while (data_->good() && (std::isspace(c))) {
                data_->get();
                c = data_->peek();
            }
        }
        if (data_->good()) {
            char c = data_->get();
            if (c == '\'') {
                saved_token_ = QuoteToken();
            } else if (c == '.') {
                saved_token_ = DotToken();
            } else if (c == '(') {
                saved_token_ = BracketToken::OPEN;
            } else if (c == ')') {
                saved_token_ = BracketToken::CLOSE;
            } else if (c == '+') {
                if (!data_->good() || !std::isdigit(data_->peek())) {
                    saved_token_ = SymbolToken{"+"};
                } else {
                    std::string num = "+";
                    while (data_->good() && std::isdigit(data_->peek())) {
                        num += data_->get();
                    }
                    saved_token_ = ConstantToken{std::stoi(num)};
                }
            } else if (c == '-') {
                if (!data_->good() || !std::isdigit(data_->peek())) {
                    saved_token_ = SymbolToken{"-"};
                } else {
                    std::string num = "-";
                    while (data_->good() && std::isdigit(data_->peek())) {
                        num += data_->get();
                    }
                    saved_token_ = ConstantToken{std::stoi(num)};
                }
            } else if (std::isdigit(c)) {
                std::string num;
                num += c;
                while (data_->good() && std::isdigit(data_->peek())) {
                    num += data_->get();
                }
                saved_token_ = ConstantToken{std::stoi(num)};
            } else if (std::isalpha(c) || IsSomeSymbol(c)) {
                std::string name;
                name += c;
                int c1 = data_->peek();
                while (data_->good() &&
                       (IsSomeSymbol(c1) || std::isalnum(c1) || IsOtherSymbol(c1))) {
                    name += data_->get();
                    c1 = data_->peek();
                }
                saved_token_ = SymbolToken{std::move(name)};
            } else {
                throw SyntaxError("SyntaxError");
            }
            has_end_ = false;
        }
    }

    Token GetToken() {
        return saved_token_;
    }

private:
    std::istream* data_;
    Token saved_token_;
    bool has_end_;
};