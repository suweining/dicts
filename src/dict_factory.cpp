#include "dict_factory.h"
#include "kv_dict.h"
#include "pattern_dict.h"
#include "redis_dict.h"

CDictFactory::CDictFactory(){

}

CDictFactory::CDictFactory(const std::string& dict_class_list_str) : m_dict_class_list_str(dict_class_list_str){

}

CDictFactory::~CDictFactory(){

}

IDict* CDictFactory::GenDictInstance(const std::string& type){
#define NEW_DICT_CLASS(TARGET_CLASS) \
    if(type == #TARGET_CLASS) \
        return new TARGET_CLASS();

    NEW_DICT_CLASS(CKvDict);
    NEW_DICT_CLASS(CPatternDict);
    NEW_DICT_CLASS(CRedisDict);
    return NULL;
}

