#include <string.h>
#include <fstream>
#include <set>
#include <map>

#include "util.h"
#include "ac_matcher.h"
#include "log.h"

AcMatcherDict::AcMatcherDict() : m_ac_automachine_dict(NULL) {
    m_ac_automachine_dict = new AcAutomachineDict();
}

AcMatcherDict::~AcMatcherDict() {
    if(NULL != m_ac_automachine_dict) {
        delete m_ac_automachine_dict; 
    }
}

int AcMatcherDict::Load(const std::string& dict_data_path) {
    if(m_ac_automachine_dict->Load(dict_data_path)) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tAcMatcherDict::Load  AcAutomachineDict::Load Fail.", __FILE__, __LINE__, pthread_self());
        return 1;
    }


    std::ifstream in_fs(dict_data_path.c_str());

    if(!in_fs.is_open()) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tAcMatherDict::Load dict_data_path:%s does not exist.", __FILE__, __LINE__, pthread_self(), dict_data_path.c_str());
        return 2;
    }
    std::string                 line_str;
    int                         line_num = 0;

    while(std::getline(in_fs, line_str)) {
        if(line_str.size() <= 0) {
            continue; 
        }
        std::vector<std::string> split_line = StringToTokens(line_str, false, '|');
        log (LOG_NOTICE, "%s:%d[tid:%lld]\tAcMatherDict::Load line:%s key.size:%d.", __FILE__, __LINE__, pthread_self(), line_str.c_str(), split_line.size());

        m_line_wordcount.push_back(split_line.size());

        FOR_EACH(itr, split_line) {
            m_word_inlines[*itr].push_back(line_num);
            log (LOG_NOTICE, "%s:%d[tid:%lld]\tAcMatherDict::Load word:%s inline:%d.", __FILE__, __LINE__, pthread_self(), itr->c_str(), line_num);
        }

        ++ line_num;
        m_lines.push_back(line_str);
        line_str.clear();
    }

    return 0;
}

int AcMatcherDict::Get(const std::string& key, void* value) {

    std::set<std::string> val;
    if(m_ac_automachine_dict->Get(key, &val)) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tAcMatcherDict::Get:%s m_ac_automachine_dict->Get fail.", __FILE__, __LINE__, pthread_self(), key.c_str());
        return 1;
    }

    size_t len = m_line_wordcount.size();
    size_t* line_wordcount = new size_t[len];

    memset(line_wordcount, 0, len);

    FOR_EACH(itr, val) {
        std::vector<int>* inlines = &m_word_inlines[*itr];
        size_t l = inlines->size();
        for(size_t i = 0; i < l; ++i) {
            line_wordcount[inlines->at(i)] ++;
        }
    }

    size_t i;
    for( i = 0; i < len; ++i) {
        if(line_wordcount[i] == 0) {
            continue;
        }
        if(m_line_wordcount[i] == line_wordcount[i]) {
            break;
        }
    }
    if(i < len) {
        // hit 
        *(std::string*)value = m_lines[i];  
    }
    else {
        // miss 
    }

    delete [] line_wordcount;
    return 0;
}

int AcMatcherDict::Del(const std::string& key) {
    return 0;
}

int AcMatcherDict::Set(const std::string& key, const std::string& value) {

       std::vector<std::string> split_key = StringToTokens(key, false, '|');

        m_line_wordcount.push_back(split_key.size());

        size_t line_num = m_line_wordcount.size() - 1;

        FOR_EACH(itr, split_key) {
            m_word_inlines[*itr].push_back(line_num);

            if(m_ac_automachine_dict->Set(*itr,"")) {
                log (LOG_WARNING, "%s:%d[tid:%lld]\tAcMatcherDict::Set AcAutomachineDict::Set word:%s.", __FILE__, __LINE__, pthread_self(), itr->c_str());
            }
        }
        return 0;
}

int AcMatcherDict::Dump() {
    return 0;
}

