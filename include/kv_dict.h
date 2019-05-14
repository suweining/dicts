#ifndef __DICTS_KV_DICT__
#define __DICTS_KV_DICT__

#include <string>
#include <vector>
#include "dict.h"
#include "hash_dict.h"

class CKvDict : public IDict {

    public:
        CKvDict();
        ~CKvDict(); 

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

        #define VALUE_TYPE int64_t 

        struct Record {
            IKey*   key;
            IValue* value;
            Record() : key(NULL), value(NULL) {
            
            }
            ~Record() {
                if(NULL != key) {
                    delete key;
                    key = NULL;
                } 

                if(NULL != value) {
                    delete value;
                    value = NULL;
                }
            }
        };

        HashDict<VALUE_TYPE>            m_hash_dict_engine;
        vector<Record*>                  m_record_repo;
        size_t                          m_item_count;
        size_t                          m_hash_capacity_init;
};
#endif
