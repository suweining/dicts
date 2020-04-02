#ifndef __DICTS_KV_DICT_COMMON_KEY__
#define __DICTS_KV_DICT_COMMON_KEY__

#include <string>
#include "key.h"

class CKvDictCommonKey : public IKey {
  public:
        CKvDictCommonKey();
        ~CKvDictCommonKey();
        int Init(const void* input);
        int SetKey(const void* input);
        int GetKey(void* output) const;
        int ToString(void* output) const;
        int Compare(const IKey& k) const;
        int Func(const void* input, void* output);
  private:
        std::string m_key;
        // just for the key is diff while query or build dict
        std::string m_key_build_dict;
        std::string m_key_query_dict;
};

#endif
