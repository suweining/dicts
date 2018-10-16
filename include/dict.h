#ifndef __DICTS_INTERFACE__
#define __DICTS_INTERFACE__

#include <string>

class Dict {
    public:
        virtual int Set(const std::string& key, const std::string& value) = 0;
        virtual int Del(const std::string& key) = 0;
        virtual int Get(const std::string& key, void* value) = 0;
        virtual int Load(const std::string& dict_data_path) = 0;
        virtual int Dump() = 0;
};
#endif
