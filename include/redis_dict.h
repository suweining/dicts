#ifndef __DICTS_REDIS_DICT__
#define __DICTS_REDIS_DICT__

#include <string>
#include <vector>
#include "redis_client.h"
#include "dict.h"

class CRedisDict : public IDict {

    public:
        CRedisDict();
        ~CRedisDict(); 

        int Init(const std::string& params);
        int Add(const IKey& key, const IValue& value);
        int Set(const IKey& key, const IValue& value);
        int Del(const IKey& key);
        int Get(const IKey& key, std::vector<IValue*>* value);
        int Dump(const std::string& dict_data_dump_path);
        int Finalize();

        int Info(std::string* info);
        int Clear();

    private:
       RedisClient* m_redis_client; 

};

#endif
