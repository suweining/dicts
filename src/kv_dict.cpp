#include <sstream>
#include "kv_dict.h"

CKvDict::CKvDict() : m_item_count(0) {

}

CKvDict::~CKvDict() {

}

int CKvDict::Set(const IKey& key, const IValue& value) {
    std::string key_str;
    if(key.GetKey(&key_str)) {
        return 1;
    }

    VALUE_TYPE val;
    if(m_hash_dict_engine.GetItem(key_str.c_str(), val)) {
        m_value_repo[val] = const_cast<IValue*>(&value);
    }
    else{
        m_value_repo.push_back(const_cast<IValue*>(&value));
        val = m_value_repo.size() - 1;

        m_hash_dict_engine.AddItem(key_str, val);

        ++ m_item_count;
    }

    return 0;
}


int CKvDict::Add(const IKey& key, const IValue& value) {
    std::string key_str;
    if(key.GetKey(&key_str)) {
        return 1;
    }

    VALUE_TYPE val;
    if(m_hash_dict_engine.GetItem(key_str.c_str(), val)) {
        // if key exist, drop
        return 2;
    }

    m_value_repo.push_back(const_cast<IValue*>(&value));
    val = m_value_repo.size() - 1;

    m_hash_dict_engine.AddItem(key_str, val);

    ++ m_item_count;
    return 0;
}

int CKvDict::Del(const IKey& key) {
    std::string key_str;
    VALUE_TYPE val;

    if(key.GetKey(&key_str)) {
        return 1;
    }

    if(!m_hash_dict_engine.GetItem(key_str.c_str(), val)) {
        // key not exist
        return 2;
    }

    m_hash_dict_engine.ModifyItem(key_str.c_str(), -1);

    -- m_item_count;

    return 0;
}


int CKvDict::Get(const IKey& key, std::vector<IValue*>* value) {
    std::string key_str;
    VALUE_TYPE val;

    if(key.GetKey(&key_str)) {
        return 1;
    }

    if(!m_hash_dict_engine.GetItem(key_str.c_str(), val)) {
        return 2;
    }

    if(m_value_repo.size() <= val) {
        return 3;
    }

    value->push_back(m_value_repo[val]);

    return 0;
}

int CKvDict::Load(const std::string& dict_data_load_path) {
    // adopt this interface
    return 0;
}

int CKvDict::Dump(const std::string& dict_data_load_path) {
    // adopt this interface
    return 0;
}

int CKvDict::Finalize() {
    // no need
    return 0;
}

int CKvDict::Info(std::string* info) {

    std::ostringstream oss;

    oss << "item_count:" << m_item_count << std::endl;
    *info = oss.str();

    return 0;
}

int CKvDict::Clear() {
    return 0;
}
