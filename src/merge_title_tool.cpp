#include <vector>
#include <string>
#include <iostream>
#include "util.h"

int main() {
    std::string pre_line = "";
    std::string curr_line = "";
    std::string line;
    while(std::getline(std::cin, line)) {
        if(0 == line.size()) {
            continue;
        }
        if(0 == pre_line.size()) {
            pre_line = line;
            continue;
        }
        curr_line = line;
        const std::vector<std::string>& curr_tokens = StringToTokens(curr_line, false, '\t', false);
        const std::vector<std::string>& pre_tokens = StringToTokens(pre_line, false, '\t', false);
        if(curr_tokens.size() > 2 && pre_tokens.size() > 2 && curr_tokens[0] == pre_tokens[0]) {
            if(3 == curr_tokens.size()) {
                std::cout << curr_tokens[0] << "\t" << pre_tokens[1] << "\t" << curr_tokens[1] << std::endl;
            }
            else {
                std::cout << curr_tokens[0] << "\t" << curr_tokens[1] << "\t" << pre_tokens[1] << std::endl;
            }
            pre_line.clear();
        }
        else {
            pre_line = line;
        }
    }
    return 0;
}
