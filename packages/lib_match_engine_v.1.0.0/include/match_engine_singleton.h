#ifndef __DICTS_MATCH_ENGINE_SINGLETON__
#define __DICTS_MATCH_ENGINE_SINGLETON__

#include "match_engine_level.h"
#include "singleton.h"

class CMatchEngineSingleton : public Singleton<CMatchEngineSingleton> {
    public:
        CMatchEngineSingleton();
        CMatchEngineSingleton(const std::string& config);
        ~CMatchEngineSingleton();

        int SetConfig(const std::string& config);

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
        CMatchEngineLevel*  m_match_engine_level;
        std::string         m_client_ip;
};

#endif
