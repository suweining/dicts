#ifndef __DICTS_MATCH_ENGINE_UNIT__
#define __DICTS_MATCH_ENGINE_UNIT__

#include <string>
#include <vector>

#include "dict.h"

class CMatchEngineUnit {

    public:
       CMatchEngineUnit();
       CMatchEngineUnit(const std::string& config, const std::string& engine);
       ~CMatchEngineUnit();

       int SetConfig(const std::string& config);
       int SetEngine(const std::string& engine);

       int Init();

       int Load();
       int Reload();
       int Reload(const std::string& load_path);

       int Dump();
       int Dump(const std::string& dump_path);

       int Set(const std::string& key, const std::string& value);
       int Add(const std::string& key, const std::string& value);
       int Del(const std::string& key);
       int Get(const std::string& key, std::vector<std::string>* value);

    private:
       int readConfig();

    private:
       std::string      m_config;
       std::string      m_engine;

       std::string      m_dict_type;
       std::string      m_key_type;
       std::string      m_value_type;
       std::string      m_load_path; 
       std::string      m_dump_path;
       bool             m_blacklist;

       IDict*           m_dict;  



};

#endif
