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
        int Init(const std::string& params); // init dict
        int Set(const std::shared_ptr<IKey> key, const std::shared_ptr<IValue> value); // use for update value while dict has build
        int Add(const std::shared_ptr<IKey> key, const std::shared_ptr<IValue> value); // add record for building the dict
        int Del(const std::shared_ptr<IKey> key); // del record info in the dict
        int Get(const std::shared_ptr<IKey> key, std::vector<std::shared_ptr<IValue> >* value); // match key
        int Dump(const std::string& dict_data_dump_path); // write the record into file
        int Finalize(); // building the dict while all record add into mem
        int Capacity(size_t capacity); // init dict
        int Info(std::string* info); // info for dict

        int Clear(); // clear dict 

    private:
       RedisClient* m_redis_client; 

};

#endif
