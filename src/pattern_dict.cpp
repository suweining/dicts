#include <fstream>
#include <algorithm>
#include <set>

#include "pattern_dict.h"
#include "pattern_dict_key.h"
#include "pattern_dict_value.h"

#include "util.h"
#include "log.h"

CPatternDict::CPatternDict() {

    m_darts_datrie_read     = new DartsDatrie();
    m_darts_datrie_write    = new DartsDatrie();
}

CPatternDict::~CPatternDict() {
    if(NULL != m_darts_datrie_read) {
        delete m_darts_datrie_read;
        m_darts_datrie_read = NULL;
    }

    if(NULL != m_darts_datrie_write) {
        delete m_darts_datrie_write;
        m_darts_datrie_write = NULL;
    }
}

int CPatternDict::Set(const IKey& key, const IValue& value) {
    std::string re_string;
    int rc;
    if((rc = key.GetKey(&re_string)) && rc != 0) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\t\tclass:CPatternDict\tfunc:Set\tinfo:GetKey, rc:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 1;
    }
    // check the re is legal
    if((rc = ReLegal(re_string)) && rc != 0) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tclass:CPatternDict\tfunc:Set\tinfo:ReLegal error, rc:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 2;
    }

    // check in m_dict_info or not
    auto key_itr =  m_dict_info_write.end();
    FOR_EACH(itr, m_dict_info_write) {
        if(*itr == key) {
            key_itr = itr; 
            break;
        }
    }

    // erase key in m_dict_info
    if(m_dict_info_write.end() != key_itr) {
        m_dict_info_write.erase(key_itr);
    }
    m_dict_info_write.push_back(DictInfoMeta(const_cast<IKey*>(&key), const_cast<IValue*>(&value)));
    return 0;
}

int CPatternDict::Add(const IKey& key, const IValue& value) {
    std::string re_string;
    int rc;
    if((rc = key.GetKey(&re_string)) && rc != 0) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\t\tclass:CPatternDict\tfunc:Add\tinfo:GetKey,rc:%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return 1;
    }

    std::string value_str;
    if((rc = value.GetVal(&value_str)) && rc != 0) {
        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\t\tclass:CPatternDict\tfunc:Add\tinfo:GetVal,rc:%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return 1;
    }

    log (LOG_DEBUG, "file:%s\tline:%d\ttid:%lld\tCPatternDict::Add key=%s value=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            re_string.c_str(),
            value_str.c_str());

    // check the re is legal
    if((rc = ReLegal(re_string)) && rc != 0) {

        log (LOG_ERROR, "file:%s\tline:%d\ttid:%lld\tclass:CPatternDict\tfunc:Add\tinfo:ReLegal error, rc:%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return 2;
    }

    // check in m_dict_info or not
    auto key_itr = m_dict_info_write.end();
    FOR_EACH(itr, m_dict_info_write) {
        if(*itr == key) {
            key_itr = itr; 

            log (LOG_WARNING, "file:%s\tline:%d\ttid:%lld\tclass:CPatternDict\tfunc:Add\tinfo:record existed",
                    __FILE__,
                    __LINE__,
                    pthread_self());
            break;
        }
    }

    m_dict_info_write.push_back(DictInfoMeta(const_cast<IKey*>(&key), const_cast<IValue*>(&value)));

    return 0;
}

int CPatternDict::Del(const IKey& key) {
    // check in m_dict_info_write or not
    auto key_itr =  m_dict_info_write.end();
    FOR_EACH(itr, m_dict_info_write) {
        if(*itr == key) {
            key_itr = itr; 
        }
    }

    if(m_dict_info_write.end() != key_itr) {
        m_dict_info_write.erase(key_itr);
    }
    else{
        return 1; 
    }
    return 0;
}

