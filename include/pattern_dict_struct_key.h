#ifndef __DICTS_PATTERN_DICT_STRUCT_KEY__
#define __DICTS_PATTERN_DICT_STRUCT_KEY__

#include "key.h"

class PatternDictStructKey : public IKey {

    public:
        PatternDictStructKey();
        ~PatternDictStructKey();

        int Init(const void* input);
        int Key(void* output) const;
        int ToString(void* output) const;
        int Compare(const IKey& key) const;
        int Func(const void* input, void* output);

    private:
        std::string m_key;
        std::string m_string;
};

#endif
