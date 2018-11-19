#include "gtest/gtest.h"
#include "re2/set.h"
#include "pattern_dict.h"
#include "pattern_dict_struct_key.h"


TEST(PatternDictStruct, Init1) {
    std::string regex;
    std::string target;
    while(true) {
        std::cout << "input regex target" << std::endl;
        std::cin >> regex >> target;
        if(RE2::PartialMatch(target.c_str(), regex.c_str())) {
            std::cout << "target:" << target << "\thit regex:" << regex << std::endl; 
        }
    }
    ASSERT_TRUE(0 == 0);
}
TEST(PatternDictStruct, Init) {
    PatternDict* pd = new PatternDict();
    int ret = pd->Load("./unit/data/struct_test.data");
    PatternDictStructKey* k = new PatternDictStructKey();
    std::string url("https://www.haozu.com/sh/shouesshangpu/test/");
    while(std::cin >> url) {
        k->SetKey(&url);
        std::vector<IValue*> values;
        ret = pd->Get(*k, &values);
        std::cout << "value.size:" << values.size() << std::endl;
        ASSERT_TRUE(ret == 0);
    }
}
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

