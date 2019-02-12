#ifndef __DICTS_SPIDER_FILTER_KV_VALUE__
#define __DICTS_SPIDER_FILTER_KV_VALUE__

#include <vector>
#include <string>

#include "value.h"

class CSpiderFilterKvValue : public IValue {
    public: 
        CSpiderFilterKvValue();
        ~CSpiderFilterKvValue();

        int Init(const void* input);
        int GetVal(void* output);
        int SetVal(const void* input);
        int ToString(void* output);
        int Compare(const IValue& v);
        int Func(const void* input, void* output);

    private:
        std::string                 m_value;
        std::string                 m_string;
        std::vector<std::string>    m_fields; 
};

#endif
