#include "scheme.h"
#include "tokenizer.h"
#include "parser.h"
#include <string>
#include <sstream>
#include <iostream>

class Interpreter {
public:
    std::string Run(const std::string& code) {
        std::stringstream my_stream;
        my_stream << code;
        Tokenizer tokenizer(&my_stream);
        auto first = Read(&tokenizer);
        std::shared_ptr<Object> second = std::make_shared<Object>();
        if (!tokenizer.IsEnd()) {
            second = Read(&tokenizer);
        }
        EvalOrError(first);
        EvalOrError(second);
        std::string ans = first->ToString();
        ans += second->ToString();
        return ans;
    }
};