#ifndef __DICTS_DICT_FACTORY__
#define __DICTS_DICT_FACTORY__

#include "singleton.h"
#include "dict.h"

class CDictFactory : public Singleton<CDictFactory> {
    public:
        CDictFactory();
        CDictFactory(const std::string& dict_class_list_str);
        ~CDictFactory();

        IDict* GenDictInstance(const std::string& type);
    private:
        std::string         m_dict_class_list_str;
};

#endif
