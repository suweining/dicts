#include <vector>
#include <string>

#include "spider_filter_pattern_key.h"
#include "log.h"
#include "util.h"


CSpiderFilterPatternKey::CSpiderFilterPatternKey() {
        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey::Construct",
                __FILE__,
                __LINE__,
                pthread_self());
}

CSpiderFilterPatternKey::~CSpiderFilterPatternKey() {
        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey::UnConstruct",
                __FILE__,
                __LINE__,
                pthread_self());
}

int CSpiderFilterPatternKey::Init(const void* input) {

    // 1. check input
    if(NULL == input){

        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());

        return 1;
    }

    std::string* input_str = (std::string*)input;

    if(0 == input_str->size()) {

         log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input's size is 0",
                __FILE__,
                __LINE__,
                pthread_self());

        return 2;
    }

    // 2. get pattern field, and input format: $key \t $value
    std::string pattern_field;

    std::vector<std::string> fields = StringToTokens(*input_str, false, '\t');
    if(2 > fields.size()) {

          log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input's field < 2",
                __FILE__,
                __LINE__,
                pthread_self());

          return 3;
    }

    // pattern_field is the first field
    pattern_field = fields[0];

    if(0 == pattern_field.size()) {
           log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input's pattern_field is 0",
                __FILE__,
                __LINE__,
                pthread_self());
          return 4;
    }

    // 3. set m_string
    m_string = pattern_field;
    m_key_build_dict = pattern_field;
    m_key_query_dict.clear();

    return 0;
}

int CSpiderFilterPatternKey::SetKey(const void* input) {
    // 1. check input
    if(NULL == input){
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:SetKey\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    const std::string* input_str = (const std::string*)input;
    if(input_str->size() == 0) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:SetKey\tinfo:input is zero",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2; 
    }

    // 2. set m_string
    m_string = *input_str;
    m_key_query_dict = * input_str;
    m_key_build_dict.clear();

    return 0;
}

int CSpiderFilterPatternKey::GetKey(void* output) const {

    if(m_string.size() <= 0) {
        return 1;
    }

    if(0 != m_key_build_dict.size()) {
        return GetBuildDictKey(output);
    }
    else {
        return GetQueryDictKey(output);
    }

    return 0;
}

int CSpiderFilterPatternKey::ToString(void* output) const {

    if(m_string.size() <= 0) {
        return 1;
    }

    *(std::string*)output = m_string;

    return 0;
}

int CSpiderFilterPatternKey::Compare(const IKey& key) const {

    std::string key_str("");

    if(key.GetKey(&key_str)) {

        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Compare\tinfo:get key failed",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    return m_string.compare(key_str);
}

int CSpiderFilterPatternKey::Func(const void* input, void* output) {

    // NULL
    return 0;
}

int CSpiderFilterPatternKey::GetBuildDictKey(void *output) const {

    std::string pattern;
    if(0 == m_key_build_dict.size()) {
       pattern = m_string;
    }
    else {
        pattern = m_key_build_dict;
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey::GetBuildDictKey pattern=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            pattern.c_str());

    // 1. get host
    std::vector<std::string> url_parts = StringToTokens(pattern, false, '/');
    size_t url_parts_len = url_parts.size();
    if(url_parts_len < 2) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey::GetBuildDictKey pattern's format is error",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    std::string host = url_parts[1];
    bool host_end_with_dollar = false;
    size_t host_len = host.size();

    if(host[host_len - 1] == '$') {
        host_end_with_dollar = true;
    }

    // 2. erase the $ (if exist) in host
    if(url_parts_len == 2 && host_end_with_dollar) {
        host.erase(host.begin() + host_len - 1);
        host_len = host.size();
    }

    if(0 == host_len) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:genKey\tinfo:host's format is error", 
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // 3. reversal host
    std::vector<std::string> host_parts = StringToTokens2(host, false, "\\.");
    size_t host_parts_len = host_parts.size();
    std::string reversal_host;
    for(size_t i = host_parts_len - 1; i > 0; --i) {
        reversal_host += host_parts[i] + "\\.";
    }
    reversal_host += host_parts[0];

    if(host_end_with_dollar) {
        reversal_host += "$";
    }

    // 4. gen target_key
    std::string target_key;
    for(size_t i = 0; i < url_parts_len; ++i) {
        if(i == 0) {
            target_key += url_parts[0] + "//";
        }
        else if(i == 1) {
            target_key += reversal_host;
        }
        else {
            target_key += "/" + url_parts[i];
        }
    }

    *(std::string*)output = target_key;
    return 0;
}

int CSpiderFilterPatternKey::GetQueryDictKey(void* output) const {

    std::string pattern;
    if(0 == m_key_query_dict.size()) {
       pattern = m_string;
    }
    else {
        pattern = m_key_query_dict;
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey::GetQueryDictKey pattern=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            pattern.c_str());

    // 1. get host
    std::vector<std::string> url_parts = StringToTokens(pattern, false, '/');
    size_t url_parts_len = url_parts.size();
    if(url_parts_len < 2) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey::GetQueryDictKey pattern's format is error",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    std::string host = url_parts[1];
    // 2. reversal host
    std::vector<std::string> host_parts = StringToTokens2(host, false, ".");
    size_t host_parts_len = host_parts.size();
    std::string reversal_host;
    for(size_t i = host_parts_len - 1; i > 0; --i) {
        reversal_host += host_parts[i] + ".";
    }
    reversal_host += host_parts[0];

    // 3. gen target_key
    std::string target_key;
    for(size_t i = 0; i < url_parts_len; ++i) {
        if(i == 0) {
            target_key += url_parts[0] + "//";
        }
        else if(i == 1) {
            target_key += reversal_host;
        }
        else {
            target_key += "/" + url_parts[i];
        }
    }

    *(std::string*)output = target_key;
    return 0;
}
