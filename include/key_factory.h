#ifndef __DICTS_KEY_FACTORY__
#define __DICTS_KEY_FACTORY__

#include "singleton.h"
#include "key.h"


class CKeyFactory : public Singleton<CKeyFactory> {

    public:
        CKeyFactory();
        CKeyFactory(const std::string& key_class_list_str);
        ~CKeyFactory();

        int SetKeyClassList(const std::string& key_class_list_str);
        IKey* GenKeyInstance(const std::string& type);

    private:
        int GenKeyClassPool();

    private:
        std::string                     m_key_class_list_str;
        std::vector<std::string>        m_key_class_list;

};
#endif
