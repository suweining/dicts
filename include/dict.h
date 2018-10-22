#ifndef __DICTS_INTERFACE__
#define __DICTS_INTERFACE__

#include <string>

#include "key.h"
#include "value.h"

class Dict {
    public:
        virtual int Set(const std::string& key, const std::string& value) = 0;
        virtual int Del(const std::string& key) = 0;
        virtual int Del() = 0;
        virtual int Finalize() = 0;
        virtual int Get(const std::string& key, void* value) = 0;
        virtual int Load(const std::string& dict_data_path) = 0;
        virtual int Dump() = 0;
        virtual ~Dict() {}
};

class IDict {
    public:
        virtual int Set(const IKey& key, const IValue& value) = 0;
        virtual int Del(const IKey& key) = 0;
        virtual int Get(const IKey& key, IValue* value) = 0;
        virtual int Load(const std::string& dict_data_load_path) = 0;
        virtual int Dump(const std::string& dict_data_dump_path) = 0;
        virtual int Finalize() = 0;

        virtual int Clear() = 0;
        virtual ~IDict() {}

};
#endif
