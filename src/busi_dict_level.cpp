#include <sys/time.h>

#include "busi_dict_handler.h"
#include "util.h"
#include "log.h"
#include "ini.h"

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

    if(ReadConfig()) {
        return 1; 
    }

    FOR_EACH(dict_path_itr, m_dict_path) {

        std::string dict_unit = dict_path_itr->first;

        m_dict_repo_online[dict_unit] = NULL;

        if(ReloadDict(dict_unit)) {
            log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Reload\tinfo:reload %s fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
            continue; 
        }
    }
    returnn 0;
}

int CBusiDictLevel::ReadConfig() {

    // TODO: check config
    if(0 == m_config_path.size()) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:config file is null", __FILE__, __LINE__, pthread_self());
        return 1;
    }

    // TODO: load the file
    INI* ini_reader = ini_init(m_config_path.c_str());
    char* read_iterm;

    // read dump_path
    if(NULL != (read_iterm = ini_read(ini_reader, m_level.c_str(), "dump_path"))) {
        m_dump_path = read_iterm; 
    }
    else {
        m_dump_path = "dump";
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level %s has no dump_path", __FILE__, __LINE__, pthread_self(), m_level.c_str());
    }

    // read blacklist_dict
    std::string blacklist;
    if(NULL != (read_iterm = ini_read(ini_reader, m_level.c_str(), "blacklist"))) {
        blacklist = read_iterm; 
    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level %s has no blacklist", __FILE__, __LINE__, pthread_self(), m_level.c_str());
    }
    std::vector<std::string> blacklist_parts = StringToToken(blacklist, false, ';');
    FOR_EACH(blacklist_parts_itr, blacklist_parts) {
        m_blacklist_dict.insert(*blacklist_parts_itr);
    }

    // read whitelist
    std::string whitelist;
    if(NULL != (read_iterm = ini_read(ini_reader, m_level.c_str(), "whitelist"))) {
        whitelist = read_iterm; 
    }
    else {
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level %s has no whitelist", __FILE__, __LINE__, pthread_self(), m_level.c_str());
    }
    std::vector<std::string> whitelist_parts = StringToToken(whitelist, false, ';');
    FOR_EACH(whitelist_parts_itr, whitelist_parts) {
        m_whitelist_dict.insert(*whitelist_parts_itr);
    }

    // read dict_type
    std::string dict_type_str;
    if(NULL != (read_iterm = ini_read(ini_reader, m_level.c_str(), "dict_type"))) {
        dict_type_str = read_iterm; 
    }
    else {
        dict_type_str = "dict_kv;dict_pattern;dict_contain;dict_contain_multi;dict_contain_multi_seq";
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level %s has no dict_type", __FILE__, __LINE__, pthread_self(), m_level.c_str());
    }

    // read dict path info in config
    std::vector<std::string> dict_type_str_parts= StringToToken(dict_type_str, false, ';');
    FOR_EACH(dict_type_str_parts_itr, dict_type_str_parts) {

        // check type
        DICT_TYPE dt = GetDictType(*dict_type_itr);
        if(0 == dt) {
            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level %s's %s is illegality", __FILE__, __LINE__, pthread_self(), m_level.c_str(), dict_type_itr->c_str());
            continue;
        }

        // read dict_path
        std::string dict_path_info;
        if(NULL != (read_iterm = ini_read(ini_reader, m_level.c_str(), dict_type_str_parts_itr->c_str()))) {
            dict_path_info = read_iterm; 
        }
        else {
            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level %s's %s is null", __FILE__, __LINE__, pthread_self(), m_level.c_str(), dict_type_itr->c_str());
            continue;
        }

        // m_dict_path; m_dict_type
        std::vector<std::string> dict_path_info_parts = StringToToken(dict_path_info, false, ';');
        FOR_EACH(dict_path_info_parts_itr, dict_path_info_parts) {
            // check dict_path_info
            std::vector<std::string> dict_unit_path_parts = StringToToken(*dict_path_info_parts_itr, false, ':');
            if(2 != dict_unit_path_parts.size()) {
                log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level:%s dict_type:%s has illegality info:%s", __FILE__, __LINE__, pthread_self(), m_level.c_str(), dict_type_itr->c_str(), dict_path_info_parts_itr->c_str());
                continue;
            }

            if(0 != m_dict_path.count(dict_unit_path_parts[0])) {
                log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReadConfig\tinfo:level:%s dict_type:%s has repetitive dict_info:%s", __FILE__, __LINE__, pthread_self(), m_level.c_str(), dict_type_itr->c_str(), dict_path_info_parts_itr->c_str());
                return 3;
            }

            m_dict_path[ dict_unit_path_parts[0] ] = dict_unit_path_parts[1];
            m_dict_type[ dict_unit_path_parts[0] ] = dt;
        }


    }


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

    IDict* new_dict = NULL; 

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

    if(NULL == new_dict) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:%s type not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 3;
    }

    if(new_dict->Load(dict_unit_path)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tinfo:%s load fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 4; 
    }    
    
    // add new_dict to m_dict_repo_online
    IDict* old_dict = m_dict_repo_online[dict_unit];
    m_dict_repo_online[dict_unit] = new_dict;

    // deal old_dict
    if(NULL == old_dict) {
        return 0; 
    }

    log (LOG_INFO, "file:%s\tline:%d\ttid:%lld\tfunc:ReloadDict\tnew_dict:%p,old_dict:%p", __FILE__, __LINE__, pthread_self(), new_dict, old_dict);

   double cur_timestamp = GetCurrentTimestamp();

    while(0 != m_dict_repo_offline.count(cur_timestamp)) {
        sleep(1); 
        cur_timestamp = GetCurrentTimestamp();
    }

    m_dict_repo_offline[cur_timestamp] = old_dict;


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
        cur_timestamp = GetCurrentTimestamp();
    }

    m_dict_repo_offline[cur_timestamp] = m_dict_repo_online[dict_unit];

    m_dict_repo_online.erase(dict_unit);

    return 0;
}
int CBusiDictLevel::DelKey(const std::string& dict_unit, const std::string& key) {
    if(0 == m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DelKey\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 1; 
    }

    IDict* d = m_dict_repo_online[dict_unit];

    IKey *k = NULL;
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
    
    if(NULL == k) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DelKey\tinfo:gen key fail", __FILE__, __LINE__, pthread_self());
        return 2;
    }

    if(d->Del(*k)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:DelKey\tinfo:%s Del fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 3; 
    }
    return 0;
}

