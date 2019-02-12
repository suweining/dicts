#include "match_engine_unit.h"
#include "log.h"
#include "util.h"
#include "ini.h"

CMatchEngineUnit::CMatchEngineUnit() {

}

CMatchEngineUnit::CMatchEngineUnit(const std::string& config, const std::string& engine) : m_config(config), m_dict_type(engine) {

}

CMatchEngineUnit::~CMatchEngineUnit() {

}

int CMatchEngineUnit::SetConfig(const std::string& config){
    m_config = config;
    return 0;
}

int CMatchEngineUnit::SetEngine(const std::string& engine){
    m_dict_type = engine;
    return 0;
}

int CMatchEngineUnit::Init() {
    // 1. check m_dict_type and m_config
    if(0 == m_dict_type.size() || 0 == m_config.size()) {

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
//    Reload(m_load_path);
}

int CMatchEngineUnit::Reload(){
//    Reload(m_load_path);
}

int CMatchEngineUnit::Reload(const std::string& load_path){
    // 1. read load_path
    // 2. init key
    IKey* key_ptr = CKeyFactory::GetInstance()->GenKeyInstance(m_key_type);
    // 3. init value
    IValue* value_ptr = CValueFactory::GetInstance()->GenValueInstance(m_value_type);
    // 4. Finalize
}

int CMatchEngineUnit::Dump() {
    return 0;
}
int CMatchEngineUnit::Dump(const std::string& dump_path) {
    return 0;
}


int CMatchEngineUnit::AddKey(const std::string& key, const std::string& value){
//    1. check
//    2. init key
//    3. init value
//    4. Add

}

int CMatchEngineUnit::DelKey(const std::string& key){
    // 1. check
    // 2. init key
    // 3. Del
}
int CMatchEngineUnit::Match(const std::string& key, std::vector<std::string>* value){
    // 1. check
    // 2. init key
    // 3. Get
}

int CMatchEngineUnit::readConfig() {

    // 1. load the file
    INI* ini_reader = ini_init(m_config.c_str());
    if(NULL == ini_reader) {
        return 1;
    }
    char* read_iterm;


    return 0; 
}
