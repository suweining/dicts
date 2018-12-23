#ifndef __BUSI_DICT_HANDLER__
#define __BUSI_DICT_HANDLER__

#include <string>
#include <vector>
#include <map>

#include "busi_dict_level.h"

class CBusiDictHandler {
    public:
        CBusiDictHandler();
        CBusiDictHandler(const std::string& config);
        ~CBusiDictHandler();

        int SetConfig(const std::string& config);
        int Load();

        int ReloadLevel(const std::string& level);
        int ReloadDict(const std::string& level, const std::string& dict_unit);

        int DumpLevel(const std::string& level);
        int DumpDict(const std::string& level, const std::string& dict_unit);

        int AddDict(const std::string& level, const std::string& dict_unit, int dict_type, const std::string& dict_path, bool is_blacklist = true);

        int AddKey(const std::string& level, const std::string& dict_unit, const std::string& key, const std::string& value);


        int DelDict(const std::string& level, const std::string& dict_unit);

        int DelKey(const std::string& level, const std::string& dict_unit, const std::string& key);

        int Match(const std::string& level, const std::string& dict_unit, const std::string& key, std::vector<std::string>* value);

        int MatchOrder(const std::string& level, const std::string& key, std::vector<std::string>* value);

        int Info(const std::string& level, std::map<std::string, std::string>* infos);

    private:
        int ReadConfig();

    private:
        std::string                                 m_config_path;
        std::string                                 m_levels;
        std::map<std::string, CBusiDictLevel*>      m_dict_level_repo;
};
#endif
