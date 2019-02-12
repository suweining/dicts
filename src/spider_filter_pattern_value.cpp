#include "spider_filter_pattern_value.h"
#include "log.h"
#include "util.h"

CSpiderFilterPatternValue::CSpiderFilterPatternValue() {

}

CSpiderFilterPatternValue::CSpiderFilterPatternValue(const std::string& line) {

}

CSpiderFilterPatternValue::~CSpiderFilterPatternValue() {

}

int CSpiderFilterPatternValue::Init(const void* input) {

    // 1. check input
    std::string* input_str = const_cast<std::string*>((std::string*)input);

    if(NULL == input_str || input_str->size() == 0) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternValue\tfunction:Init\tinfo:input is null",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    // 2. init m_fields
    m_fields = StringToTokens(*input_str, false, '\t');

    if(2 > m_fields.size()) {
         log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CSpiderFilterPatternValue\tfunction:Init\tinfo:input format is illegal",
                __FILE__,
                __LINE__,
                pthread_self());
         return 2;
    }

    // 3. get m_value
    m_value = m_fields[1];

    // 4. get m_string
    m_string = "";
    size_t fields_len = m_fields.size();
    for(size_t i = 1; i < fields_len - 1; ++i) {
        m_string += m_fields[i] + "\t";
    }

    m_string += m_fields[fields_len - 1];

    return 0;
}

int CSpiderFilterPatternValue::GetVal(void* output) {

    if(0 != m_value.size()) {
        *(std::string*) output = m_value;
    }
    else {
        return 1;
    }

    return 0;
}

int CSpiderFilterPatternValue::SetVal(const void* input) {
    return 0;
}

int CSpiderFilterPatternValue::ToString(void* output) {

    if(0 != m_string.size()) {
        *(std::string*) output = m_string;
    }
    else {
        return 1;
    }

    return 0;
}

int CSpiderFilterPatternValue::Compare(const IValue& v) {

    return 0;
}

int CSpiderFilterPatternValue::Func(const void* input, void* output) {
    return 0;
}


