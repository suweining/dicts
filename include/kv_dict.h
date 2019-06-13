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

        #define VALUE_TYPE int64_t 

        struct Record {
            std::shared_ptr<IKey>   key;
            std::shared_ptr<IValue> value;
            Record() : key(NULL), value(NULL) {
            }
            ~Record() {

            }
        };

        HashDict<VALUE_TYPE>            m_hash_dict_engine;
        vector<Record>                  m_record_repo;
        size_t                          m_item_count;
        size_t                          m_hash_capacity_init;
        size_t                          m_capacity;
};
#endif
