#include "match_engine_singleton.h"
#include "log.h"

CMatchEngineSingleton::CMatchEngineSingleton() : m_match_engine_level(NULL) {
    m_match_engine_level = new CMatchEngineLevel(); 
}

CMatchEngineSingleton::CMatchEngineSingleton(const std::string& config) : m_match_engine_level(NULL) {

    m_match_engine_level = new CMatchEngineLevel(config);
}

CMatchEngineSingleton::~CMatchEngineSingleton() {
    if(NULL != m_match_engine_level) {
        delete m_match_engine_level;
        m_match_engine_level = NULL;
    }
}

int CMatchEngineSingleton::SetConfig(const std::string& config){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::SetConfig m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->SetConfig(config);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::SetConfig m_match_engine_level->SetConfig error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return rc; 
    }

    rc = m_match_engine_level->Init();
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Load m_match_engine_level->Init error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return rc; 
    }

    return 0;
}

int CMatchEngineSingleton::Load(){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Load m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Load();
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Load m_match_engine_level->Load error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return rc; 
    }

    return 0;
}

int CMatchEngineSingleton::Reload() { // reload all
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Reload m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Reload();
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Reload m_match_engine_level->Reload error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return rc; 
    }

    return 0;    
}

int CMatchEngineSingleton::Reload(const std::string& engine){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Reload m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Reload(engine);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Reload m_match_engine_level->Reload error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return rc; 
    }

    return 0; 
}
int CMatchEngineSingleton::Dump() { // dump all
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Dump m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Dump();
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Dump m_match_engine_level->Dump error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return rc; 
    }

    return 0; 
} 

int CMatchEngineSingleton::Dump(const std::string& engine){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Dump m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Dump(engine);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Dump m_match_engine_level->Dump(%s) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str(),
                rc);
        return rc; 
    }

    return 0;
}

int CMatchEngineSingleton::Set(const std::string& engine, const std::string& key, const std::string& value){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Set m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Set(engine, key, value);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Set m_match_engine_level->Set(%s, %s, %s) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str(),
                key.c_str(),
                value.c_str(),
                rc);
        return rc; 
    }

    return 0;
}

int CMatchEngineSingleton::Add(const std::string& engine, const std::string& key, const std::string& value){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Add m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Add(engine, key, value);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Add m_match_engine_level->Add(%s, %s, %s) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str(),
                key.c_str(),
                value.c_str(),
                rc);
        return rc; 
    }
    log(LOG_DEBUG, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Add m_match_engine_level->Add(%s, %s, %s) success",
            __FILE__,
            __LINE__,
            pthread_self(),
            engine.c_str(),
            key.c_str(),
            value.c_str());

    return 0;

}

int CMatchEngineSingleton::Del(const std::string& engine, const std::string& key){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Del m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->Del(engine, key);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Del m_match_engine_level->Del(%s, %s) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str(),
                key.c_str(),
                rc);
        return rc; 
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tCMatchEngineSingleton::Del m_match_engine_level->Del(%s, %s, %s) success",
            __FILE__,
            __LINE__,
            pthread_self(),
            engine.c_str(),
            key.c_str());

    return 0;
}

int CMatchEngineSingleton::GetEngine(const std::string& engine, const std::string& key, std::vector<std::string>* value){

    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->GetEngine(engine, key, value);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level->GetEngine(%s, %s, %d) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                engine.c_str(),
                key.c_str(),
                value->size(),
                rc);
        return rc;
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level->GetEngine(%s, %s, %d) success",
            __FILE__,
            __LINE__,
            pthread_self(),
            engine.c_str(),
            key.c_str(),
            value->size());

    return 0;


}
int CMatchEngineSingleton::GetLevel(const std::string& level, const std::string& key, std::vector<std::string>* value){

    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->GetLevel(level, key, value);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level->GetLevel(%s, %s, %d) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                level.c_str(),
                key.c_str(),
                value->size(),
                rc);
        return rc; 
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level->GetLevel(%s, %s, %d) success",
            __FILE__,
            __LINE__,
            pthread_self(),
            level.c_str(),
            key.c_str(),
            value->size());

    return 0;
}

int CMatchEngineSingleton::GetSpec(const std::string& spec, const std::string& key, std::vector<std::string>* value){
    if(NULL == m_match_engine_level) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level is NULL",
                __FILE__,
                __LINE__,
                pthread_self());
        return 0;
    }

    int rc = m_match_engine_level->GetSpec(spec, key, value);
    if(0 != rc) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetEngine m_match_engine_level->GetSpec(%s, %s, %d) error=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                spec.c_str(),
                key.c_str(),
                value->size(),
                rc);
        return rc; 
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tCMatchEngineSingleton::GetSpec m_match_engine_level->GetSpec(%s, %s, %d) success",
            __FILE__,
            __LINE__,
            pthread_self(),
            spec.c_str(),
            key.c_str(),
            value->size());

    return 0;

}


