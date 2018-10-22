#ifndef __AC_AUTOMATION_DICT__
#define __AC_AUTOMATION_DICT__

#include <vector>
#include <map>

#include "dict.h"
#include "ahocorasick.h"

class AcAutomachineDict : public Dict {
    public:
        AcAutomachineDict();
        AcAutomachineDict(const std::string& dict_data_path);
        ~AcAutomachineDict();
        int Del(const std::string& key);
        int Del() {return 0;}
        int Get(const std::string& key, void* value);
        int Set(const std::string& key, const std::string& value = "");
        int Finalize() {return 0;}
        int Load(const std::string& dict_data_path);
        int Dump();

    private:
        int BuildAcMation(const std::vector<std::string>& words);

    private:
        std::string                                     m_dict_data_path; 
        AC_AUTOMATA_t*                                  m_ac_automation;
        std::vector<std::string>                        m_words;
        std::vector<int>                                m_line_wordcount;
        std::map<std::string, std::vector<int> >        m_word_inlines;


};

#endif