int CBusiDictLevel::Match(const std::string& dict_unit, const std::string& key, std::vector<std::string>* values) {
    
    if(NULL == values) {
        return 1; 
    }

    if(0 == m_dict_repo_online.count(dict_unit)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Match\tinfo:%s not exist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 2; 
    }

    IDict* d = m_dict_repo_online[dict_unit];

    IKey *k = NULL;
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
    
    if(NULL == k) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Match\tinfo:gen key fail", __FILE__, __LINE__, pthread_self());
        return 3;
    }

    std::vector<IValue*> vals;

    if(d->Get(*k, &vals)) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tfunc:Match\tinfo:%s match fail", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
        return 4; 
    }
   
    FOR_EACH(val_itr, vals) {
        std::string v;
        val_itr->ToString(&v);
        values->push_back(v);
    }

    FOR_EACH(val_itr, vals) {
        delete *val_itr; 
    }
    return 0;
}

int CBusiDictLevel::MatchOrder(const std::string& key, int* hit, std::string* hit_dict_unit, std::vector<std::string>* values) {

    if(0 == order.size()) {
        *hit = 1;
        log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:MatchOrder\tinfo:order's size is 0", __FILE__, __LINE__, pthread_self());
        return 0;
    }

    std::vector<std::string>    vals;
    std::string                 dict_unit;
    FOR_EACH(order_itr, m_order) {
        if(Match(*order_itr, key, &vals)) {
            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:MatchOrder\tinfo:match failed", __FILE__, __LINE__, pthread_self());
            continue;
        } 
        if(0 != vals.size()) {
            dict_unit = *order_itr;
            break; 
        }
    }
    if(0 != vals.size()) {
        // check hit the blacklist or whitelist
        if(m_blacklist_dict.count(dict_unit)) {
            *hit = 2; 
        }
        else if(m_whitelist_dict.count(dict_unit)) {
            *hit = 3; 
        }
        else {
            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tfunc:MatchOrder\tinfo:hit dict:%s not blacklist/whitelist", __FILE__, __LINE__, pthread_self(), dict_unit.c_str());
            *hit = 4; 
        }
    }

    if(NULL != hit_dict_unit) {
        hit_dict_unit->swap(dict_unit); 
    }

    if(NULL != value) {
        value->swap(vals); 
    }

    return 0;
}

int CBusiDictLevel::Info(std::map<std::string, std::string>* infos) {
    if(NULL == infos) {
        return 1; 
    }

    FOR_EACH(dict_repo_itr, m_dict_repo_online) {
        std::string dict_unit = dict_repo_itr->first;
        std::string dict_path = m_dict_path[dict_unit];
        std::string dict_kind = (0 != m_blacklist_dict.count(dict_unit) ? "blacklist" : "whitelist");
        DICT_TYPE   dict_type = m_dict_type[dict_unit];
        std::string dict_info;
        
        m_dict_repo_online[dict_unit]->Info(&dict_info);
        
        std::ostringstream oss;
        oss << "dict_name:" << dict_unit << "\t"
            << "dict_path:" << dict_path << "\t"
            << "dict_kind:" << dict_kind << "\t"
            << "dict_type:" << dict_type << "\t";
            << "dict_info:" << dict_info << "\n";

        (*infos)[dict_unit] = oss.c_str());
    }

    std::string order;
    FOR_EACH(order_itr, m_order) {
        order += *order_itr + "\t"; 
    }
    (*infos)["match_order"] = order;

    return 0;
}
const double CBusiDictLevel::GetCurrentTimestamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec * 1.0 / 1000;
}

CBusiDictLevel::DICT_TYPE CBusiDictLevel::GetDictType(const std::string& dict_type_str) {
    DICT_TYPE dt = 0;
    static string[] dict_type_vec = {"dict_key", "dict_pattern", "dict_contain", "dict_contain_multi", "dict_contain_multi_seq"};

    size_t len = sizeof(dict_type_vec) / sizeof(&dict_type_vec[0]);

    for(size_t i = 0; i < len; ++i) {
        if(dict_type_str == dict_type_vec) {
            dt = 1 + i;
            break;
        }
    }

    return dt;
}
