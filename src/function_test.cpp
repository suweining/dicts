#include <vector>
#include <string>
#include <iostream>
#include "log.h"
#include "match_engine_level.h"

int main() {

    log_init("../logs/log", LOG_YEAR_DAY_HOUR, LOG_DEBUG, false);

    CMatchEngineLevel match_engine_level("../config/config.ini");

    match_engine_level.Init();
    match_engine_level.Load();
    std::vector<std::string> value_vec;
    std::string input_line;

    while(std::getline(std::cin, input_line)) {

        if(input_line == "quit") break; 

        if(input_line == "reload") {

            match_engine_level.Reload();

            std::cout << "reload success" << std::endl;
            continue; 
        }

        match_engine_level.GetEngine("matchine1", input_line, &value_vec);
        size_t value_len = value_vec.size();

        for(size_t i = 0; i < value_len; ++i) {
            std::cout << "hit[" << i << "]:" << value_vec[i]  << std::endl;
        }
        value_vec.clear();
    }

    std::cout << "dump..." << std::endl;
    match_engine_level.Dump();
    return 0;
}
