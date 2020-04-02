#include "kv_dict_common_value.h"
#include "util.h"
#include "log.h"

CKvDictCommonValue::CKvDictCommonValue() {

}

CKvDictCommonValue::~CKvDictCommonValue() {

}

int CKvDictCommonValue::Init(const void* input) {

    // 1. check input
    std::string* input_str = const_cast<std::string*>((std::string*)input);

    if(NULL == input_str || input_str->size() == 0) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonValue\tfunction:Init\tinfo:input is null",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    // 2. init m_fields
    m_fields = StringToTokens(*input_str, false, '\t');

    if(2 > m_fields.size()) {
         log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonValue\tfunction:Init\tinfo:input format is illegal",
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

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CKvDictCommonValue\tfunction:Init\tinfo:success init val=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            m_string.c_str());

    return 0;
}

int CKvDictCommonValue::GetVal(void* output) const {

    if(NULL == output) {
        return 1;
    }

    if(0 != m_value.size()) {
        *(std::string*) output = m_value;
    }
    else {
        return 2;
    }

    return 0;
}

int CKvDictCommonValue::SetVal(const void* input) {

    // 1. check input
    std::string* input_str = const_cast<std::string*>((std::string*)input);

    if(NULL == input_str || input_str->size() == 0) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CKvDictCommonValue\tfunction:Init\tinfo:input is null",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    // 2. init m_fields, warning: m_fields from SetVal is not same from Init
    m_fields = StringToTokens(*input_str, false, '\t');

    // 3. get m_value
    m_value = m_fields[0];

    // 4. get m_string
    m_string = *input_str;
    return 0;
}

int CKvDictCommonValue::ToString(void* output) const{

    if(NULL == output) {
        return 1;
    }
    if(0 != m_string.size()) {
        *(std::string*) output = m_string;
    }
    else {
        return 2;
    }

    return 0;
}

int CKvDictCommonValue::Compare(const IValue& v) const{
    return 0;
}

int CKvDictCommonValue::Func(const void* input, void* output) {

    return 0;
}


