PatternDict::PatternDict() {

}

int PatternDict::Set(const IKey& key, const IValue& value) {
    std::string re_info;
    if((int rc = key.Key(&re_info)) && rc != 0) {
        std::cerr << "key() error:" << rc << std::endl;
        return 1;  
    }

    // TODO: check the re is legal
    if((int rc = ReLegal(re_info)) && rc != 0) {
        std::cerr << "ReLegal() error:" << rc << std::endl;
        return 2; 
    } 

    // TODO: check in m_dict_info or not
    auto key_itr = EXIST(key, m_dict_info_write);
    if(NULL != key_itr) {
        m_dict_info_write.erase(key_itr);
    }
    m_dict_info_write.push_back(DictInfoMeta(key, value));

    /*

    // TODO: get prefix of re
    PrefixInfoMeta prefix_info;
    if((int rc = RePrefix(re_info, &(prefix_info.prefix)) && rc != 0) {
        std::cerr << "RePrefix() error:" << rc << std::endl;
        return 3;        
    } 
    
    RegexInfoMeta regex_info_meta;
    regex_info_meta.regex   = re_info;
    regex_info_meta.index   = m_dict_info.size(); 

    // TODO: prefix in m_prefix_info or not
    auto prefix_info_itr = EXIST(prefix_info, m_prefix_info); 
    if(NULL == prefix_info_itr) {
        // not exist
        prefix_info.regex_info_repo.push_back(regex_info_meta);
        m_prefix_info.push_back(prefix_info);
    }

    */
    return 0;
}


int PatternDict::Del(const IKey& key) {
    // TODO: check in m_dict_info_write or not
    auto key_itr = EXIST(key, m_dict_info_write);
    if(NULL == key_itr) {
        return 1; 
    }
    m_dict_info_write.erase(key_itr);
    return 0;
}

int PatternDict::Get(const IKey& key, IValue* value) {
    // TODO: check trie and get m_prefix_info_read s' index 
    // TODO: check DFAs and get regex_info_online/offline
    // TODO: check regex in regex_info_online/offline and get m_dict_info_read s' index     
    return 0;
}

int PatternDict::Load(const std::string& dict_data_load_path) {
    if(0 != m_dict_info_write.size()) {
        std::vector<DictInfoMeta>   t_dict_info_write;
        std::vector<PrefixInfoMeta> t_prefix_info_write;

        m_dict_info_write.swap(t_dict_info_write);
        m_prefix_info_write.swap(t_prefix_info_write);
    }
    // TODO: read file
    // Set
    // Finalize
    return 0;
}

int PatternDict::Dump(const std::string& dict_data_dump_path) {
    return 0;
}

int PatternDict::Finalize() {
    // async
    return 0;
}
