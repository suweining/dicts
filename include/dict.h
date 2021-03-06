#ifndef __DICTS_INTERFACE__
#define __DICTS_INTERFACE__

#include <string>
#include <vector>
#include <memory>

#include "key.h"
#include "value.h"

class IDict {
    public:
        virtual int Init(const std::string& params) = 0; // init dict
        virtual int Set(const IKey& key, const IValue& value) = 0; // use for update value while dict has build
        virtual int Add(const IKey& key, const IValue& value) = 0; // add record for building the dict
        virtual int Del(const IKey& key) = 0; // del record info in the dict
        virtual int Get(const IKey& key, std::vector<IValue*>* value) = 0; // match key
        virtual int Dump(const std::string& dict_data_dump_path) = 0; // write the record into file
        virtual int Finalize() = 0; // building the dict while all record add into mem
        virtual int Info(std::string* info) = 0; // info for dict

        virtual int Clear() = 0; // clear dict 
        virtual ~IDict() {}
};
#endif