int CPatternDict::Get(const IKey& key, std::vector<IValue*>* value) {
    std::vector<DartsDatrie::result_pair_type> results;
    size_t result_size = 0;
    std::string keyword;
    key.GetKey(&keyword);

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Get key[%s]",
            __FILE__,
            __LINE__,
            pthread_self(),
            keyword.c_str());

    // check trie and get m_prefix_info_read s' index
    result_size = m_darts_datrie_read->commonPrefixSearch(keyword.c_str(), results);
    if(result_size == 0) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Get\tinfo:miss darts",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Get key=%s, ret_size=%d, result_size=%d",
            __FILE__,
            __LINE__,
            pthread_self(),
            keyword.c_str(),
            result_size,
            results.size());

    if(result_size != results.size()) {
        log(LOG_INFO, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Get\tinfo:match size error",
                __FILE__,
                __LINE__,
                pthread_self());
        return 2;
    }

    // get hit_prefix_info by hit_index
    std::vector<PrefixInfoMeta> hit_prefix_info;
    for(size_t i = 0; i < result_size; ++i) {

        size_t index = static_cast<size_t>(results[i].value);

        hit_prefix_info.push_back(m_prefix_info_read[index]);

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Get\tinfo:hit m_prefix_info_read[%d]=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                index,
                (m_prefix_info_read[index].prefix).c_str());
    }

    std::vector<RegexInfoMeta> hit_regex_info;
    // check DFAs and get hit_regex_info from regex_info_online/offline
    FOR_EACH(prefix_info_itr, hit_prefix_info) {
        std::vector<int> match_regex_info_index; 
        prefix_info_itr->dfa->Match(keyword, &match_regex_info_index);

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Get prefix[%s]'s DFA ret match_regex_info_index.size=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                prefix_info_itr->prefix.c_str(),
                match_regex_info_index.size());

        for(size_t i = 0; i < match_regex_info_index.size(); ++i) {
            // bug
            hit_regex_info.push_back(prefix_info_itr->regex_info_online[i]);

            log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Get prefix[%s]'s DFA hit regex info:%s",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    prefix_info_itr->prefix.c_str(),
                    prefix_info_itr->regex_info_online[i].regex.c_str());

        }
        for(size_t i = 0; i < prefix_info_itr->regex_info_offline.size(); ++i) {
            if(RE2::FullMatch(keyword.c_str(), prefix_info_itr->regex_info_offline[i].regex.c_str())) {
                hit_regex_info.push_back(prefix_info_itr->regex_info_offline[i]);

                log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Get offline hit regex info:%s",
                        __FILE__,
                        __LINE__,
                        pthread_self(),
                        prefix_info_itr->regex_info_offline[i].regex.c_str());
            }
        }
    }
    // get hit_dict_info index from hit_prefix_info
    std::vector<DictInfoMeta> hit_dict_info;
    for(size_t i = 0; i < hit_regex_info.size(); ++i) {
        hit_dict_info.push_back( m_dict_info_read[ hit_regex_info[i].index ] );
    }

    // get values from hit_dict_info
    for(size_t i = 0; i < hit_dict_info.size(); ++i) {
        std::string str;
        hit_dict_info[i].key->ToString(&str);

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Get\tinfo:hit key info[%s]",
                __FILE__,
                __LINE__,
                pthread_self(),
                str.c_str());

        value->push_back(hit_dict_info[i].value);
    }

    return 0;
}

int CPatternDict::Dump(const std::string& dict_data_dump_path) {
    std::ofstream out_stream(dict_data_dump_path, std::ifstream::out);
    if(out_stream.is_open()) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Dump\tinfo:fail to dump %s", __FILE__, __LINE__, pthread_self(), dict_data_dump_path.c_str());
        return 1;
    }
    FOR_EACH(dict_info_itr, m_dict_info_read) {
        std::string key_str, value_str;
        dict_info_itr->key->ToString(&key_str);
        dict_info_itr->value->ToString(&key_str);

        out_stream << key_str << "\t" << value_str << std::endl;
    }
    return 0;
}

