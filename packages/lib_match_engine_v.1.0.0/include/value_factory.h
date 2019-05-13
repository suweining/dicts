#ifndef __DICTS_VALUE_FACTORY__
#define __DICTS_VALUE_FACTORY__

#include <string>
#include "singleton.h"
#include "value.h"

class CValueFactory : public Singleton<CValueFactory> {
    public:
        CValueFactory();
        CValueFactory(const std::string& value_class_list_str);
        ~CValueFactory();

        int SetValueClassList(const std::string& value_class_list_str);
        IValue* GenValueInstance(const std::string& type);

    private:
        std::string         m_value_class_list_str;
};
#endif
