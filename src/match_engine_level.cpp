#include "match_engine_level.h"
#include "util.h"
#include "log.h"

CMatchEngineLevel::CMatchEngineLevel(){

}

CMatchEngineLevel::CMatchEngineLevel(const std::string& config) : m_config(config) {

}

CMatchEngineLevel::~CMatchEngineLevel(){
    FOR_EACH(engine_pool_itr, m_engine_pool) {
        if(NULL != engine_pool_itr->second) {
            delete engine_pool_itr->second;
        }
    }
}

int CMatchEngineLevel::SetConfig(const std::string& config){
    m_config = config;
    return 0;
}

int CMatchEngineLevel::Init(){
    // read config: m_level_pool
    if(0 == m_config.size()) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:Init\tinfo:m_config is null",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }
    // 1. open config file
    INI* ini_reader = ini_init(m_config.c_str());
    if(NULL == ini_reader) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:Init\tinfo:open m_config failed",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    // 2. read Level::levels
    std::string levels;
    char* read_iterm;

    if(NULL != (read_iterm = ini_read(ini_reader, "Level", "levels"))) {
        levels = read_iterm;
    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:Level has no levels",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // 3. read Level::$level
    std::vector<std::string> level_vec = StringToTokens(levels, false, ';');
    FOR_EACH(level_vec_itr, level_vec) {

        std::string engines;
        if(NULL != (read_iterm = ini_read(ini_reader, "Level", level_vec_itr->c_str()))) {
            engines = read_iterm;
        }
        else {
            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:Level's %s has no engines",
                    __FILE__,
                    __LINE__,
                    level_vec_itr->c_str());
        }

        std::vector<std::string> engine_vec;
        engine_vec = StringToTokens(engines, false, ';');

        // 4. gen m_level_pool
        m_level_pool[level_vec_itr->c_str()] = engine_vec;
    }

    return 0;
}

int CMatchEngineLevel::Load(){
    // read dict: m_engine_pool
    return 0;
}

int CMatchEngineLevel::Reload() {
    // reload all

}

int CMatchEngineLevel::Reload(const std::string& engine){

}

int CMatchEngineLevel::Dump() {

}

int CMatchEngineLevel::Dump(const std::string& engine){

}

int CMatchEngineLevel::Set(const std::string& engine, const std::string& key, const std::string& value){

}

int CMatchEngineLevel::Add(const std::string& engine, const std::string& key, const std::string& value){

}

int CMatchEngineLevel::Del(const std::string& engine, const std::string& key){

}

int CMatchEngineLevel::GetEngine(const std::string& engine, const std::string& key, std::vector<std::string>* value){

}

int CMatchEngineLevel::GetLevel(const std::string& level, const std::string& key, std::vector<std::string>* value){

}

int CMatchEngineLevel::GetSpec(const std::string& spec, const std::string& key, std::vector<std::string>* value){

}


