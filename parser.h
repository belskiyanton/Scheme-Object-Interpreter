#pragma once

#include <memory>

#include "object.h"
#include "tokenizer.h"

inline Token GetOrError(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("SyntaxError");
    }
    return tokenizer->GetToken();
}

inline bool IsCloseBracket(const Token& now) {
    return ((std::get_if<BracketToken>(&now)) &&
            (*std::get_if<BracketToken>(&now) == BracketToken::CLOSE));
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer);

inline std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (IsCloseBracket(GetOrError(tokenizer))) {
        tokenizer->Next();
        return std::shared_ptr<Object>(nullptr);
    }
    std::shared_ptr<Object> first = Read(tokenizer);
    Token now = GetOrError(tokenizer);

    if (std::get_if<DotToken>(&now)) {
        tokenizer->Next();
        auto ans = std::make_shared<Cell>(first, Read(tokenizer));
        if (IsCloseBracket(GetOrError(tokenizer))) {
            tokenizer->Next();
            return ans;
        }
        throw SyntaxError("SyntaxError");
    }
    return std::make_shared<Cell>(first, ReadList(tokenizer));
}

std::shared_ptr<Object> ReadCarefully(Tokenizer* tokenizer);

inline std::shared_ptr<Object> ReadListCarefully(Tokenizer* tokenizer) {
    if (IsCloseBracket(GetOrError(tokenizer))) {
        tokenizer->Next();
        return std::make_shared<Cell>(std::shared_ptr<Object>(nullptr),
                                      std::shared_ptr<Object>(nullptr));
    }
    std::shared_ptr<Object> first = ReadCarefully(tokenizer);
    Token now = GetOrError(tokenizer);

    if (std::get_if<DotToken>(&now)) {
        tokenizer->Next();
        auto second = ReadCarefully(tokenizer);
        if (IsCloseBracket(GetOrError(tokenizer))) {
            tokenizer->Next();
            if (!second || Is<Cell>(second)) {
                if (!As<Cell>(second)->GetFirst()) {
                    return std::make_shared<Cell>(first, std::shared_ptr<Object>(nullptr));
                }
                return std::make_shared<Cell>(first, second);
            } else {
                return std::make_shared<Cell>(
                    first, std::make_shared<Cell>(std::make_shared<Dot>(), second));
            }
        }
        throw SyntaxError("SyntaxError");
    }
    auto second = ReadListCarefully(tokenizer);
    if (Is<Cell>(second) && !As<Cell>(second)->GetFirst()) {
        return std::make_shared<Cell>(first, std::shared_ptr<Object>(nullptr));
    }
    return std::make_shared<Cell>(first, second);
}

inline std::shared_ptr<Object> ReadCarefully(Tokenizer* tokenizer) {
    Token now = GetOrError(tokenizer);
    tokenizer->Next();
    if (ConstantToken* n = std::get_if<ConstantToken>(&now)) {
        return std::make_shared<Number>(n->value);
    } else if (SymbolToken* str = std::get_if<SymbolToken>(&now)) {
        return std::make_shared<Symbol>(std::move(str->name));
    } else if ((std::get_if<BracketToken>(&now)) &&
               (*std::get_if<BracketToken>(&now) == BracketToken::OPEN)) {
        return ReadListCarefully(tokenizer);
    }
    throw SyntaxError("SyntaxError");
}

inline std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    Token now = GetOrError(tokenizer);
    tokenizer->Next();
    if (ConstantToken* n = std::get_if<ConstantToken>(&now)) {
        return std::make_shared<Number>(n->value);
    } else if (SymbolToken* str = std::get_if<SymbolToken>(&now)) {
        return std::make_shared<Symbol>(std::move(str->name));
    } else if ((std::get_if<BracketToken>(&now)) &&
               (*std::get_if<BracketToken>(&now) == BracketToken::OPEN)) {
        Token next = GetOrError(tokenizer);
        if (std::get_if<SymbolToken>(&next) && std::get_if<SymbolToken>(&next)->name == "quote") {
            tokenizer->Next();
            auto ans = ReadCarefully(tokenizer);
            if (IsCloseBracket(GetOrError(tokenizer))) {
                tokenizer->Next();
                return std::make_shared<Cell>(std::make_shared<Symbol>("quote"), ans);
            }
            throw SyntaxError("SyntaxError");
        }
        return ReadList(tokenizer);
    } else if (std::get_if<QuoteToken>(&now)) {
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"), ReadCarefully(tokenizer));
    }
    throw SyntaxError("SyntaxError");
}
