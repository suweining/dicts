#include <fstream>
#include <string.h>
#include "match_engine_unit.h"
#include "dict_factory.h"
#include "key_factory.h"
#include "value_factory.h"
#include "dict.h"
#include "log.h"
#include "util.h"
#include "ini.h"

CMatchEngineUnit::CMatchEngineUnit() {

}

CMatchEngineUnit::CMatchEngineUnit(const std::string& config, const std::string& engine) : m_config(config), m_engine(engine) {
    log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit\tfunc:Construct\tinfo:config=%s, dict_type=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            m_config.c_str(),
            m_engine.c_str());
}

CMatchEngineUnit::~CMatchEngineUnit() {

}

int CMatchEngineUnit::SetConfig(const std::string& config){
    m_config = config;
    return 0;
}

int CMatchEngineUnit::SetEngine(const std::string& engine){
    m_engine = engine;
    return 0;
}

int CMatchEngineUnit::Init() {
    // 1. check m_engine and m_config
    if(0 == m_engine.size() || 0 == m_config.size()) {

        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit\tfunc:Init\tinfo:m_dict_type or m_config is null",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    // 2. read config
    if(readConfig()) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit\tfunc:Init\tinfo:read config error",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // 3. init the dict
    m_dict = CDictFactory::GetInstance()->GenDictInstance(m_dict_type);
    return 0;
}

int CMatchEngineUnit::Load(){
    return Reload(m_load_path);
}

int CMatchEngineUnit::Reload(){
    return Reload(m_load_path);
}

int CMatchEngineUnit::Reload(const std::string& load_path){

    // 1. read load_path
    std::ifstream in_stream(load_path.c_str(), std::ifstream::in);
    if(!in_stream.is_open()) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tCMatchEngineUnit::ReLoad fail to open:%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                load_path.c_str());
        return 1;
    }

    log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReLoad load_path:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            load_path.c_str());


    std::string line;
    while(std::getline(in_stream, line) && !in_stream.eof()) {
        // here need shared_ptr
        IKey* key_ptr = CKeyFactory::GetInstance()->GenKeyInstance(m_key_type);
        IValue* value_ptr = CValueFactory::GetInstance()->GenValueInstance(m_value_type);

        if(NULL == key_ptr || NULL == value_ptr) {
            log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReLoad gen key(%s) or value(%s) error",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    m_key_type.c_str(),
                    m_value_type.c_str());
            continue;
        }

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReLoad readline:%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                line.c_str());

        if(key_ptr->Init(&line) || value_ptr->Init(&line)) {
            log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineUnit::ReLoad key or value init fail, and line:%s",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    line.c_str());

            continue;
        }

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tCMatchEngineUnit::ReLoad init key and value success line:%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                line.c_str());

        // 4. add to dict
        if(NULL == m_dict || m_dict->Add(*key_ptr, *value_ptr)) {
            log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineUnit::ReLoad fail to add:%s",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    line.c_str());
            continue;
        }
    }
    in_stream.close();

    // Finalize
    if(NULL == m_dict || m_dict->Finalize()) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CMatchEngineUnit\tfunction:Load\tinfo:fail to Finalize",
                __FILE__,
                __LINE__,
                pthread_self());
        return 3; 
    }

    log(LOG_NOTICE, "%s:%d\ttid:%lld\tCMatchEngineUnit::ReLoad load success dict:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            load_path.c_str());

    return 0;
}

int CMatchEngineUnit::Dump() {
    if(m_dump_path.size() != 0 && 0 == Dump(m_dump_path)) {
       return 0;
    }
    return 1;
}
int CMatchEngineUnit::Dump(const std::string& dump_path) {

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::Dump dump.path:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            dump_path.c_str());


    if(NULL == m_dict || m_dict->Dump(dump_path)) {
        return 1;
    }
    return 0;
}


int CMatchEngineUnit::Set(const std::string& key, const std::string& value){

    IKey* key_ptr = CKeyFactory::GetInstance()->GenKeyInstance(m_key_type);
    IValue* value_ptr = CValueFactory::GetInstance()->GenValueInstance(m_value_type);

    if(key_ptr->SetKey(&key) || value_ptr->SetVal(&value)) {
        return 1;
    }

    if(NULL == m_dict || m_dict->Set(*key_ptr, *value_ptr)) {
        return 2;
    }

    return 0;

}
int CMatchEngineUnit::Add(const std::string& key, const std::string& value){

    IKey* key_ptr = CKeyFactory::GetInstance()->GenKeyInstance(m_key_type);
    IValue* value_ptr = CValueFactory::GetInstance()->GenValueInstance(m_value_type);

    if(key_ptr->SetKey(&key) || value_ptr->SetVal(&value)) {
        return 1;
    }

    if(NULL == m_dict || m_dict->Add(*key_ptr, *value_ptr)) {
        return 2;
    }

    return 0;
}

