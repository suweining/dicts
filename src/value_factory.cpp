#include "value_factory.h"
#include "spider_filter_kv_value.h"
#include "spider_filter_pattern_value.h"

CValueFactory::CValueFactory(){}
CValueFactory::CValueFactory(const std::string& value_class_list_str) : m_value_class_list_str(value_class_list_str){}
CValueFactory::~CValueFactory(){}

int CValueFactory::SetValueClassList(const std::string& value_class_list_str){

    m_value_class_list_str = value_class_list_str;

    return 0;
}

IValue* CValueFactory::GenValueInstance(const std::string& type){

#define NEW_VALUE_CLASS(TARGET_CLASS) \
    if(type == #TARGET_CLASS) \
        return new TARGET_CLASS();

    NEW_VALUE_CLASS(CSpiderFilterKvValue);
    NEW_VALUE_CLASS(CSpiderFilterPatternValue);

    return NULL;
}


