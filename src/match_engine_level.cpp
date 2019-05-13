#include <string>
#include "match_engine_level.h"
#include "util.h"
#include "log.h"
#include "ini.h"

CMatchEngineLevel::CMatchEngineLevel(){

}

CMatchEngineLevel::CMatchEngineLevel(const std::string& config) : m_config(config) {
    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:Construct\tinfo:m_config is %s",
            __FILE__,
            __LINE__,
            pthread_self(),
            m_config.c_str());
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
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:Init\tinfo:open m_config[%s] failed",
                __FILE__,
                __LINE__,
                pthread_self(),
                m_config.c_str());
        return 1;
    }

    // 2. read Level::levels
    std::string levels;
    char* read_iterm;

    if(NULL != (read_iterm = ini_read(ini_reader, "Level", "levels"))) {
        levels = read_iterm;
        log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:Level=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                read_iterm);
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
            log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:%s=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                level_vec_itr->c_str(),
                read_iterm);
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
    FOR_EACH(level_pool_itr, m_level_pool) {
        std::vector<std::string>& engine_vec = level_pool_itr->second;
        FOR_EACH(engine_vec_itr, engine_vec) {

            std::string engine = *engine_vec_itr;

            log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:Load\tinfo:begin to load %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str());

            CMatchEngineUnit* match_engine_unit_ptr = new CMatchEngineUnit(m_config, engine);
            if(match_engine_unit_ptr->Init()
                    || match_engine_unit_ptr->Load()) {
                log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:load engine %s failed",
                        __FILE__,
                        __LINE__,
                        pthread_self(),
                        engine.c_str());
                continue;
            }
            m_engine_pool[engine] = match_engine_unit_ptr;
        }
    }
    return 0;
}

int CMatchEngineLevel::Reload() {
    // reload all
    FOR_EACH(level_pool_itr, m_level_pool) {
        std::vector<std::string>& engine_vec = level_pool_itr->second;
        FOR_EACH(engine_vec_itr, engine_vec) {

            std::string engine = *engine_vec_itr;
            Reload(engine);
        }
    }
    return 0;
}

int CMatchEngineLevel::Reload(const std::string& engine){

    CMatchEngineUnit* match_engine_unit_ptr = m_engine_pool[engine];
    if(match_engine_unit_ptr->Reload()) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:reload engine %s failed",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str());

        return 1;
    }
    return 0;
}

int CMatchEngineLevel::Dump() {

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCMatchEngineLevel::Dump begin",
            __FILE__,
            __LINE__,
            pthread_self());


    FOR_EACH(level_pool_itr, m_level_pool) {
        std::vector<std::string>& engine_vec = level_pool_itr->second;
        FOR_EACH(engine_vec_itr, engine_vec) {

            std::string engine = *engine_vec_itr;
            Dump(engine);
       }
    }
    return 0;
}

int CMatchEngineLevel::Dump(const std::string& engine){

    CMatchEngineUnit* match_engine_unit_ptr = m_engine_pool[engine];
    if(match_engine_unit_ptr->Dump()) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tCMatchEngineLevel::Dump dump engine %s failed",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str());

        return 1;
    }
    return 0;
}

int CMatchEngineLevel::Set(const std::string& engine, const std::string& key, const std::string& value){
    CMatchEngineUnit* match_engine_unit_ptr = m_engine_pool[engine];
    if(match_engine_unit_ptr->Set(key, value)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:set engine %s failed",
                __FILE__,
                __LINE__,
                engine.c_str());

        return 1;
    }
    return 0;

}

int CMatchEngineLevel::Add(const std::string& engine, const std::string& key, const std::string& value){

    CMatchEngineUnit* match_engine_unit_ptr = m_engine_pool[engine];
    if(match_engine_unit_ptr->Add(key, value)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:Add engine %s failed",
                __FILE__,
                __LINE__,
                engine.c_str());

        return 1;
    }
    return 0;
}

int CMatchEngineLevel::Del(const std::string& engine, const std::string& key){
    CMatchEngineUnit* match_engine_unit_ptr = m_engine_pool[engine];

    if(match_engine_unit_ptr->Del(key)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\t\tclass:CMatchEngineLevel\tfunc:ReadConfig\tinfo:Add engine %s failed",
                __FILE__,
                __LINE__,
                engine.c_str());
        return 1;
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
int CMatchEngineLevel::GetEngine(const std::string& engine, const std::string& key, std::vector<std::string>* value){
    CMatchEngineUnit* match_engine_unit_ptr = m_engine_pool[engine];

    return match_engine_unit_ptr->Get(key, value);
}
/*
 *
 * ret < 0 : error
 * ret = 0 : miss
 * ret = 1 : hit blacklist
 * ret = 2 : hit whitelist
 *
 * */
int CMatchEngineLevel::GetLevel(const std::string& level, const std::string& key, std::vector<std::string>* value){

    int rc = 0;
    std::vector<std::string>& engine_vec = m_level_pool[level];
    FOR_EACH(engine_vec_itr, engine_vec) {
        int rc_engine = GetEngine(*engine_vec_itr, key, value);
        if(0 >= rc) rc = rc_engine;
    }
    return rc;
}

/*
 *
 * ret < 0 : error
 * ret = 0 : miss
 * ret = 1 : hit blacklist
 * ret = 2 : hit whitelist
 *
 * */

int CMatchEngineLevel::GetSpec(const std::string& spec, const std::string& key, std::vector<std::string>* value){
    int rc = 0;
    std::vector<std::string> spec_vec = StringToTokens(spec, false, ';');
    FOR_EACH(spec_itr, spec_vec) {
        int rc_engine = GetEngine(*spec_itr, key, value);
        if(0 >= rc) rc = rc_engine;
    }
    return rc;
}