int CPatternDict::Finalize() {

    //prepare m_prefix_info_write by m_dict_info_write 
    size_t dict_info_len = m_dict_info_write.size();

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Finalize begin finalize, m_dict_info_write.size=%d", 
            __FILE__,
            __LINE__,
            pthread_self(), 
            dict_info_len);

    for(size_t i = 0; i < dict_info_len; ++i) {
        std::string re_string;
        m_dict_info_write[i].key->GetKey(&re_string);

        PrefixInfoMeta prefix_info;
        int rc = 0;
        if((rc = RePrefix(re_string, &(prefix_info.prefix))) && rc != 0) {
            log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Finalize\tinfo:fail to Finalize, rc:%s", 
                    __FILE__,
                    __LINE__,
                    pthread_self(), 
                    rc);
            continue;
        }

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Finalize\tinfo:prefix %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                prefix_info.prefix.c_str());

        RegexInfoMeta regex_info_meta;
        regex_info_meta.regex   = re_string;
        regex_info_meta.index   = i;

        typeof(m_prefix_info_write.begin()) prefix_info_itr = m_prefix_info_write.end();
        FOR_EACH(itr, m_prefix_info_write) {
            if(itr->prefix == regex_info_meta.regex) {
                prefix_info_itr= itr;
            }
        }
        if(m_prefix_info_write.end() == prefix_info_itr) {
            // not exist in m_prefix_info_write
            prefix_info.regex_info_repo.push_back(regex_info_meta);
            m_prefix_info_write.push_back(prefix_info);
        }
        else {
            // prefix_info has exist in m_prefix_info_write
            prefix_info_itr->regex_info_repo.push_back(regex_info_meta);
        }
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Finalize\tinfo:begin to build Trie",
            __FILE__,
            __LINE__,
            pthread_self());
    // build trie
    int rc = 0;
    if((rc = BuildTrie()) && rc != 0) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Finalize\tinfo:BuildTrie error rc=%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return 1;
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Finalize\tinfo:end to build Trie",
            __FILE__,
            __LINE__,
            pthread_self());
    // build dfa
    FOR_EACH(prefix_info_itr, m_prefix_info_write) {
        prefix_info_itr->BuildDfa(); 
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:Finalize\tinfo:end to build DFA", __FILE__, __LINE__, pthread_self());
    // write.swap(read) should be async
    Separation();
    return 0;
}

int CPatternDict::Info(std::string* info) {

    *info = "pattern dict";
    return 0;
}
int CPatternDict::Clear() {
    if(m_dict_info_write.size() > 0) {
        std::vector<DictInfoMeta>   t_dict_info_write;
        m_dict_info_write.swap(t_dict_info_write);
    }

    if(m_prefix_info_write.size() > 0) {
        std::vector<PrefixInfoMeta> t_prefix_info_write;
        m_prefix_info_write.swap(t_prefix_info_write);
    }
    return 0;
}
int CPatternDict::Separation() {

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Separation begin use m_dict_info_write swap m_dict_info_read",
            __FILE__,
            __LINE__,
            pthread_self());

    if(NULL == m_darts_datrie_write) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:CPatternDict::Separation begin use m_dict_info_write swap m_dict_info_read",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }

    m_meta_info_lock_control.EntryWrite();
    {
        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Separation Get the mutex",
                __FILE__,
                __LINE__,
                pthread_self());


        RWGuard g(m_meta_info_mutex, true);
        m_dict_info_read.swap(m_dict_info_write);
        m_prefix_info_read.swap(m_prefix_info_write);

        DartsDatrie* pre        = m_darts_datrie_read;
        m_darts_datrie_read     = m_darts_datrie_write;
        m_darts_datrie_write    = pre;

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::Separation Drop the mutex",
                __FILE__,
                __LINE__,
                pthread_self());

    }
    m_meta_info_lock_control.ExitWrite();

    // TODO: shall wait for five minutes
    std::vector<DictInfoMeta>   t_dict_info_write;
    std::vector<PrefixInfoMeta> t_prefix_info_write;

    m_dict_info_write.swap(t_dict_info_write);
    m_prefix_info_write.swap(t_prefix_info_write);

    m_dict_info_write.insert(m_dict_info_write.begin(), m_dict_info_read.begin(), m_dict_info_read.end());
    m_prefix_info_write.insert(m_prefix_info_write.begin(), m_prefix_info_read.begin(), m_prefix_info_read.end());

    delete m_darts_datrie_write;
    m_darts_datrie_write = NULL;
    return 0;
}

