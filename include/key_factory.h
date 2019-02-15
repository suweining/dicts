#ifndef __DICTS_KEY_FACTORY__
#define __DICTS_KEY_FACTORY__

#include "singleton.h"


class CKeyFactory : public Singleton<CKeyFactory> {

    public:
        CKeyFactory();
        ~CKeyFactory();

        int GenKeyInstance(const std::string& type);
};
#endif
