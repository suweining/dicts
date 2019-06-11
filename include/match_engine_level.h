#ifndef __DICTS_MATCH_ENGINE_LEVEL__
#define __DICTS_MATCH_ENGINE_LEVEL__

#include <string>
#include <map>
#include <vector>

#include "match_engine_unit.h"

class CMatchEngineLevel {
    
    public:
        CMatchEngineLevel();
        CMatchEngineLevel(const std::string& config);
        ~CMatchEngineLevel();
    
        int SetConfig(const std::string& config);

        int Init();
        int Load();
        int Reload(); // reload all
        int Reload(const std::string& engine);
        int Dump(); // dump all
        int Dump(const std::string& engine);
        
        int Set(const std::string& engine, const std::string& key, const std::string& value);
        int Add(const std::string& engine, const std::string& key, const std::string& value);
        int Del(const std::string& engine, const std::string& key);
        int GetEngine(const std::string& engine, const std::string& key, std::vector<std::string>* value);
        int GetLevel(const std::string& level, const std::string& key, std::vector<std::string>* value);
        int GetSpec(const std::string& spec, const std::string& key, std::vector<std::string>* value);

    private:
        std::string                                         m_config;
        std::map<std::string, std::vector<std::string> >    m_level_pool;
        std::map<std::string, CMatchEngineUnit*>            m_engine_pool; 
};

#endif
