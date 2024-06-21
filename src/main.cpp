#include <string>

#include "eval_env.h"
#include "parse.h"
#include "rjsj_test.hpp"
#include "tokenizer.h"
#include "value.h"

struct TestCtx {
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        return value->toString();
    }
};

int main() {
    // RJSJ_TEST(TestCtx, Lv2, Lv2Only);
    /*ValuePtr a = std::make_shared<PairValue>(
        std::make_shared<SymbolValue>("quote"),
        std::make_shared<PairValue>(std::make_shared<NumericValue>(42),
                                    std::make_shared<NilValue>()));
    std::cout << a->toString() << std::endl;*/
    EvalEnv env;
    while (true) {
        try {
            std::cout << ">>> ";
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));  // TokenPtr 不支持复制
            auto value = parser.parse();
            auto result = env.eval(std::move(value));
            std::cout << result->toString() << std::endl;  // test3
            for (auto& token : tokens) {
                std::cout << *token << std::endl;
            }
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
