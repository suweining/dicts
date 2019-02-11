#include "spider_filter_pattern_key.h"
#include "log.h"

CSpiderFilterPatternKey::CSpiderFilterPatternKey() {

}

CSpiderFilterPatternKey::~CSpiderFilterPatternKey() {

}

int CSpiderFilterPatternKey::Init(const void* input) {

    // 1. check input
    if(NULL == input){
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }


    std::string* input_str = static_cast<std::string*>(input);

    if(0 == input_str->size()) {
         log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input's size is 0",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // 2. get pattern field, and input format: $key \t $value
    std::string pattern_field;

    std::vector<std::string> fields = StringToTokens(*input_str, false, '\t');
    if(2 > fields.size()) {
          log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input's field < 2",
                __FILE__,
                __LINE__,
                pthread_self());
          return 3;
    }

    // pattern_field is the first field
    pattern_field = fields[0];

    if(0 == pattern_field.size()) {
           log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:input's pattern_field is 0",
                __FILE__,
                __LINE__,
                pthread_self());
          return 4;
    }

    // 3. pattern_field is url pattern, we need reverse the host part to get m_key
    int rc = genKey(pattern_field);
    if(0 != rc) {
            log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo: genKey errorcode is %d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
            return 5;
    }

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

    // 2. input_str is url pattern, we need reverse the host part to get m_key
    int rc = genKey(*input_str);
    if(0 != rc) {
            log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:SetKey\tinfo: genKey errorcode is %d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
            return 3;
    }

    return 0;
}
int CSpiderFilterPatternKey::GetKey(void* output) const {
    return 0;
}
int CSpiderFilterPatternKey::ToString(void* output) const {
    return 0;
}
int CSpiderFilterPatternKey::Compare(const IKey& key) const {
    return 0;
}
int CSpiderFilterPatternKey::Func(const void* input, void* output) {
    return 0;
}


/*
 * TODO: gen m_key: reverse the host part
 * */
int CSpiderFilterPatternKey::genKey(const std::string& pattern) {

    // 1. get host
    std::vector<std::string> url_parts = StringToTokens(pattern, false, '/');
    size_t url_parts_len = url_parts.size();
    if(url_parts_len < 2) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternKey\tfunction:Init\tinfo:pattern's format is error",
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

    // 4. gen m_key: joint url as m_key
    for(size_t i = 0; i < url_parts_len; ++i) {
        if(i == 0) {
            m_key += url_parts[0] + "//";
        }
        else if(i == 1) {
            m_key += reversal_host;
        }
        else {
            m_key += "/" + url_parts[i];
        }
    }
    return 0;
}

