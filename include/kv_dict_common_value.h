#ifndef __DICTS_KV_DICT_COMMON_VALUE__
#define __DICTS_KV_DICT_COMMON_VALUE__

#include <vector>
#include <string>

#include "value.h"

class CKvDictCommonValue : public IValue {
    public: 
        CKvDictCommonValue();
        ~CKvDictCommonValue();

        int Init(const void* input);
        int GetVal(void* output) const;
        int SetVal(const void* input);
        int ToString(void* output) const;
        int Compare(const IValue& v) const;
        int Func(const void* input, void* output);

    private:
        std::string                 m_value;
        std::string                 m_string;
        std::vector<std::string>    m_fields; 
};

#endif
