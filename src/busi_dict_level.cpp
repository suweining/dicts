#include <sys/time.h>

#include "busi_dict_handler.h"
#include "util.h"
#include "log.h"

CBusiDictLevel::CBusiDictLevel() : m_level(""), m_config_path(""), m_dump_path("dump") {}
CBusiDictLevel::CBusiDictLevel(const std::string& level) : m_level(level), m_config_path(""), m_dump_path("dump") {}

CBusiDictLevel::CBusiDictLevel(const std::string& level, const std::string& config) : m_level(level), m_config_path(config), m_dump_path("dump") {}

CBusiDictLevel::~CBusiDictLevel() {
    FOR_EACH(dict_repo_itr, m_dict_repo_online) {
        IDict* dict_ptr = dict_repo_itr->second;
        if(NULL != dict_ptr) {
            delete dict_ptr; 
        }
    }
}

int CBusiDictLevel::SetConfig(const std::string& config) {
    m_config_path = config;
    return 0;
}

int CBusiDictLevel::SetLevel(const std::string& level) {
    m_level = level;
    return 0;
}

int CBusiDictLevel::Load() {

    // TODO: read config 
    // TODO: load the file

} 

int CBusiDictLevel::Reload() {

    FOR_EACH(dict_path_itr, m_dict_path) {

        std::string dict_unit = dict_path_itr->first;

        if(ReloadDict(dict_unit)) {
            log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Reload\tinfo:reload %s fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
            return 1; 
        }

    }
    return 0;
}

int CBusiDictLevel::ReloadDict(const std::string& dict_unit) {

    if(0 == m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    std::string dict_unit_path = m_dict_path[dict_unit];
    if(NULL == dict_unit_path || 0 == dict_unit_path.size()) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:%s path is null", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 2;
    }

    IDict* d = NULL; 

    switch(m_dict_type[dict_unit]) {
        case DICT_KV:
            break;
        case DICT_PATTERN:
            break;
        case DICT_CONTAIN:
            break;
        case DICT_CONTAIN_MULTI:
            break;
        case DICT_CONTAIN_MULTI_SEQ:
            break;
        default:

    }

    if(NULL == d) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:%s type not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 3;
    }

    if(d->Load(dict_unit_path)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:%s load fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 4; 
    }
    // TODO: add d to m_dict_repo_online
    //
    return 0;
}

int CBusiDictLevel::DumpLevel() {
    FOR_EACH(dict_repo_itr, m_dict_repo_online) {
        std::string dict_unit = dict_repo_itr->first;
        if(DumpDict(dict_unit)) {
            log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DumpLevel\tinfo:dump %s fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
            return 1;
        }
    } 
    return 0;
}

int CBusiDictLevel::DumpDict(const std::string& dict_unit) {

    if(0 == m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DumpDict\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    IDict* d = m_dict_repo_online[dict_unit];
    if(NULL == d) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DumpDict\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    std::string dump_path = m_dump_path + "/" + dict_unit + "." + m_level; 
    if(d->Dump(dump_path)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DumpDict\tinfo:%s dump into %s fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str(), dump_path.c_str());
        return 2;
    }

    return 0;
}

int CBusiDictLevel::AddDict(const std::string& dict_unit, const std::string& dict_path, const int dict_type, const bool is_blacklist) {
    if(0 != m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:AddDict\tinfo:%s exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    m_dict_path[dict_unit] = dict_path; 
    m_dict_type[dict_unit] = dict_type;

    if(is_blacklist) {
        m_blacklist_dict.insert(dict_unit); 
    }
    else{
        m_whitelist_dict.insert(dict_unit); 
    }
    return 0;
}

int CBusiDictLevel::AddKey(const std::string& dict_unit, const std::string& key, const std::string& value) {

    if(0 == m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:AddKey\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    IKey *k = NULL;
    IValue *v = NULL;

    switch(m_dict_type[dict_unit]) {
        case DICT_KV:
            break;
        case DICT_PATTERN:
            break;
        case DICT_CONTAIN:
            break;
        case DICT_CONTAIN_MULTI:
            break;
        case DICT_CONTAIN_MULTI_SEQ:
            break;
        default:

    }
    
    if(NULL == k || NULL == v) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:AddKey\tinfo: init key or value error", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 2; 
    }

    IDict *d = m_dict_repo_online[dict_unit];
    if(d->Set(*k, *v)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:AddKey\tinfo:%s Set error", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 3; 
    }

    return 0;
}

int CBusiDictLevel::DelDict(const std::string& dict_unit) {
    // TODO: need mutex
    if(0 == m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DelDict\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    double cur_timestamp = GetCurrentTimestamp();

    while(0 != m_dict_repo_offline.count(cur_timestamp)) {
        sleep(1); 
    }

    m_dict_repo_offline[cur_timestamp] = m_dict_repo_online[dict_unit];

    m_dict_repo_online.erase(dict_unit);
        
    return 0;
}

const double CBusiDictLevel::GetCurrentTimestamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec * 1.0 / 1000;
}
