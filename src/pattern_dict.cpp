#include "pattern_dict.h"
#include "pattern_dict_key.h"
#include "pattern_dict_value.h"

PatternDict::PatternDict() {

}

int PatternDict::Set(const IKey* key, const IValue* value) {
    std::string re_string;
    if((int rc = key->Key(&re_string)) && rc != 0) {
        std::cerr << "key() error:" << rc << std::endl;
        return 1;
    }

    // check the re is legal
    if((int rc = ReLegal(re_string)) && rc != 0) {
        std::cerr << "ReLegal() error:" << rc << std::endl;
        return 2;
    }

    // check in m_dict_info or not
    auto key_itr = EXIST(*key, m_dict_info_write);
    if(NULL != key_itr) {
        m_dict_info_write.erase(key_itr);
    }
    m_dict_info_write.push_back(DictInfoMeta(key, value));

    return 0;
}


int PatternDict::Del(const IKey& key) {
    // check in m_dict_info_write or not
    auto key_itr = EXIST(key, m_dict_info_write);
    if(NULL == key_itr) {
        return 1;
    }
    m_dict_info_write.erase(key_itr);
    return 0;
}

int PatternDict::Get(const IKey& key, std::vector<IValue*>* value) {
    std::vector<DartsDatrie::result_pair_type> results;
    size_t result_size = 0;
    std::string keyword;
    key->Key(&keyword);

    // check trie and get m_prefix_info_read s' index
    if((result_size = m_darts_datrie_read->commonPrefixSearch(keyword, results)) && result_size == 0) {
        std::cerr << "WARNING: miss darts dict" << std::endl;
        return 1;
    }

    if(result_size != results.size()) {
        std::cerr << "WARNING: darts match fail" << std::endl;
        return 2;
    }

    // get hit_prefix_info by hit_index
    std::vector<PrefixInfoMeta> hit_prefix_info;
    for(size_t i = 0; i < result_size; ++i) {
        size_t index = static_cast<size_t>(results[i]);
        hit_prefix_info.push_back(m_prefix_info_read[index]);
    }

    std::vector<RegexInfoMeta> hit_regex_info;
    // check DFAs and get hit_regex_info from regex_info_online/offline
    FOR_EACH(prefix_info_itr, hit_prefix_info) {
        std::vector<int> match_regex_info_index; 
        prefix_info_itr->dfa->Match(keyword, &match_regex_info_index);
        for(size_t i = 0; i < match_regex_info_index.size(); ++i) {
            hit_regex_info.push_back(prefix_info_itr->regex_info_online[i]);
        }
        for(size_t i = 0; i < prefix_info_itr->regex_info_offline.size(); ++i) {
            if(RE2::FullMatch(keyword.c_str(), prefix_info_itr->regex_info_offline[i].regex.c_str())) {
                hit_regex_info.push_back(prefix_info_itr->regex_info_offline[i]);
            }
        }
    }
    // get hit_dict_info index from hit_prefix_info
    std::vector<DictInfoMeta> hit_dict_info;
    for(size_t i = 0; i < hit_regex_info.size(); ++i) {
        hit_dict_info.push_back(m_dict_info_read[hit_regex_info[i].index]);
    }

    // get values from hit_dict_info
    for(size_t i = 0; i < hit_dict_info.size(); ++i) {
        value->push_back(hit_dict_info[i].value);
    }

    return 0;
}

int PatternDict::Load(const std::string& dict_data_load_path) {
    if(0 != m_dict_info_write.size()) {
        std::vector<DictInfoMeta>   t_dict_info_write;
        std::vector<PrefixInfoMeta> t_prefix_info_write;

        m_dict_info_write.swap(t_dict_info_write);
        m_prefix_info_write.swap(t_prefix_info_write);
    }
    // read file
    std::ifstream in_stream(dict_data_load_path);
    if(!in_stream.is_open()) {
        std::cerr << "error to open input file:" << dict_data_load_path << std::endl; 
        return 1;
    }
    std::string line;
    while(in_stream >> line) {
        IKey* key = new PatternDictKey(line);
        IValue* value = new PatternDictValue(line);

        if(Set(key, value)) {
            std::cerr << "Load::Set key error, key input:" << line << std::endl;
            in_stream.close();
            return 2;
        }
    }
    // Finalize
    if(Finalize()) {
        std::cerr << "Load::Finalize  error"  << std::endl;
        in_stream.close();
        return 3; 
    }
    in_stream.close();
    return 0;
}