int CMatchEngineUnit::Del(const std::string& key){
    IKey* key_ptr = CKeyFactory::GetInstance()->GenKeyInstance(m_key_type);
    if(key_ptr->SetKey(&key)) {
        return 1;
    }

    if(NULL == m_dict || m_dict->Del(*key_ptr)) {
        return 2;
    }
    return 0;
}

/*
 *
 * ret < 0 : error
 * ret = 0 : miss
 * ret = 1 : hit blacklist
 * ret = 2 : hit whitelist
 *
 * */
int CMatchEngineUnit::Get(const std::string& key, std::vector<std::string>* value){

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit::Get m_key_type=%s, key=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_key_type.c_str(),
                key.c_str());

    if(NULL == value || 0 == m_key_type.size()) {

        log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit::Get value or m_key_type is null",
                __FILE__,
                __LINE__,
                pthread_self());

                return -1;
    }

    IKey* key_ptr = CKeyFactory::GetInstance()->GenKeyInstance(m_key_type);
    if(NULL == key_ptr) {
        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit::Get gen key_ptr null and key=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                key.c_str());

        return -2;
    }

    if(key_ptr->SetKey(&key)) {
        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit::Get init key_ptr fail and key=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                key.c_str());
        return -3;
    }

    std::vector<IValue*> hit_value_vec;
    if(NULL == m_dict || m_dict->Get(*key_ptr, &hit_value_vec)) {
        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit::Get key and key=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                key.c_str());
        return -4;
    }

    FOR_EACH(hit_value_vec_itr, hit_value_vec) {
        std::string value_str;
        if((*hit_value_vec_itr)->GetVal(&value_str)) {
            continue;
        }
        std::string ret = "{\"engine\":\"" + m_engine + "\""
            + ",\"value\":\"" + value_str + "\""
            + ",\"blacklist\":";

        m_blacklist ? ret += "\"true\"}" : ret += "\"false\"}";

        value->push_back(ret);

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit::Get hit value=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                ret.c_str());
    }

    if(0 == value->size()) {
        return 0;
    }

    if(m_blacklist) {
        return 1;
    }

    return 2;
}

int CMatchEngineUnit::readConfig() {

    // 1. load the file
    INI* ini_reader = ini_init(m_config.c_str());
    if(NULL == ini_reader) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineUnit\tfunc:Init\tinfo:open m_config failed",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    char* read_iterm;

    if(NULL != (read_iterm = ini_read(ini_reader, m_engine.c_str(), "dict_type"))) {
        m_dict_type = read_iterm;

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReadConfig %s's dict_type is %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str(),
                read_iterm);

    }
    else {

        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:engine %s has no dict_type",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str());
        return 1;
    }

    if(NULL != (read_iterm = ini_read(ini_reader, m_engine.c_str(), "key_type"))) {

        m_key_type = read_iterm;

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReadConfig %s's key_type is %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str(),
                read_iterm);

    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:engine %s has no key_type",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str());
        return 2;
    }

    if(NULL != (read_iterm = ini_read(ini_reader, m_engine.c_str(), "value_type"))) {
        m_value_type = read_iterm;

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReadConfig %s's value_type is %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str(),
                read_iterm);

    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:engine %s has no value_type",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str());
        return 3;
    }

    if(NULL != (read_iterm = ini_read(ini_reader, m_engine.c_str(), "load_path"))) {
        m_load_path = read_iterm;

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReadConfig %s's load_path is %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str(),
                read_iterm);


    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:engine %s has no load_path",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str());
        return 4;
    }

    if(NULL != (read_iterm = ini_read(ini_reader, m_engine.c_str(), "dump_path"))) {
        m_dump_path = read_iterm;

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReadConfig %s's dump_path is %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str(),
                read_iterm);

    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:engine %s has no dump_path",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str());
    }

    if(NULL != (read_iterm = ini_read(ini_reader, m_engine.c_str(), "blacklist"))) {
        if(strcmp("1", read_iterm) == 0) {
            m_blacklist = true;
        }
        else {
            m_blacklist = false;
        }

        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineUnit::ReadConfig %s's blacklist is %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str(),
                read_iterm);
    }
    else {
        // m_blacklist default is true;
        m_blacklist = true;
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:engine %s has no blacklist",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_engine.c_str());
    }

    return 0; 
}
