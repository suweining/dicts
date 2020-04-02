#include "gtest/gtest.h"
#include "redis_dict.h"
#include "redis_common_key.h"
#include "redis_common_value.h"
TEST(RedisDict, Init) {
    CRedisDict* redis_dict = new CRedisDict();
     int ret_code = redis_dict->Init("10.173.194.2:16021");
    ASSERT_TRUE(0 == ret_code);
}

TEST(RedisDict, Set) {
    CRedisDict* redis_dict = new CRedisDict();
    //int ret_code = redis_dict->Init("10.173.194.2:16021");
    int ret_code = redis_dict->Init("10.142.234.19:8411:3a05169f477ab9ba");
    ASSERT_TRUE(0 == ret_code);
 
    std::shared_ptr<CRedisCommonKey> key(new CRedisCommonKey());
    std::shared_ptr<CRedisCommonValue> value(new CRedisCommonValue());

    std::string key_str = "test_key";
    std::string value_str = "test_val";
    key->SetKey(&key_str);
    value->SetVal(&value_str);

    ret_code = redis_dict->Set(key, value);

    ASSERT_TRUE(0 == ret_code);
}


