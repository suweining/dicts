#include "key_factory.h"

CKeyFactory::CKeyFactory() {

}

CKeyFactory::CKeyFactory(const std::string& key_class_list_str) : m_key_class_list_str(key_class_list_str) {

}

CKeyFactory::~CKeyFactory() {

}

int CKeyFactory::SetKeyClassList(const std::string& key_class_list_str) {

    m_key_class_list_str = key_class_list_str;

    return 0;
}
IKey* CKeyFactory::GenKeyInstance(const std::string& type) {
// TODO: shall use class name to new target class
#define NEW_KEY_CLASS(TARGET_CLASS) \
    if(type == #TARGET_CLASS) \
        return new TARGET_CLASS();

    NEW_KEY_CLASS(CSpiderFilterKvKey)

    NEW_KEY_CLASS(CSpiderFilterKvKey)

    return NULL;
}
