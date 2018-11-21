#ifndef __BUSI_DICT_LEVEL__
#define __BUSI_DICT_LELEL__


class CBusiDictLevel{
    public:
        enum DICT_TYPE {
            DICT_KV = 1,
            DICT_PATTERN,
            DICT_CONTAIN,
            DICT_CONTAIN_MULTI,
            DICT_CONTAIN_MULTI_SEQ
        };
    public:
        CBusiDictLevel();
        CBusiDictLevel(const std::string& level);
        CBusiDictLevel(const std::string& level, const std::string& config);
        ~CBusiDictLevel();

        int SetConfig(const std::string& config);
        int SetLevel(const std::string& level);

        int Load();

        int Reload();
        int ReloadDict(const std::string& dict_unit);

        int DumpLevel();
        int DumpDict(const std::string& dict_unit);

        int AddDict(const std::string& dict_unit, const std::string& dict_path, const int dict_type, const bool is_blacklist = true);

        int AddKey(const std::string& dict_unit, const std::string& key, const std::string& value);


        int DelDict(const std::string& level, const std::string& dict_unit);

        int DelKey(const std::string& dict_unit, const std::string& key);

        int Match(const std::string& dict_unit, const std::string& key, std::vector<std::string>* value);

        int MatchOrder(const std::string& key, int* hit, std::string* hit_dict_unit = NULL, std::vector<std::string>* value = NULL);

        int Info(std::map<std::string, std::string>* infos);

    private:
        const double GetCurrentTimestamp();
        DICT_TYPE GetDictType(const std::string& dict_type_str);

    private:
        std::string                             m_level;
        std::string                             m_config_path;
        std::string                             m_dump_path;
        std::vector<std::string>                m_order;

        std::map<std::string, IDict*>           m_dict_repo_online;
        std::map<double, IDict*>                m_dict_repo_offline;
        std::map<std::string, std::string>      m_dict_path;
        std::map<std::string, DICT_TYPE>        m_dict_type;

        std::set<std::string>                   m_blacklist_dict;
        std::set<std::string>                   m_whitelist_dict;
};
#endif
