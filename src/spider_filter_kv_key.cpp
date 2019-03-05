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
    m_key_build_dict = fields[0];

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CSpiderFilterKvKey\tfunction:Init\tinfo:init success key=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            m_key_build_dict.c_str());

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
    m_key_query_dict = *input_str;
    m_key_build_dict.clear();

    return 0;
}

int CSpiderFilterKvKey::GetKey(void* output) const {
    if(0 != m_key_build_dict.size()) {
        return GetKeyBuildDict(output);
    }
    else {
        return GetKeyQueryDict(output);
    }
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

int CSpiderFilterKvKey::GetKeyBuildDict(void* output) const {

    std::string target_key_str;
    if(0 == m_key_query_dict.size()) {
        target_key_str = m_key_build_dict;
    }
    else {
        target_key_str = m_key_query_dict;
    }

    if(target_key_str.size() <= 0) {
        return 1;
    }

    *(std::string*)output = target_key_str;

    return 0;
}

int CSpiderFilterKvKey::GetKeyQueryDict(void* output) const {
    std::string target_key_str;
    if(0 == m_key_query_dict.size()) {
        target_key_str = m_key_build_dict;
    }
    else {
        target_key_str = m_key_query_dict;
    }

    if(target_key_str.size() <= 0) {
        return 1;
    }

    // 1. check target_key_str is url or not 
    bool is_url = false;
    if(0 == target_key_str.find("http://")
            || 0 == target_key_str.find("https://")
            || 0 == target_key_str.find("ftp://")
            || 0 == target_key_str.find("rtsp://")
            || 0 == target_key_str.find("mms://")) {
        is_url = true;
    }

    if(!is_url) {
        // if target_key_str is url, the output shall be "url \t host \t domain"
        *(std::string*)output = target_key_str;
        return 0;
    }

    std::string candidate_key = target_key_str;

    // 2. get host
    std::vector<std::string> url_parts = StringToTokens(target_key_str, false, '/');
    if(2 > url_parts.size()) {
        *(std::string*)output = candidate_key;
        return 0;
    }

    std::string host = url_parts[1];
    candidate_key += "\t" + host;

    // 3. get domain
    std::vector<std::string> host_parts = StringToTokens(host, false, '.');
    size_t host_parts_len = host_parts.size();

    if(2 > host_parts_len) {
        *(std::string*)output = candidate_key;
        return 0;
    }

    std::string domain = host_parts[host_parts_len - 2] + "." + host_parts[host_parts_len - 1];
    candidate_key += "\t" + domain;

    // 4. return candidate_key
    *(std::string*)output = candidate_key;

    return 0;
}
