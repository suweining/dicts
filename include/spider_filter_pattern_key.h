#ifndef __DICTS_SPIDER_FILTER_PATTERN_KEY__
#define __DICTS_SPIDER_FILTER_PATTERN_KEY__

#include <string>
#include "key.h"

class CSpiderFilterPatternKey : public IKey {

    public:
        CSpiderFilterPatternKey();
        ~CSpiderFilterPatternKey();

        int Init(const void* input);
        int SetKey(const void* input);
        int GetKey(void* output) const;
        int ToString(void* output) const;
        int Compare(const IKey& key) const;
        int Func(const void* input, void* output);

    private:
        int genKey(const std::string& key_string);
    private:
        std::string m_key;
        std::string m_key_string;
};
#endif
