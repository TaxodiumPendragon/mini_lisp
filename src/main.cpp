#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "eval_env.h"
#include "parse.h"
#include "rjsj_test.hpp"
#include "tokenizer.h"
#include "value.h"

struct TestCtx {
    std::shared_ptr<EvalEnv> env;

    TestCtx() : env(EvalEnv::createGlobal()) {}

    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};
bool fileMode = false;
int main(int argc, char* argv[]) {
    RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    /*ValuePtr a = std::make_shared<PairValue>(
        std::make_shared<SymbolValue>("quote"),
        std::make_shared<PairValue>(std::make_shared<NumericValue>(42),
                                    std::make_shared<NilValue>()));
    std::cout << a->toString() << std::endl;*/
    EvalEnv env(nullptr);
    std::ifstream file;
    if (argc > 1) {
        fileMode = true;
        file.open(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << argv[1] << std::endl;
            return 1;
        } else
            std::cout << "File opened successfully" << std::endl;
    }
    while (true) {
        try {
            std::string line;
            if (fileMode) {
                std::getline(file, line);
            } else {
                std::cout << ">>> ";

                std::getline(std::cin, line);
                if (std::cin.eof()) {
                    std::exit(0);
                }
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));  // TokenPtr 不支持复制
            auto value = parser.parse();
            auto result = env.eval(std::move(value));
            if (!fileMode) {
                std::cout << result->toString() << std::endl;  // test3
                for (auto& token : tokens) {
                    std::cout << *token << std::endl;
                }
            }
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
