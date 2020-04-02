#include <vector>
#include "kv_dict_common_key.h"
#include "util.h"
#include "log.h"
CKvDictCommonKey::CKvDictCommonKey(){

}

CKvDictCommonKey::~CKvDictCommonKey(){

}

int CKvDictCommonKey::Init(const void* input){
    // 1. check input
    if(NULL == input){
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonKey\tfunction:Init\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    std::string* input_str = (std::string*)input;

    if(0 == input_str->size()) {
         log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonKey\tfunction:Init\tinfo:input's size is 0",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // 2. get m_key and input format: $key \t $value
    const std::vector<std::string>& fields = StringToTokens(*input_str, false, '\t');
    if(2 > fields.size()) {
          log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonKey\tfunction:Init\tinfo:input's field < 2",
                __FILE__,
                __LINE__,
                pthread_self());
          return 3;
    }

    m_key = fields[0];
    m_key_build_dict = fields[0];
    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CKvDictCommonKey\tfunction:Init\tinfo:init success key=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            m_key_build_dict.c_str());

    return 0;
}

int CKvDictCommonKey::SetKey(const void* input) {

    // 1. check input
    if(NULL == input){
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonKey\tfunction:SetKey\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    const std::string* input_str = (const std::string*)input;
    if(input_str->size() == 0) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonKey\tfunction:SetKey\tinfo:input is zero",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2; 
    }

    // 2. set m_key
    m_key = *input_str;
    m_key_query_dict = *input_str;
    m_key_build_dict.clear();

    return 0;
}

int CKvDictCommonKey::GetKey(void* output) const {

    if(NULL == output) {
        return 1;
    }
    if(0 != m_key_build_dict.size()) {
        *(std::string*) output = m_key_build_dict;
    }
    else {
        *(std::string*) output = m_key_query_dict;
    }

    return 0;
}


int CKvDictCommonKey::ToString(void* output) const{

    if(m_key.size() <= 0) {
        return 1;
    }
    *(std::string*)output = m_key;

    return 0;
}

int CKvDictCommonKey::Compare(const IKey& k) const{

    return 0;
}

int CKvDictCommonKey::Func(const void* input, void* output){

    return 0;
}
