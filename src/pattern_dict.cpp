#include <fstream>
#include <algorithm>
#include <set>

#include "pattern_dict.h"
#include "pattern_dict_key.h"
#include "pattern_dict_value.h"
#include "pattern_dict_struct_key.h"
#include "pattern_dict_struct_value.h"

#include "util.h"
#include "log.h"

// log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDict\tfunction:Get\tkey:%s", __FILE__, __LINE__, pthread_self(), keyword.c_str());
PatternDict::PatternDict() {

    m_darts_datrie_read     = new DartsDatrie();
    m_darts_datrie_write    = new DartsDatrie();
}
PatternDict::PatternDict(const std::string key_class, const std::string value_class) : m_darts_datrie_read(NULL), m_darts_datrie_write(NULL) {
    m_key_class = key_class;
    m_value_class = value_class;

    m_darts_datrie_read     = new DartsDatrie();
    m_darts_datrie_write    = new DartsDatrie();
}

PatternDict::~PatternDict() {
    if(NULL != m_darts_datrie_read) {
        delete m_darts_datrie_read;
        m_darts_datrie_read = NULL;
    }

    if(NULL != m_darts_datrie_write) {
        delete m_darts_datrie_write;
        m_darts_datrie_write = NULL;
    }
}

int PatternDict::Set(const IKey& key, const IValue& value) {
    std::string re_string;
    int rc;
    if((rc = key.GetKey(&re_string)) && rc != 0) {
        std::cerr << "key() error:" << rc << std::endl;
        return 1;
    }
    // check the re is legal
    if((rc = ReLegal(re_string)) && rc != 0) {
        std::cerr << "ReLegal() error:" << rc << std::endl;
        return 2;
    }

    // check in m_dict_info or not
    auto key_itr =  m_dict_info_write.end();
    FOR_EACH(itr, m_dict_info_write) {
        if(*itr == key) {
            key_itr = itr; 
        }
    }
    if(m_dict_info_write.end() != key_itr) {
        m_dict_info_write.erase(key_itr);
    }
    m_dict_info_write.push_back(DictInfoMeta(const_cast<IKey*>(&key), const_cast<IValue*>(&value)));

    return 0;
}