int PatternDict::Dump(const std::string& dict_data_dump_path) {
    std::ofstream out_stream(dict_data_dump_path);
    if(out_stream.is_open()) {
        std::cerr << "error to open dump file:" << dict_data_dump_path << std::endl;
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

int PatternDict::Finalize() {
    //prepare m_prefix_info_write by m_dict_info_write 
    size_t dict_info_len = m_dict_info_write.size();
    for(size_t i = 0; i < dict_info_len; ++i) {
        std::string re_string;
        dict_info_itr->key->Key(&re_string);
        PrefixInfoMeta prefix_info;
        if((int rc = RePrefix(re_string, &(prefix_info.prefix)) && rc != 0) {
            std::cerr << "RePrefix() error:" << rc << std::endl;
            continue;
        }

        RegexInfoMeta regex_info_meta;
        regex_info_meta.regex   = re_string;
        regex_info_meta.index   = i; 

        auto prefix_info_itr = EXIST(prefix_info, m_prefix_info_write);
        if(NULL == prefix_info_itr) {
            // not exist in m_prefix_info_write
            prefix_info.regex_info_repo.push_back(regex_info_meta);
            m_prefix_info_write.push_back(prefix_info);
        }
        else {
            // prefix_info has exist in m_prefix_info_write
            prefix_info_itr->regex_info_repo.push_back(regex_info_meta);
        }
    }

    // build trie
    if((int rc = BuildTrie()) && rc != 0) {
        std::cerr << "BuildTrie() error:" << rc << std::endl;
        return 1;
    }
    // build dfa
    FOR_EACH(prefix_info_itr, m_prefix_info_write) {
        prefix_info_itr->BuildDfa(); 
    }
    // write.swap(read) should be async
    Separation();
    return 0;
}

int PatternDict::Separation() {
    if(NULL == m_darts_datrie_write) {
        return 1;
    }
    m_meta_info_lock_control.EntryWrite();
    {
        Guard g(m_meta_info_mutex, true);
        m_dict_info_read.swap(m_dict_info_write);
        m_prefix_info_read.swap(m_prefix_info_write);

        DartsDatrie* pre        = m_darts_datrie_read;
        m_darts_datrie_read     = m_darts_datrie_write;
        m_darts_datrie_write    = pre;
    }
    m_meta_info_lock_control.ExitWrite();

    // TODO: shall wait for five minutes
    std::vector<DictInfoMeta>   t_dict_info_write;
    std::vector<PrefixInfoMeta> t_prefix_info_write;

    m_dict_info_write.swap(t_dict_info_write);
    m_prefix_info_write.swap(t_prefix_info_write);

    m_dict_info_write.insert(m_dict_info_read.begin(), m_dict_info_read.end());
    m_prefix_info_write.insert(m_prefix_info_read.begin(), m_prefix_info_read.end());

    delete m_darts_datrie_write;
    m_darts_datrie_write = NULL;
    return 0;
}

int PatternDict::BuildTrie() {
    int prefix_info_write_len = m_prefix_info_write.size();
    std::vector<const char *> keys(prefix_info_write_len);
    std::vector<std::size_t> lengths(prefix_info_write_len);
    std::vector<DartsDatrie::value_type> values(prefix_info_write_len);

    // 1. sort m_prefix_info_write because DartsTrie's keys mast be sort
    std::sort(m_prefix_info_write.begin(), m_prefix_info_write.end());

    // 2. prepare keys, lengths, values
    for(size_t i = 0; i < prefix_info_write_len; ++i) {
        std::string prefix = m_prefix_info_write[i].prefix;
        keys.push_back(prefix);
        lengths.push_back(prefix.length());
        values.push_back(static_cast<DartsDatrie::value_type>(i));
    }
    // 3. build darts
    try {
        if((int rc = m_darts_datrie_write.build(
                        prefix_info_write_len,
                        keys,
                        lengths,
                        values)) && rc != 0) {
            std::cerr << "m_darts_datrie_write.build error" << std::endl;
            return 1;
        }
    }
    catch(const std::exception &ex) {
        std::cerr << "exception: " << ex.what() << std::endl;  
        return 2;
    }
    return 0;
}

int PatternDict::RePrefix(const std::string& re_string, std::string* prefix) {
    *prefix = re_string;
    return 0;
}

int PatternDict::ReLegal(const std::string& re_string) {
    RE2 re(re_string);
    return re.ok() ? 0 : 1;
}

int PatternDict::PrefixInfoMeta::BuildDfa() {

    // 1. init dfa_opt
    re2::RE2::Options dfa_opt;
    dfa_opt.set_case_sensitive(false);
    dfa_opt.set_never_capture(true); 
    dfa_opt.set_max_mem(DFA_MAX_MEM);

    // 2. init dfa
    dfa =  new re2::RE2::Set(dfa_opt, RE2::ANCHOR_START);
    size_t regex_info_repo_len = regex_info_repo.size();
    for(size_t i = 0; i < regex_info_repo_len; +i) {
        std::string regex = regex_info_repo[i].regex;
        re2::StringPiece sp(regex.c_str(), regex.size());
        if((int rc = dfa->Add(sp, NULL)) && rc == -1) {
            std::cerr << "warning:" << regex << "\tadd into set fail" << std::endl;
            regex_info_offline.push_back(regex_info_repo[i]);
        }
        else {
            regex_info_online.push_back(regex_info_repo[i]);
        }
    }
    return 0;
}
