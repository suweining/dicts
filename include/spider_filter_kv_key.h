#ifndef __DICTS_SPIDER_FILTER_KV_KEY__
#define __DICTS_SPIDER_FILTER_KV_KEY__

#include <string>
#include "key.h"

class CSpiderFilterKvKey : public IKey {

    public:
        CSpiderFilterKvKey();
        ~CSpiderFilterKvKey();

        int Init(const void* input);
        int SetKey(const void* input);
        int GetKey(void* output) const;
        int ToString(void* output) const;
        int Compare(const IKey& key) const;
        int Func(const void* input, void* output);
    
    private:
        int GetKeyBuildDict(void* output) const;
        int GetKeyQueryDict(void* output) const;

    private:
        std::string m_key;
        std::string m_key_build_dict;
        std::string m_key_query_dict;
};

#endif
