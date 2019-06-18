#include "gtest/gtest.h"
#include "match_engine_singleton.h"

TEST(CMatchEngineSingleton, Construct) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("null");
    ASSERT_TRUE(rc != 0);
}

TEST(CMatchEngineSingleton, Load) {

    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
}