int CPatternDict::BuildTrie() {
    int prefix_info_write_len = m_prefix_info_write.size();
    std::vector<const char *> keys;
    std::vector<std::size_t> lengths;
    std::vector<DartsDatrie::value_type> values;

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::BuildTrie begin build and m_prefix_info_write.size=%d",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    prefix_info_write_len);

    // 1. sort m_prefix_info_write because DartsTrie's keys mast be sort
    std::sort(m_prefix_info_write.begin(), m_prefix_info_write.end());

    // 2. prepare keys, lengths, values
    for(size_t i = 0; i < prefix_info_write_len; ++i) {
        std::string prefix = m_prefix_info_write[i].prefix;
        if(prefix.size() == 0) {

            log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildTrie\tinfo:not good key %d",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    i);

            continue;
        }

        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::BuildTrie m_prefix_info_write[%d]=%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                i,
                prefix.c_str());

        // TODO: should remove "\" in "\.", "\$", "\{"
        keys.push_back(prefix.c_str());
        lengths.push_back(prefix.size());
        if(lengths[lengths.size() - 1] == 0) {
            log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildTrie\tinfo:length is 0, index=%d",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    i);
        }
        values.push_back(static_cast<DartsDatrie::value_type>(i));
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildTrie\tinfo:key's size %d, length's size %d",
            __FILE__,
            __LINE__,
            pthread_self(),
            keys.size(),
            lengths.size());

    // 3. build darts
    if(m_darts_datrie_write == NULL) {

        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildTrie\tinfo:darts_trie is null", 
                __FILE__,
                __LINE__,
                pthread_self());

        return 2; 
    }
    try {
        int rc = 0;
        if((rc = m_darts_datrie_write->build(
                        keys.size(),
                        &keys[0],
                        &lengths[0],
                        &values[0])) && rc != 0) {

            log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildTrie\tinfo:m_darts_datrie_write.build ERROR",
                    __FILE__,
                    __LINE__,
                    pthread_self());

            return 1;
        }
    }catch(const std::exception &ex) {

        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildTrie\tinfo:m_darts_datrie_write.build Exception %s",
                __FILE__,
                __LINE__,
                pthread_self(),
                ex.what());

        return 2;
    }
    return 0;
}

