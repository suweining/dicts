#ifndef __DICTS_PATTERN_DICT_VALUE__
#define __DICTS_PATTERN_DICT_VALUE__

class PatternDictValue: public IValue {
    public:
        PatternDictValue() {}
        PatternDictValue(const std::string& line) {}
        int Init(const void* input) {return 0;}
        int Val(void* output) {return 0;}
        int ToString(void* output) {return 0;}
        int Compare(const IValue& v) {return 0;}
        int Func(const void* input, void* output) {
            return 0; 
        }

};
#endif
