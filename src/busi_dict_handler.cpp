#include "busi_dict_handler.h"
#include "log.h"
#include "ini.h"

CBusiDictHandler::CBusiDictHandler() : m_config_path("") {}

CBusiDictHandler::CBusiDictHandler(const std::string& config) : m_config_path(config) {}

CBusiDictHandler::~CBusiDictHandler() {
    FOR_EACH(dict_repo_itr, m_dict_repo) {
        CBusiDictLevel* dict_ptr = dict_repo_itr->second;
        if(NULL != dict_ptr) {
            delete dict_ptr;
        }
    }
}

int CBusiDictHandler::SetConfig(const std::string& config) {

    m_config_path = config;

}


int CBusiDictHandler::Load() {
    // TODO: read config
    if(0 == m_config_path.size()) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Load\tinfo:config file is null", __FILE__, __LINE__, pthread_self());
        return 1;
    }

    INI* ini_reader = ini_init(m_config_path.c_str());
    char* read_iterm;

    // read level 
    if(NULL != (read_iterm = ini_read(ini_reader, "Level", "levels"))) {
        m_levels = read_iterm; 
    }
    else {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Load\tinfo:Level has no levels", __FILE__, __LINE__, pthread_self());
        return 2;
    }

    // new BusiDictLevel
    std::vector<std::string> level_parts = StringToToken(m_levels, false, ';');
    FOR_EACH(level_parts_itr, level_parts) {

        if(m_dict_level_repo.count(*level_parts_itr)) {
            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:Load\tinfo:Level %s is exist", __FILE__, __LINE__, pthread_self(), level_parts_itr->c_str());
            continue;
        }

        CBusiDictLevel* bdl = new CBusiDictLevel(*level_parts_itr, m_config_path);
        if(NULL == bdl || bdl->Load()) {
            continue;
        }

        m_dict_level_repo[ *level_parts_itr ] = bdl;
        log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\tfunc:Load\tinfo:add Level %s, addr:%p", __FILE__, __LINE__, pthread_self(), level_parts_itr->c_str(), bdl);
    }

}

int CBusiDictHandler::ReloadLevel(const std::string& level) {
    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadLevel\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->Reload()) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadLevel\tinfo:Level %s reload all error", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 2; 
    }
    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadLevel\tinfo:Level %s reload all success", __FILE__, __LINE__, pthread_self(), level.c_str());
    return 0;
}

int CBusiDictHandler::ReloadDict(const std::string& level, const std::string& dict_unit) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->ReloadDict(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:Level %s reload dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2; 
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:Level %s reload dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}

int CBusiDictHandler::DumpLevel(const std::string& level) {
    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:DumpLevel\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(dbl->DumpLevel()) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:Level %s dump all error", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 2;
    }

    return 0;
}

int CBusiDictHandler::DumpDict(const std::string& level, const std::string& dict_unit) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:DumpDict\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->DumpDict(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DumpDict\tinfo:Level %s dump dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2; 
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:DumpDict\tinfo:Level %s dump dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}

int CBusiDictHandler::AddDict(const std::string& level, const std::string& dict_unit, const std::string& dict_path, int dict_type, bool is_blacklist) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:AddDict\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->AddDict(dict_unit, dict_path, dict_type, is_blacklist)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:AddDict\tinfo:Level %s add dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:AddDict\tinfo:Level %s add dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}

int CBusiDictHandler::AddKey(const std::string& level, const std::string& dict_unit, const std::string& key, const std::string& value) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:AddKey\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->AddDict(dict_unit, dict_path, dict_type, is_blacklist)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:AddKey\tinfo:Level %s add dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:AddKey\tinfo:Level %s add dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}

int CBusiDictHandler::DelDict(const std::string& level, const std::string& dict_unit) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:DelDict\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->DelDict(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DelDict\tinfo:Level %s del dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:DelDict\tinfo:Level %s del dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;

}

int CBusiDictHandler::DelKey(const std::string& level, const std::string& dict_unit, const std::string& key) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:DelKey\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->DelKey(dict_unit, key)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DelKey\tinfo:Level %s del dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:DelKey\tinfo:Level %s del dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}

int CBusiDictHandler::Match(const std::string& level, const std::string& dict_unit, const std::string& key, std::vector<std::string>* value) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:Match\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->Match(dict_unit, key, value)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Match\tinfo:Level %s del dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:Match\tinfo:Level %s del dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}

int CBusiDictHandler::MatchOrder(const std::string& level, const std::string& key, std::vector<std::string>* value) {

    if(0 == m_dict_level_repo.count(level)) {
        log (log_warning, "file:%s\tline:%d\ttid:%lld\tfunc:matchorder\tinfo:level %s not exist", __file__, __line__, pthread_self(), level.c_str());
        return 1;
    }

    cbusidictlevel* bdl = m_dict_level_repo[level];

    int hit = 0;
    std::string hit_dict_unit;
    if(bdl->MatchOrder(dict_unit, key, &hit, &hit_dict_unit, value)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:MatchOrder\tinfo:Level %s del dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:MatchOrder\tinfo:Level %s del dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}
int CBusiDictHandler::Info(const std::string& level, std::map<std::string, std::string>* infos) {

    if(0 == m_dict_level_repo.count(level)) {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:Info\tinfo:Level %s not exist", __FILE__, __LINE__, pthread_self(), level.c_str());
        return 1;
    }

    CBusiDictLevel* bdl = m_dict_level_repo[level];

    if(bdl->Info(infos)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Info\tinfo:Level %s del dict %s error", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());
        return 2;
    }

    log (LOG_NOTICE, "file:%s\tline:%d\ttid:%lld\tfunc:Info\tinfo:Level %s del dict %s success", __FILE__, __LINE__, pthread_self(), level.c_str(), dict_unit.c_str());

    return 0;
}
