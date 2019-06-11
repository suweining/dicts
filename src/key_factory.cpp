#include <string>
#include "key_factory.h"
#include "spider_filter_kv_key.h"
#include "spider_filter_pattern_key.h"
#include "redis_common_key.h"
#include "log.h"

CKeyFactory::CKeyFactory() {

}

CKeyFactory::CKeyFactory(const std::string& key_class_list_str) : m_key_class_list_str(key_class_list_str) {

}

CKeyFactory::~CKeyFactory() {

}

int CKeyFactory::SetKeyClassList(const std::string& key_class_list_str) {

    m_key_class_list_str = key_class_list_str;

    return 0;
}
IKey* CKeyFactory::GenKeyInstance(const std::string& type) {
// TODO: shall use class name to new target class
#define NEW_KEY_CLASS(TARGET_CLASS) \
    if(type == #TARGET_CLASS) \
        return new TARGET_CLASS();

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKeyFactory::GenDictInstance type=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                type.c_str());

    if(type == "CSpiderFilterKvKey") {

        return new CSpiderFilterKvKey();
    }

    if(type == "CSpiderFilterPatternKey") {
        return new CSpiderFilterPatternKey();
    }

    if(type == "CRedisCommonKey") {
        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKeyFactory::GenDictInstance gen CRedisCommonKey",
                __FILE__,
                __LINE__,
                pthread_self());


        return new CRedisCommonKey();
    }
    return NULL;
}
