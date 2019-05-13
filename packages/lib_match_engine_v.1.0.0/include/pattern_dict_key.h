#ifndef __DICTS_PATTERN_DICT_KEY__
#define __DICTS_PATTERN_DICT_KEY__

#include "key.h"

class PatternDictKey : public IKey {
  public:    
        PatternDictKey(){}
        PatternDictKey(const std::string& line) {
        
        }
        int Init(const void* input){
            return 0; 
        }
        int Key(void* output) const{
            return 0; 
        }
        int ToString(void* output) const {
            return 0; 
        }
        int Compare(const IKey& k) const {
            return 0; 
        }
        int Func(const void* input, void* output) {
            return 0; 
        }

};

#endif
