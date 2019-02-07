#include "spider_filter_pattern_key.h"
#include "log.h"

CSpiderFilterPatternKey::CSpiderFilterPatternKey() {

}

CSpiderFilterPatternKey::~CSpiderFilterPatternKey() {

}

/*
 * TODO: reverse the host part
 * */
int CSpiderFilterPatternKey::genKey(std::string* key_string) {

    std::string& pattern = *key_string;

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


    return 0;
}
int CSpiderFilterPatternKey::Init(const void* input) {

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

    // get pattern field
    // input: $key \t $value
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

    // pattern_field is url pattern, we need reverse the host part

    return 0;
}
