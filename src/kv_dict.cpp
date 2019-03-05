#include <sstream>
#include "kv_dict.h"
#include "util.h"
#include "log.h"

CKvDict::CKvDict() : m_item_count(0), m_hash_capacity_init(65535) {

    m_hash_dict_engine.Initial(m_hash_capacity_init);
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
        m_record_repo[val]->value = const_cast<IValue*>(&value);
    }
    else{
        Record* record = new Record();

        record->key      = const_cast<IKey*>(&key);
        record->value    = const_cast<IValue*>(&value);


        m_record_repo.push_back(record);
        val = m_record_repo.size() - 1;
        m_hash_dict_engine.AddItem(key_str, val);

        ++ m_item_count;
    }

    return 0;
}


int CKvDict::Add(const IKey& key, const IValue& value) {
    std::string key_str;
    if(key.GetKey(&key_str)) {
        log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Add\tinfo:GetKey fail",
                __FILE__,
                __LINE__,
                pthread_self());

        return 1;
    }

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Add\tinfo:key=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            key_str.c_str());

    VALUE_TYPE val;
    if(m_hash_dict_engine.GetItem(key_str.c_str(), val)) {
        log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Add\tinfo:key=%s is exist, add fail",
                __FILE__,
                __LINE__,
                pthread_self(),
                key_str.c_str());
        // if key exist, drop
        return 2;
    }

    Record* record = new Record();

    record->key      = const_cast<IKey*>(&key);
    record->value    = const_cast<IValue*>(&value);

    m_record_repo.push_back(record);
    val = m_record_repo.size() - 1;

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Add\tinfo:add key success. key=%s, value=%d",
            __FILE__,
            __LINE__,
            pthread_self(),
            key_str.c_str(),
            val);

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
    log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Get\tinfo:begin",
            __FILE__,
            __LINE__,
            pthread_self());

    if(key.GetKey(&key_str)) {
        return 1;
    }

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Get\tinfo:key=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            key_str.c_str());


    std::vector<std::string> key_vec = StringToTokens(key_str, false, '\t');

    FOR_EACH(key_vec_itr, key_vec) {

        if(!m_hash_dict_engine.GetItem(key_vec_itr->c_str(), val)) {
            continue;
        }

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Get\tinfo:key=%s hit val=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                key_str.c_str(),
                val);


        if(m_record_repo.size() <= val) {
            continue;
        }

        value->push_back(m_record_repo[val]->value);
    }

    return 0;
}

int CKvDict::Load(const std::string& dict_data_load_path) {
    // adopt this interface
    return 0;
}

int CKvDict::Dump(const std::string& dict_data_dump_path) {
    // adopt this interface
    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCKvDict::Dump dict_data_dump_path:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            dict_data_dump_path.c_str());

    std::ofstream out_stream(dict_data_dump_path.c_str(), std::ifstream::out);
    if(!out_stream.is_open()) {

        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tCKvDict::Dump failed to open dict_data_dump_path:%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                dict_data_dump_path.c_str());

        return 1;
    }

    FOR_EACH(record_repo_itr, m_record_repo) {
        std::string key_str, value_str;
        if((*record_repo_itr)->key->ToString(&key_str)
                || (*record_repo_itr)->value->ToString(&value_str)) {

            log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\tCKvDict::Dump failed to ToString",
                    __FILE__,
                    __LINE__,
                    pthread_self());

            continue;
        }

        out_stream << key_str << "\t" << value_str << std::endl;
    }

    out_stream.close();
    return 0;
}

int CKvDict::Finalize() {
    // no need
    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CKvDict\tfunc:Finalize\tinfo:item_count:%d",
            __FILE__,
            __LINE__,
            pthread_self(),
            m_item_count);


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
