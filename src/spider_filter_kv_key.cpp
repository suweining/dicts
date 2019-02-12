#include <vector>
#include "spider_filter_kv_key.h"
#include "log.h"
#include "util.h"


CSpiderFilterKvKey::CSpiderFilterKvKey() {

}

CSpiderFilterKvKey::~CSpiderFilterKvKey() {

}

int CSpiderFilterKvKey::Init(const void* input) {

    // 1. check input
    if(NULL == input){
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterKvKey\tfunction:Init\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    std::string* input_str = (std::string*)input;

    if(0 == input_str->size()) {
         log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterKvKey\tfunction:Init\tinfo:input's size is 0",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // 2. get m_key and input format: $key \t $value
    std::vector<std::string> fields = StringToTokens(*input_str, false, '\t');
    if(2 > fields.size()) {
          log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterKvKey\tfunction:Init\tinfo:input's field < 2",
                __FILE__,
                __LINE__,
                pthread_self());
          return 3;
    }

    m_key = fields[0];

    return 0;
}

int CSpiderFilterKvKey::SetKey(const void* input) {

    // 1. check input
    if(NULL == input){
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterKvKey\tfunction:SetKey\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    const std::string* input_str = (const std::string*)input;
    if(input_str->size() == 0) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterKvKey\tfunction:SetKey\tinfo:input is zero",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2; 
    }

    // 2. set m_key
    m_key = *input_str;

    return 0;
}

int CSpiderFilterKvKey::GetKey(void* output) const {

    if(m_key.size() <= 0) {
        return 1;
    }
    *(std::string*)output = m_key;

    return 0;
}

int CSpiderFilterKvKey::ToString(void* output) const {

    if(m_key.size() <= 0) {
        return 1;
    }
    *(std::string*)output = m_key;


    return 0;
}

int CSpiderFilterKvKey::Compare(const IKey& key) const {
    return 0;
}

int CSpiderFilterKvKey::Func(const void* input, void* output) {
    return 0;
}


