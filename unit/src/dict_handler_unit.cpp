#include <iostream>

#include "gtest/gtest.h"
#include "busi_dict_handler.h"

TEST(BusiDictHandler, MatchOrder) {
    CBusiDictHandler* bdh = CBusiDictHandler::GetInstance();
    bdh->SetConfig("./unit/config/config.ini");
    int rc = 0;
    rc = bdh->Load();
    ASSERT_TRUE(rc == 0);
    std::vector<std::string> val;
    rc = bdh->MatchOrder("level1", "https://www.haozu.com/sh/shouesshangpu/test/", &val);
    ASSERT_TRUE(rc == 0);

    rc = bdh->MatchOrder("level1", "https://www.haozu.com/sh/shouesshangpu/test/", &val);
    ASSERT_TRUE(rc == 0);


    size_t len = val.size();
    for(size_t i = 0; i < len; ++i) {
        std::cout << val[i] << std::endl;
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
