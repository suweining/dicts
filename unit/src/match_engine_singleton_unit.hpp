#include "gtest/gtest.h"
#include "match_engine_singleton.h"
#include <iostream>

TEST(Construct, Func) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("null");
    ASSERT_TRUE(rc != 0);
}

TEST(Load, Func) {

    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
}

TEST(GetEngine, KvDict_Func) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    std::vector<std::string> value;
    ptr->GetEngine("matchine1", "http://www.so.com/index.html", &value);
    ASSERT_TRUE(value.size() == 2);
}

TEST(GetEngine, RedisDict_Func) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    std::vector<std::string> value;
    ptr->GetEngine("matchine3", "so.com", &value);
    ASSERT_TRUE(value.size() == 1);
    std::cout << value[0] << std::endl;
}
TEST(GetEngine, NullEngine) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    std::vector<std::string> value;
    rc = ptr->GetEngine("matchine_null", "http://www.so.com/index.html", &value);
    ASSERT_TRUE(rc == -11);
    ASSERT_TRUE(value.size() == 0);
}

TEST(GetSpec, Func) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    std::vector<std::string> value;
    ptr->GetSpec("matchine1;matchine1", "http://www.so.com/index.html", &value);
    ASSERT_TRUE(value.size() == 4);
}

TEST(Set, Func) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    rc = ptr->Set("matchine1", "unit_test_key", "unit_test_value");
    std::cout << rc << std::endl;
    ASSERT_TRUE(rc == 0);

    std::vector<std::string> value;
    rc = ptr->GetEngine("matchine1", "unit_test_key", &value);
    ASSERT_TRUE(value.size() == 1);
    ASSERT_TRUE(value[0].find("unit_test_value") != std::string::npos);
}

TEST(Set, NullEngine) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    rc = ptr->Set("matchine_null", "unit_test_key", "unit_test_value");
    ASSERT_TRUE(rc == 1);

    std::vector<std::string> value;
    rc = ptr->GetEngine("matchine1", "unit_test_key", &value);
    ASSERT_TRUE(value.size() == 0);
}

TEST(Set, RedisDictTest) {
    CMatchEngineSingleton* ptr = CMatchEngineSingleton::GetInstance();
    ASSERT_TRUE(ptr != NULL);
    int rc = ptr->SetConfig("../config/config.ini");
    ASSERT_TRUE(rc == 0);
    rc = ptr->Load();
    ASSERT_TRUE(rc == 0);
    rc = ptr->Set("matchine3", "unit_test_key", "unit_test_value");
    ASSERT_TRUE(rc == 0);

    std::vector<std::string> value;
    rc = ptr->GetEngine("matchine3", "unit_test_key", &value);
    ASSERT_TRUE(value.size() == 1);
    ASSERT_TRUE(value[0].find("unit_test_value") != std::string::npos);
}

