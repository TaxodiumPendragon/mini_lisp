#include <string>
#include "./tokenizer.h"
#include "./value.h"
#include "./parse.h"

int main() {
    ValuePtr a = std::make_shared<NumericValue>(42);
    std::cout << a->toString() << std::endl;
    /*while (true) {
        try {
            std::cout << ">>> ";
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            for (auto& token : tokens) {
                std::cout << *token << std::endl;
            }
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }*/
}