int PatternDict::Del(const IKey& key) {
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

int PatternDict::Get(const IKey& key, std::vector<IValue*>* value) {
    std::vector<DartsDatrie::result_pair_type> results;
    size_t result_size = 0;
    std::string keyword;
    key.GetKey(&keyword);

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDict\tfunction:Get\tinfo:key[%s]", __FILE__, __LINE__, pthread_self(), keyword.c_str());

    // check trie and get m_prefix_info_read s' index
    if((result_size = m_darts_datrie_read->commonPrefixSearch(keyword.c_str(), results)) && result_size == 0) {
        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDict\tfunction:Get\tinfo:miss darts", __FILE__, __LINE__, pthread_self());
        return 1;
    }

    if(result_size != results.size()) {
        log(LOG_WARNING, "%s:%d\ttid:%lld\tclass:PatternDict\tfunction:Get\tinfo:match size error", __FILE__, __LINE__, pthread_self());
        return 2;
    }

    // get hit_prefix_info by hit_index
    std::vector<PrefixInfoMeta> hit_prefix_info;
    for(size_t i = 0; i < result_size; ++i) {
        size_t index = static_cast<size_t>(results[i].value);
        hit_prefix_info.push_back(m_prefix_info_read[index]);
        log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDict\tfunction:Get\tinfo:hit prefix[%s]", __FILE__, __LINE__, pthread_self(),(m_prefix_info_read[index].prefix).c_str());
    }

    std::vector<RegexInfoMeta> hit_regex_info;
    // check DFAs and get hit_regex_info from regex_info_online/offline
    FOR_EACH(prefix_info_itr, hit_prefix_info) {
        std::vector<int> match_regex_info_index; 
        prefix_info_itr->dfa->Match(keyword, &match_regex_info_index);
        for(size_t i = 0; i < match_regex_info_index.size(); ++i) {
            // bug
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
        std::string str;
        hit_dict_info[i].key->ToString(&str);
        std::cout << "debug hit key info:" << str << std::endl;
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
    std::ifstream in_stream(dict_data_load_path.c_str(), std::ifstream::in);
    if(!in_stream.is_open()) {
        std::cerr << "error to open input file:" << dict_data_load_path << std::endl; 
        return 1;
    }
    std::string line;
    while(std::getline(in_stream, line) && !in_stream.eof()) {
        // TODO: class_key, class_value should come from config 
        IKey* key = new PatternDictStructKey();
        IValue* value = new PatternDictStructValue();

        key->Init(&line);
        value->Init(&line);

        if(Set(*key, *value)) {
            std::cerr << "Load::Set key error, key input:" << line << std::endl;
            in_stream.close();
            return 2;
        }
    }
    in_stream.close();
    // Finalize
    if(Finalize()) {
        std::cerr << "Load::Finalize  error"  << std::endl;
        in_stream.close();
        return 3; 
    }
    return 0;
}

int PatternDict::Dump(const std::string& dict_data_dump_path) {
    std::ofstream out_stream(dict_data_dump_path, std::ifstream::out);
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
        m_dict_info_write[i].key->GetKey(&re_string);

        PrefixInfoMeta prefix_info;
        int rc = 0;
        if((rc = RePrefix(re_string, &(prefix_info.prefix))) && rc != 0) {
            std::cerr << "RePrefix() error:" << rc << std::endl;
            continue;
        }

        std::cout << "debug prefix:" << prefix_info.prefix << std::endl; 

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

    std::cout << "debug: build Trie begin" << std::endl;
    // build trie
    int rc = 0;
    if((rc = BuildTrie()) && rc != 0) {
        std::cerr << "BuildTrie() error:" << rc << std::endl;
        return 1;
    }
    std::cout << "debug: build Trie end" << std::endl;
    // build dfa
    FOR_EACH(prefix_info_itr, m_prefix_info_write) {
        prefix_info_itr->BuildDfa(); 
    }

    std::cout << "debug: build Dfa end" << std::endl;
    // write.swap(read) should be async
    Separation();
    return 0;
}
int PatternDict::Clear() {
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
int PatternDict::Separation() {
    if(NULL == m_darts_datrie_write) {
        return 1;
    }
    m_meta_info_lock_control.EntryWrite();
    {
        RWGuard g(m_meta_info_mutex, true);
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

    m_dict_info_write.insert(m_dict_info_write.begin(), m_dict_info_read.begin(), m_dict_info_read.end());
    m_prefix_info_write.insert(m_prefix_info_write.begin(), m_prefix_info_read.begin(), m_prefix_info_read.end());

    delete m_darts_datrie_write;
    m_darts_datrie_write = NULL;
    return 0;
}

int PatternDict::BuildTrie() {
    int prefix_info_write_len = m_prefix_info_write.size();
    std::vector<const char *> keys;
    std::vector<std::size_t> lengths;
    std::vector<DartsDatrie::value_type> values;

    // 1. sort m_prefix_info_write because DartsTrie's keys mast be sort
    std::sort(m_prefix_info_write.begin(), m_prefix_info_write.end());

    // 2. prepare keys, lengths, values
     for(size_t i = 0; i < prefix_info_write_len; ++i) {
         std::string prefix = m_prefix_info_write[i].prefix;
         if(prefix.size() == 0) {
             std::cout << "debug no good key:" << i  << std::endl;
             continue;
         }
         // TODO: should remove "\" in "\.", "\$", "\{"
         keys.push_back(prefix.c_str());
         lengths.push_back(prefix.size());
         if(lengths[lengths.size() - 1] == 0) {
             std::cout << "length[i] is 0 :" << i << std::endl;
         }
         values.push_back(static_cast<DartsDatrie::value_type>(i));
     }
     std::cout << "key'size:" << keys.size() << "\tlength'size:" << lengths.size() <<"\t" << prefix_info_write_len << std::endl;
    // 3. build darts
    if(m_darts_datrie_write == NULL) {
        std::cerr << "darts_trie is null" << std::endl;
        return 2; 
    }
    try {
        int rc = 0;
         if((rc = m_darts_datrie_write->build(
                         keys.size(),
                         &keys[0],
                         &lengths[0],
                         &values[0])) && rc != 0) {
             std::cerr << "m_darts_datrie_write.build error" << std::endl;
             return 1;
         }
    }catch(const std::exception &ex) {
        std::cerr << "exception: " << ex.what() << std::endl;  
        return 2;
    }
    return 0;
}

int PatternDict::RePrefix(const std::string& re_string, std::string* prefix) {
    std::string spec_sym_arr[] = {"$", "(", "*", "+", ".", "[", "?", "^", "{", "|"};
    size_t size = (size_t)(sizeof(spec_sym_arr) / sizeof(spec_sym_arr[0]) );
    std::set<std::string> spec_sym_set(spec_sym_arr, spec_sym_arr + size);

    size_t min_pos = -1;

    std::string trope_char = "\\";
    // 1. check trope_char
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

    // 2. check spec_sys_arr
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
    std::cout << "debug prefix_with_trope:" << prefix_with_trope << std::endl;
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
    std::cout << "debug prefix_without_trope:" << prefix_without_trope << std::endl;
    *prefix = prefix_without_trope;
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
    for(size_t i = 0; i < regex_info_repo_len; ++i) {
        int rc = 0;
        std::string regex = regex_info_repo[i].regex;
        re2::StringPiece sp(regex.c_str(), regex.size());
        if((rc = dfa->Add(sp, NULL)) && rc == -1) {
            std::cerr << "warning:" << regex << "\tadd into set fail" << std::endl;
            regex_info_offline.push_back(regex_info_repo[i]);
        }
        else {
            regex_info_online.push_back(regex_info_repo[i]);
        }
    }
    // 3. compile dfa
    if(!dfa->Compile()) {
        std::cerr << "dfa->Compile error" << std::endl;
        return 1;
    }
    return 0;
}
