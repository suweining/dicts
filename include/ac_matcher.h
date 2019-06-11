#ifndef _AC_MATCHER_
#define _AC_MATCHER_

#include "ac_automachine_dict.h"
#include "dict.h"

class AcMatcherDict : public Dict {
    public:
        AcMatcherDict();
        ~AcMatcherDict();
        int Del(const std::string& key);
        int Get(const std::string& key, void* value);
        int Set(const std::string& key, const std::string& value);
        int Load(const std::string& dict_data_path);
        int Dump();

    private:
        AcAutomachineDict*                              m_ac_automachine_dict;
        std::vector<int>                                m_line_wordcount;
        std::map<std::string, std::vector<int> >        m_word_inlines;
        std::vector<std::string>                        m_lines;

};
#endif
