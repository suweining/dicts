#include "gtest/gtest.h"
#include "re2/set.h"
#include "pattern_dict.h"
#include "pattern_dict_struct_key.h"

TEST(PatternDictStruct, Get) {
    PatternDict* pd = new PatternDict();
    int ret = pd->Load("./unit/data/struct_test.data");
    PatternDictStructKey* k = new PatternDictStructKey();
    std::string url("https://www.haozu.com/sh/shouesshangpu/test/");
    k->SetKey(&url);
    std::vector<IValue*> values;
    ret = pd->Get(*k, &values);
    ASSERT_TRUE(ret == 0);

    size_t len = values.size();
    std::cout << "len:" << len << std::endl;
    for(size_t i = 0; i < len; ++i) {
        std::string v;
        values[i]->Val((void*)&v);
        std::cout << "ret_val:" << v << std::endl;
    }
    ret = pd->Get(*k, &values);
    std::cout << "len:" << len << std::endl;
    for(size_t i = 0; i < len; ++i) {
        std::string v;
        values[i]->Val((void*)&v);
        std::cout << "ret_val:" << v << std::endl;
    }

}
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