int CPatternDict::RePrefix(const std::string& re_string, std::string* prefix) {

    std::string spec_sym_arr[] = {"$", "(", "*", "+", ".", "[", "?", "^", "{", "|"};
    size_t size = (size_t)(sizeof(spec_sym_arr) / sizeof(spec_sym_arr[0]) );
    std::set<std::string> spec_sym_set(spec_sym_arr, spec_sym_arr + size);

    size_t min_pos = -1;

    std::string trope_char = "\\";
    // 1. check trope_char
    // eg: http://www.so.com/\(not_trope\$not_trope\\not_trop\Aistrop/  then the prefix:http://www.so.com/\(not_trope\$not_trope\\not_trop 
    size_t hit_pos = 0;
    while((hit_pos = re_string.find(trope_char, hit_pos)) && hit_pos != std::string::npos) {
        if(hit_pos + 1 == std::string::npos) {
            break; 
        }
        std::string next_ch(1, re_string[hit_pos + 1]);
        if(spec_sym_set.count(next_ch) == 0 && next_ch != "\\") {
            min_pos = min_pos > hit_pos ? hit_pos : min_pos;
            break; 
        }
        hit_pos += trope_char.size() + 1;
    }

    // 2. check spec_sym_arr find spec_sym
    // eg: http://www.so.com/\(notspec\$notspec(isspec)isspec then http://www.so.com/\(notspec\$notspec is the target
    for(size_t i = 0; i < size; ++i) {
        std::string spec_sym = spec_sym_arr[i]; 
        hit_pos = 0;
        while((hit_pos = re_string.find(spec_sym, hit_pos)) && hit_pos != std::string::npos) {
            if(hit_pos - 1 < 0) {
                continue; 
            }
            std::string pre_ch(1, re_string[hit_pos - 1]);
            if(pre_ch != "\\") {
                min_pos = min_pos > hit_pos ? hit_pos : min_pos;
                break; 
            }
            hit_pos += spec_sym.size();
        }
    }
    std::string prefix_with_trope = re_string.substr(0, min_pos);

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:RePrefix\tfunction:BuildTrie\tinfo:prefix_with_trope %s", 
            __FILE__, 
            __LINE__, 
            pthread_self(), 
            prefix_with_trope.c_str());
    // 3. remove trope '\'
    trope_char = "\\";
    hit_pos = 0;
    while((hit_pos = prefix_with_trope.find(trope_char, hit_pos)) && hit_pos != std::string::npos) {
        if(hit_pos + 1 == std::string::npos) {
            break;
        }
        std::string next_ch(1, prefix_with_trope[hit_pos + 1]);
        if(spec_sym_set.count(next_ch) != 0) {
            // the hit_pos is trope
            prefix_with_trope.erase(prefix_with_trope.begin() + hit_pos);
        }
        // 
        ++ hit_pos;
    }
    std::string prefix_without_trope = prefix_with_trope;

    if(0 == prefix_without_trope.find_first_of("^")) {
        prefix_without_trope.erase(0, 1);
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:RePrefix\tfunction:BuildTrie\tinfo:prefix_without_trope=%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            prefix_without_trope.c_str());

    *prefix = prefix_without_trope;
    return 0;
}

int CPatternDict::ReLegal(const std::string& re_string) {
    RE2 re(re_string);
    return re.ok() ? 0 : 1;
}

int CPatternDict::PrefixInfoMeta::BuildDfa() {

    // 1. init dfa_opt
    re2::RE2::Options dfa_opt;
    dfa_opt.set_case_sensitive(false);
    dfa_opt.set_never_capture(true);
    dfa_opt.set_max_mem(DFA_MAX_MEM);

    // 2. init dfa
    dfa =  new re2::RE2::Set(dfa_opt, RE2::ANCHOR_START);
    size_t regex_info_repo_len = regex_info_repo.size();
    for(size_t i = 0; i < regex_info_repo_len; ++i) {
        int rc = 0;
        std::string regex = regex_info_repo[i].regex;
        re2::StringPiece sp(regex.c_str(), regex.size());
        if((rc = dfa->Add(sp, NULL)) && rc == -1) {

            regex_info_offline.push_back(regex_info_repo[i]);

            log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::BuildDfa regex[%s] is regex_info_offline",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    regex.c_str());
        }
        else {
            regex_info_online.push_back(regex_info_repo[i]);

            log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:CPatternDict::BuildDfa regex[%s] is regex_info_online",
                    __FILE__,
                    __LINE__,
                    pthread_self(),
                    regex.c_str());

        }
    }
    // 3. compile dfa
    if(!dfa->Compile()) {
        log(LOG_ERROR, "%s:%d\ttid:%lld\tclass:CPatternDict\tfunction:BuildDfa\tinfo:dfa->Compile Error",
                __FILE__,
                __LINE__,
                pthread_self());
        return 1;
    }
    return 0;
}
