#include <iostream>
#include "pattern_dict_struct_key.h"
#include "util.h"
#include "log.h"


PatternDictStructKey::PatternDictStructKey() {

}

PatternDictStructKey::~PatternDictStructKey() {

}

int PatternDictStructKey::Init(const void* input) {

    if(NULL == input){
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Init\tinfo:input is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    std::string* input_str = (std::string*)input;

    if(input_str->size() == 0) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Init\tinfo:input's size is 0",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1; 
    }

    // get pattern field
    std::string pattern_field;
    std::vector<std::string> fields = StringToTokens(*input_str, false, '\t');
    FOR_EACH(field_itr, fields) {
        size_t pos = field_itr->find("pattern:");
        if(pos == 0) {
            pattern_field = *field_itr; 
            break;
        }
    }
    if(pattern_field.size() == 0) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Init\tinfo:pattern_field's size is 0", 
                __FILE__,
                __LINE__,
                pthread_self());
        return 2; 
    }
    std::string pattern = (StringToTokens(pattern_field, false, ':', true))[1];
    // TODO: check pattern's syntax
    m_string = pattern;
    // get host
    std::vector<std::string> url_parts = StringToTokens(pattern, false, '/');
    size_t url_parts_len = url_parts.size();
    if(url_parts_len < 2) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Init\tinfo:pattern's format is error", __FILE__, __LINE__, pthread_self());
        return 3; 
    }

    std::string host = url_parts[1];
    bool host_end_with_dollar = false;
    size_t host_len = host.size();

    if(host[host_len - 1] == '$') {
        host_end_with_dollar = true;
    }

    // erase the dollar in host end
    if(url_parts_len == 2 && host_end_with_dollar) {
        host.erase(host.begin() + host_len - 1);
        host_len = host.size();
    }
    if(0 == host_len) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Init\tinfo:host's format is error", __FILE__, __LINE__, pthread_self());
        return 4;
    }
    // reversal host
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
    // joint url as m_key
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

int PatternDictStructKey::SetKey(const void* input) {

    if(NULL == input){
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:SetKey\tinfo:input is NULL", __FILE__, __LINE__, pthread_self());
        return 1;
    }

    const std::string* input_str = (const std::string*)input;
    if(input_str->size() == 0) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:SetKey\tinfo:input is zero", __FILE__, __LINE__, pthread_self());
        return 1; 
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:SetKey\tinfo:input is %d", __FILE__, __LINE__, pthread_self(), input_str->size());
    // get host
    std::vector<std::string> url_parts = StringToTokens(*input_str, false, '/');
    size_t url_parts_len = url_parts.size();
    if(url_parts_len < 2) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:SetKey\tinfo:pattern's format is error", __FILE__, __LINE__, pthread_self());
        return 2; 
    }

    std::string host = url_parts[1];
    if(host.size() == 0) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:SetKey\tinfo:pattern's format is error", __FILE__, __LINE__, pthread_self());
        return 3;
    }
    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:SetKey\tinfo:host is %s", __FILE__, __LINE__, pthread_self(), host.c_str());
     // reversal host
    std::vector<std::string> host_parts = StringToTokens(host, false, '.');
    size_t host_parts_len = host_parts.size();
    std::string reversal_host;
    for(size_t i = host_parts_len - 1; i > 0; --i) {
        reversal_host += host_parts[i] + ".";
    }
    reversal_host += host_parts[0];

    // joint url as m_key
    m_key.clear();

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
int PatternDictStructKey::GetKey(void* output) const {
    if(m_key.size() <= 0) {
        return 1;
    }

    *(std::string*)output = m_key;
    return 0;
}

int PatternDictStructKey::ToString(void* output) const {
    if(m_string.size() <= 0) {
        return 1;
    }

    *(std::string*)output = m_string;
    return 0;
}

int PatternDictStructKey::Compare(const IKey& key) const {
    std::string key_str;

    if(key.GetKey(&key_str)) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:PatternDictStructKey\tfunction:Compare\tinfo:get key failed", __FILE__, __LINE__, pthread_self());
    }

    return m_key.compare(key_str);
}

int PatternDictStructKey::Func(const void* input, void* output) {
    // NULL
    return 0;
}
