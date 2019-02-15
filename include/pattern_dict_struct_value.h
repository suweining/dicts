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
        int GetVal(void* output);
        int SetVal(const void* input);
        int ToString(void* output);
        int Compare(const IValue& v);
        int Func(const void* input, void* output);

    private:
        std::string                         m_value; // json
        std::string                         m_string;
        std::map<std::string, std::string>  m_fields;

};
#endif
