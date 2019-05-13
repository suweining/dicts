#ifndef __DICTS_PATTERN_DICT_STRUCT_VALUE__
#define __DICTS_PATTERN_DICT_STRUCT_VALUE__

#include <map> 
#include <string>
#include <vector>

#include "value.h"

class PatternDictStructValue : public IValue {

    public:
        PatternDictStructValue();
        PatternDictStructValue(const std::string& line);
        ~PatternDictStructValue();

        int Init(const void* input);
        int GetVal(void* output) const;
        int SetVal(const void* input);
        int ToString(void* output) const;
        int Compare(const IValue& v) const;
        int Func(const void* input, void* output);

    private:
        std::string                         m_value; // json
        std::string                         m_string;
        std::map<std::string, std::string>  m_fields;

};
#endif
