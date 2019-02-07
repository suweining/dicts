#ifndef __DICTS_KV_DICT__
#define __DICTS_KV_DICT__

#include "dict.h"
#include "hash_dict.h"

class CKvDict : public IDict {

    public:
        CKvDict();
        ~CKvDict(); 

        int Add(const IKey& key, const IValue& value);
        int Set(const IKey& key, const IValue& value);
        int Del(const IKey& key);
        int Get(const IKey& key, std::vector<IValue*>* value);
        int Load(const std::string& dict_data_load_path);
        int Dump(const std::string& dict_data_dump_path);
        int Finalize();

        int Info(std::string* info);
        int Clear();

    private:

        #define VALUE_TYPE int64_t 

        HashDict<VALUE_TYPE>            m_hash_dict_engine;
        vector<IValue*>                 m_value_repo;
        size_t                          m_item_count;
};
#endif
