#include <string>
#include <fstream>
#include <set>
#include <iostream>

#include "ac_automachine_dict.h"
#include "log.h"
#include "util.h"

AcAutomachineDict::AcAutomachineDict() : m_ac_automation (NULL) {
    m_ac_automation = ac_automata_init();
}

AcAutomachineDict::AcAutomachineDict(const std::string& dict_data_path) : m_dict_data_path(dict_data_path), m_ac_automation(NULL) {

}

AcAutomachineDict::~AcAutomachineDict() {
    if(NULL != m_ac_automation) {
        ac_automata_release(m_ac_automation);
        m_ac_automation = NULL;
    }
}

int AcAutomachineDict::Get(const std::string& key, void* value) {

    log (LOG_DEBUG, "%s:%d[tid:%lld]\tAcAutomachineDict::Get key:%s.", __FILE__, __LINE__, pthread_self(), key.c_str());
    std::set<std::string> match_word;
    AC_TEXT_t ac_text;

    ac_text.astring = key.c_str();
    ac_text.length = key.length();

    ac_automata_settext(m_ac_automation, &ac_text, 0);

    AC_MATCH_t* ac_match;
    while(ac_match = ac_automata_findnext(m_ac_automation)) {
        for(int i = 0; i < ac_match->match_num; ++i) {
            if(ac_match->patterns[i].rep.number < m_words.size()) {
                match_word.insert(m_words[ ac_match->patterns[i].rep.number ]);
                log (LOG_DEBUG, "%s:%d[tid:%lld]\tAcAutomachineDict::Get key:%s hit:%s.", __FILE__, __LINE__, pthread_self(), key.c_str(), m_words[ ac_match->patterns[i].rep.number ].c_str());
            }
        }
    }
    if(NULL != value) {
        ((std::set<std::string>*)value)->swap(match_word);
    }
    return 0;
}

int AcAutomachineDict::Del(const std::string& key) {

    return 0;
}

int AcAutomachineDict::Set(const std::string& key, const std::string& value) {

    return 0;
}

int AcAutomachineDict::Load(const std::string& dict_data_path) {

    std::ifstream in_fs(dict_data_path.c_str());

    if(!in_fs.is_open()) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tAcAutomachineDict::Load dict_data_path:%s does not exist.", __FILE__, __LINE__, pthread_self(), dict_data_path.c_str());
        return 1;
    }

    std::string                 line_str;
    std::vector<std::string>    words;

    while(in_fs >> line_str) {
        if(line_str.size() <= 0) {
            continue; 
        }
        std::vector<std::string> split_line = StringToTokens(line_str, false, '|');

        FOR_EACH(itr, split_line) {
            words.push_back(*itr);

        }

        line_str.clear();
    }
    return BuildAcMation(words);
}

int AcAutomachineDict::BuildAcMation(const std::vector<std::string>& words) {
    FOR_EACH(itr, words) {
        AC_PATTERN_t ac_pattern;
        ac_pattern.rep.number   = m_words.size();
        ac_pattern.astring      = itr->c_str();
        ac_pattern.length       = itr->length();
        if(ACERR_SUCCESS != ac_automata_add(m_ac_automation, &ac_pattern)) {
            log (LOG_DEBUG, "%s:%d[tid:%lld]\tAcAutomachineDict::BuildAcMation word (%s) in dict_data_path build fail.", __FILE__, __LINE__, pthread_self(), itr->c_str());
        }
        else{
            m_words.push_back(*itr);
            log (LOG_DEBUG, "%s:%d[tid:%lld]\tAcAutomachineDict::BuildAcMation word (%s) in dict_data_path build success, position:%d.", __FILE__, __LINE__, pthread_self(), itr->c_str(),m_words.size());
        }
    }
    ac_automata_finalize(m_ac_automation);

    return 0;
}

int AcAutomachineDict::Dump() {


    return 0;
}
