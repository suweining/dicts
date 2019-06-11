/**********************************************************************
*  _____             _         _____      _   _
* |  __ \           | |       |  __ \    | | | |
* | |  | | __ _ _ __| |_ ___  | |__) |_ _| |_| |_ ___ _ __ _ __
* | |  | |/ _` | '__| __/ __| |  ___/ _` | __| __/ _ \ '__| '_ \
* | |__| | (_| | |  | |_\__ \ | |  | (_| | |_| ||  __/ |  | | | |
* |_____/ \__,_|_|   \__|___/ |_|   \__,_|\__|\__\___|_|  |_| |_|
*
*********************************************************************/



#ifndef DARTS_PATTERN_H_INCLUDED
#define DARTS_PATTERN_H_INCLUDED

/*************************************************************************
* Darts Pattern
* Author: NiuBen
* Email: niuben@360.cn
* Creating Time:2014/09/23
* Update Time:2015/08/19
* News:
*************************************************************************/
/*
    if you just use for  converting pc url to mobile url,
    just use create(), save(), load(), and pcToMobile().

    to see more information when running , please setDebug(true).

    in this vision, the number of prefix(pattern's ) have a limit of 20,000,000 approximately.

*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "darts.h"
#include <pcre.h>

#include "re2/set.h"

#define SUBSTITUTE_SIZE 100       ///  size of ovector when use pcre_exec()
#define IN_BLACK_LIST 0           /// in black list
const std::size_t DFA_MAX_MEM = 2L * 1024L * 1024L * 1024L - 1L;

typedef Darts::DoubleArrayImpl<char, unsigned char, long, unsigned long> DartsDatrie;

class DartsPattern {
public:


    /// store pattern information
    struct patternInfo{

        patternInfo(): pc_pat(NULL), pc_length(0), pc_weight(1), compiled_re(NULL), re_length(0), mob_pat(NULL), mob_length(0) {}
        ~patternInfo() {}


        ///for descending order
        bool operator <(const patternInfo& pi) const{
            return pc_weight > pi.pc_weight;
        }

        const char * pc_pat;
        std::size_t pc_length;
        int pc_weight;
        const pcre * compiled_re;
        std::size_t re_length;
        const char* mob_pat;
        std::size_t mob_length;

    };

    /// to store compiled regexs and mobile patterns
    struct patternGroup{
        patternGroup(): num(0), size(sizeof(std::size_t)), pattern_dfa(NULL) {}

        ~patternGroup(){
        };

        /// one
        /// substitute from pc-url to mobile-url
        bool substitute(const std::string &_pc_url, std::string &ans, std::size_t pos = 0, std::string *matchedPat = NULL) {
            std::string pc_url = _pc_url.substr(pos, _pc_url.length() - pos);

            std::vector<int> found_ids;
            /// dfa find process
            if(pattern_dfa != NULL) {
                pattern_dfa->Match(pc_url, &found_ids);
                // complement
                for(std::size_t i = 0; i < pattern_dfa_complement.size(); ++i) {
                    found_ids.push_back(pattern_dfa_complement[i]);
                }
                std::sort(found_ids.begin(), found_ids.end());
            }else {
                for(int i = 0; i < num; ++i) {
                    found_ids.push_back(i);
                }
            }

            for(std::size_t i = 0; i < found_ids.size(); ++i) {
//                if(pattern_info[i].pc_weight == IN_BLACK_LIST)  continue;  ///  black
//                std::string mobilePattern = pattern_info[i].mob_pat;
                std::string mobilePattern = hostReversal2(pattern_info[found_ids[i]].mob_pat);
                int vec_pos[SUBSTITUTE_SIZE];
                int br = pcre_exec(pattern_info[found_ids[i]].compiled_re, NULL, pc_url.c_str(), pc_url.length(), 0, PCRE_ANCHORED, vec_pos, SUBSTITUTE_SIZE);
                std::vector<std::string> vec;

                if(br == 0) {
                    std::cerr << "SUBSTITUTE_SIZE is too little." << std::endl;
                }

                for(int k = 0; k < br; k++) {
                    int vec_begin = vec_pos[2 * k];
                    int vec_length = vec_pos[2 * k + 1] - vec_pos[2 * k];
                    std::string ins = (vec_length == 0 ? std::string(""):pc_url.substr(vec_begin, vec_length));
                    vec.push_back(ins);

                }

                if(vec.size() > 0) {
                    std::size_t sub_pos = 0;

                    while((sub_pos = mobilePattern.find('\\', sub_pos)) != std::string::npos) {
                        int j = mobilePattern.at(sub_pos + 1) - '0';
                        if(j < vec.size()) {
                            mobilePattern.replace(sub_pos, 2, vec[j]);
                            sub_pos = std::min(mobilePattern.length(), sub_pos + vec[j].length());
                        }
                        else
                            sub_pos = std::min(mobilePattern.length(), sub_pos + 2);

                    }
//                    for(std::size_t j = 1; j < vec.size(); ++j) {
//                        char tmp_sub[32] = "\\";
//                        sprintf(tmp_sub + 1, "%d", j);
//                        std::string sub(tmp_sub);
//                        std::size_t tmp_sub_pos;
//                        if((tmp_sub_pos = mobilePattern.find(sub, sub_pos)) != std::string::npos) {
//                            mobilePattern.replace(tmp_sub_pos, sub.length(), vec[j]);
//                            sub_pos = std::min(mobilePattern.length(), tmp_sub_pos + vec[j].length());
//                        }
//
//                    }
                    ans = mobilePattern;
                    if(matchedPat != NULL)  {
                        char tmp_s[70] = "`";
                        sprintf(tmp_s + 1, "%d", pattern_info[found_ids[i]].pc_weight);
                        *matchedPat = std::string(pattern_info[found_ids[i]].pc_pat) + std::string("`") + std::string(pattern_info[found_ids[i]].mob_pat) + std::string(tmp_s);

                    }

                    return true;
                }
            }
            if(matchedPat != NULL) matchedPat = NULL;
            return false;
        }

        /// all
        /// substitute from pc-urls to mobile-urls
        bool substitute(const std::string &_pc_url,  std::vector<std::pair<std::string, std::string> > &ans, std::size_t pos = 0) {
            std::string pc_url = _pc_url.substr(pos, _pc_url.length() - pos);
            bool return_code = false;

            std::vector<int> found_ids;
            /// dfa find process
            if(pattern_dfa != NULL) {
                pattern_dfa->Match(pc_url, &found_ids);
                // complement
                for(std::size_t i = 0; i < pattern_dfa_complement.size(); ++i) {
                    found_ids.push_back(pattern_dfa_complement[i]);
                }
                std::sort(found_ids.begin(), found_ids.end());
            }else {
                for(int i = 0; i < num; ++i) {
                    found_ids.push_back(i);
                }
            }

            for(std::size_t i = 0; i < found_ids.size(); ++i)  {
//                if(pattern_info[i].pc_weight == IN_BLACK_LIST)  continue;  ///  black
//                std::string mobilePattern = pattern_info[i].mob_pat;
                std::string mobilePattern = hostReversal2(pattern_info[found_ids[i]].mob_pat);
                int vec_pos[SUBSTITUTE_SIZE];
                int br = pcre_exec(pattern_info[found_ids[i]].compiled_re, NULL, pc_url.c_str(), pc_url.length(), 0, PCRE_ANCHORED, vec_pos, SUBSTITUTE_SIZE);
                std::vector<std::string> vec;


                if(br == 0) {
                    std::cerr << "SUBSTITUTE_SIZE is too little." << std::endl;
                }


                for(int k = 0; k < br; k++) {
                    int vec_begin = vec_pos[2 * k];
                    int vec_length = vec_pos[2 * k + 1] - vec_pos[2 * k];
                    std::string ins = (vec_length == 0 ? std::string(""):pc_url.substr(vec_begin, vec_length));
                    vec.push_back(ins);

                }

                if(vec.size() > 0) {
                    std::size_t sub_pos = 0;

                    while((sub_pos = mobilePattern.find('\\', sub_pos)) != std::string::npos) {
                        int j = mobilePattern.at(sub_pos + 1) - '0';
                        if(j < vec.size()) {
                            mobilePattern.replace(sub_pos, 2, vec[j]);
                            sub_pos = std::min(mobilePattern.length(), sub_pos + vec[j].length());
                        }
                        else
                            sub_pos = std::min(mobilePattern.length(), sub_pos + 2);

                    }
                    char tmp_s[70] = "`";
                    sprintf(tmp_s + 1, "%d", pattern_info[found_ids[i]].pc_weight);
                    std::string matchedPat = std::string(pattern_info[found_ids[i]].pc_pat) + std::string("`") + std::string(pattern_info[found_ids[i]].mob_pat) + std::string(tmp_s);


                    ans.push_back(std::make_pair(mobilePattern, matchedPat));
                    return_code = true;
                }
            }
            return return_code;
        }


        std::size_t num;            /// should be equal to pattern_info.size()
        std::size_t size;
        std::vector<patternInfo> pattern_info;

        re2::RE2::Set *pattern_dfa;
        std::vector<int> pattern_dfa_complement;
//    private:

        /// create DFA
        bool makeOneDFA(const re2::RE2::Options &dfa_ropt) {
            #ifdef _DEBUG
            printf("------------------makeOneDFA()------------------\n");
            #endif // _DEBUG


//            static re2::RE2::Options dfa_ropt;
//            dfa_ropt.set_case_sensitive(false);
//            dfa_ropt.set_never_capture(true);
//            dfa_ropt.set_max_mem(2 * 1024 * 1024 * 1024 - 1);
            if(pattern_info.size() == 0)
                return false;

            std::string prefix = DartsPattern::hostReversal(pattern_info[0].pc_pat);
            std::size_t end_prefix = DartsPattern::lastNotOfRegex(prefix, pattern_info[0].pc_weight);

//            pattern_dfa = new re2::RE2::Set(dfa_ropt, RE2::ANCHOR_BOTH);
            pattern_dfa_complement.clear();
            pattern_dfa = new re2::RE2::Set(dfa_ropt, RE2::ANCHOR_START);
            bool dfa_add_error = false;
            for(std::size_t i = 0; i < pattern_info.size(); ++i) {
                std::string prefix_i = DartsPattern::hostReversal(pattern_info[i].pc_pat);
                int inserted_id =  pattern_dfa->Add(std::string("^") + prefix_i.substr(std::min(end_prefix + 1, prefix_i.length())), NULL);
                if(inserted_id == -1) {
                    inserted_id = pattern_dfa->Add("^YOUWILLNEVERMATCHTHISPATTERN~BY~NB$", NULL);
                    pattern_dfa_complement.push_back(i);
                }
                if(inserted_id != i) {
                    dfa_add_error = true;
                    break;
                }

            }

            #ifdef _DEBUG
            printf("DFA Patterns:\n");
            for(std::size_t i = 0; i < pattern_info.size(); ++i) {
                std::string prefix_i = DartsPattern::hostReversal(pattern_info[i].pc_pat);
                std::cout << "\t" << prefix_i <<"\t" << std::string("^") + prefix_i.substr(std::min(end_prefix + 1, prefix_i.length())) << std::endl;

            }
            #endif // _DEBUG

            if(dfa_add_error || !pattern_dfa->Compile()) {
                std::cerr << "ERROR: makeOneDFA failed with the first: " << prefix << ", dfa_add_error: " << dfa_add_error << std::endl;
                if(pattern_dfa != NULL) {
                    delete pattern_dfa;
                    pattern_dfa = NULL;
                }
                pattern_dfa_complement.clear();

            }
            return true;


        }

        /// http://\1.\2.blog.163.com/blog/static/\3/ -> http://\2.\1.blog.163.com/blog/static/\3/
        std::string hostReversal2(const std::string &pat) {
            std::size_t pos_0, pos_1, end_host, len = pat.length();
            std::vector<std::string> pat_part;
            bool isUrl = true;
            std::string point = isUrl? ".": "\\.";
            std::size_t offset = isUrl? 1: 2;

            pos_0 = pat.find("//") + 2;
            if((end_host = pat.find("/", pos_0)) == std::string::npos) {
                end_host = (pat[len - 1] == '$'? len - 1: len);
            }
            std::string host = pat.substr(0, end_host);
            std::string ans = pat.substr(0, pos_0);
            while((pos_1 = host.find(point, pos_0)) != std::string::npos) {
                pat_part.push_back(pat.substr(pos_0, pos_1 - pos_0));
                pos_0 = pos_1 + offset;
            }
            pat_part.push_back(host.substr(pos_0, host.length() - pos_0));

            std::stack<std::string> pat_stack;
            for(std::size_t i = 0 ; i < pat_part.size(); ++i) {
                if(pat_part[i].find("\\") != std::string::npos) {
                    pat_stack.push(pat_part[i]);
                    pat_part[i].clear();
                }
            }
            for(std::size_t i = 0 ; i < pat_part.size(); ++i) {
                if(pat_part[i].empty() && !pat_stack.empty()) {
                    pat_part[i] = pat_stack.top();
                    pat_stack.pop();
                }
            }


            if(pat_part.size() > 0) ans += pat_part[0];
            for(std::size_t i = 1 ; i < pat_part.size(); ++i) {
                ans += point + pat_part[i];
            }

            /// [^.] -> [^./]
            //        if(isUrl == false) {
            //            std::size_t pos_adjust = ans.find("^.");
            //            if(pos_adjust != std::string::npos) ans.replace(pos_adjust, 2, "^./");
            //        }
            ans += pat.substr(end_host, len - end_host);
            return ans;

        }
    };


    DartsPattern(): dat_offset(0), dat_size(0), addr(NULL), dfa_limit(-1), debug(false) { }
    ~DartsPattern() {
        delete_mmap();
        deleteDFA();
    }

    /// set debug = true, if you want to see more information
    void setDebug(bool d) {
        debug = d;
    }


    /// wirte regex-part to binfile and create datrie
    bool create(const char* input, const char* output) {

        std::map<std::string, patternGroup> tmpMap;
        std::size_t pat_num = preProcess(input, tmpMap);

        std::vector<const char *> keys(pat_num);
        std::vector<std::size_t> lengths(pat_num);
        std::vector<DartsDatrie::value_type> values(pat_num);
        std::vector<patternGroup> regexs(pat_num);
        std::vector<stringInfo> regexs_info(pat_num);

        std::size_t key_id = 0, tmp_offset = sizeof(std::size_t) * 2;
        std::map<std::string, patternGroup>::iterator it;
        for(it = tmpMap.begin(); it != tmpMap.end(); it++) {
            keys[key_id] = it->first.c_str();
            lengths[key_id] = it->first.length();
            values[key_id] = static_cast<DartsDatrie::value_type>(key_id);

            std::sort(it->second.pattern_info.begin(), it->second.pattern_info.end());  ///  sort by weight
            regexs[key_id] = it->second;
            stringInfo tmp_info;
            tmp_info.length = it->second.size;
            tmp_info.offset = tmp_offset;
            tmp_offset += tmp_info.length;
            regexs_info[key_id] = tmp_info;
            ++key_id;

        }

        dat_size = key_id;
        dat_offset = tmp_offset + sizeof(stringInfo) * dat_size;

        if(debug) {
            printf("---------- create() ----------\n");
            printf("keys.size(): %zu\n",keys.size());
            printf("lengths.size(): %zu\n",lengths.size());
            printf("values.size(): %zu\n",values.size());
            printf("regexs.size(): %zu\n",regexs.size());
            printf("regexs_info.size(): %zu\n",regexs_info.size());
            printf("dat_size: %zu\n",dat_size);
            printf("dat_offset: %zu\n",dat_offset);
            printf("tmp_offset: %zu\n",tmp_offset);
        }

        /// write compiled regexs to bin file
        std::ofstream fout(output);
        fout.write((const char*)&dat_size, sizeof(dat_size));
        fout.write((const char*)&dat_offset, sizeof(dat_offset));
        fout.write((const char*)&regexs_info[0], sizeof(stringInfo) * regexs_info.size());
        for(std::size_t i = 0; i < dat_size; ++i) {
            fout.write((const char*)&regexs[i].num, sizeof(regexs[i].num));
            for(std::size_t j = 0; j < regexs[i].num; ++j) {

                fout.write((const char*)&regexs[i].pattern_info[j].pc_length, sizeof(regexs[i].pattern_info[j].pc_length));
                fout.write(regexs[i].pattern_info[j].pc_pat, regexs[i].pattern_info[j].pc_length);
                fout.write((const char*)&regexs[i].pattern_info[j].pc_weight, sizeof(regexs[i].pattern_info[j].pc_weight));
                fout.write((const char*)&regexs[i].pattern_info[j].re_length, sizeof(regexs[i].pattern_info[j].re_length));
                fout.write((const char*)regexs[i].pattern_info[j].compiled_re, regexs[i].pattern_info[j].re_length);
                fout.write((const char*)&regexs[i].pattern_info[j].mob_length, sizeof(regexs[i].pattern_info[j].mob_length));
                fout.write(regexs[i].pattern_info[j].mob_pat, regexs[i].pattern_info[j].mob_length);


                pcre_free((pcre *)regexs[i].pattern_info[j].compiled_re);
                delete [] regexs[i].pattern_info[j].mob_pat;
                delete [] regexs[i].pattern_info[j].pc_pat;
            }

        }

        if(dat_offset != fout.tellp()) {
            std::cerr << "save regexs failed." << std::endl;
        }
        fout.close();

        try{
            datrie.build(keys.size(), &keys[0], &lengths[0], &values[0]);

            DartsDatrie::value_type value;
            DartsDatrie::result_pair_type result;

        }catch (const std::exception &ex) {
            std::cerr << "exception: " << ex.what() << std::endl;
            throw ex;
        }

        return true;
    }

    /// save datrie to the binfile
    bool save(const char* output) {
        try {
            if(dat_offset == 0)  {
                std::cerr << "the datrie have not be created." << std::endl;
                return false;
            }

            if(datrie.save(output, "a+b") != 0)  {
                std::cerr << "save datrie failed." << std::endl;
                return false;
            }
        }catch (const std::exception &ex) {
            std::cerr << "exception: " << ex.what() << std::endl;
            throw ex;
        }
        return true;

    }

    /// load binfile
    ///dfa_limit:
    /// if(-1) don't create DFA; else create DFA && pat_num>=dfa_limit
    bool load(const char* input, const int _dfa_limit = -1) {
        ///
        dfa_limit = _dfa_limit;

        delete_mmap();
        try {
            std::ifstream fin(input);
            fin.read((char *)&dat_size, sizeof(dat_size));
            fin.read((char *)&dat_offset, sizeof(dat_offset));
            fin.close();

            if(!create_mmap(input)) return false;
            if(datrie.open(input, "rb", dat_offset) != 0)  {
                std::cerr << "open datrie failed." << std::endl;
                return false;
            }

        }catch (const std::exception &ex) {
            std::cerr << "exception: " << ex.what() << std::endl;
            throw ex;
        }

        if(dfa_limit >= 2) {
            if(!createDFA()) {
                deleteDFA();
                return false;
            }
        }

        return true;

    }

    /// prefix search, return a vector
    bool prefixSearch(const char *s, std::vector<patternGroup> &res, std::vector<std::size_t> &len) {
        std::vector<DartsDatrie::result_pair_type> results;
//        std::string newS = hostReversal(s, true);
//        std::size_t num_results = datrie.commonPrefixSearch(newS.c_str(), results);
        std::size_t num_results = datrie.commonPrefixSearch(s, results);


        if(debug) {
            printf("---------- prefixSearch() ---------- \n");
            printf("reversal keyword: %s\n", s);
            printf("num_results: %zu\n", num_results);
        }

        if(num_results <= 0)    return false;
        for(std::size_t i = 0; i < results.size(); ++i) {
            res.push_back(getRegex(results[i].value));
            len.push_back(results[i].length);
        }
        return true;
    }

    ///   pc url to mobile url
    ///   return 0: return correct answer
    ///   return 1: don't match any prefix
    ///   return 2: don't match any pattern
    int pcToMobile(const std::string &pc_url, std::string &mobile_url, std::string *matchedPat = NULL) {

        if(debug) {
            printf("pc url: %s\n", pc_url.c_str());
        }
        std::string re_pc_url = hostReversal(pc_url, true);
        std::string re_pc_url_for_sub = re_pc_url;
        /// tolower
        std::transform(re_pc_url.begin(), re_pc_url.end(), re_pc_url.begin(), ::tolower);
        /// pc_url for substitute, re_pc_url for prefixSearch
        std::vector<DartsPattern::patternGroup> pc_pats;
        std::vector<std::size_t> len;
        bool ok = prefixSearch(re_pc_url.c_str(), pc_pats, len);
        if(ok) {
            bool isfound = false;
            std::vector<DartsPattern::patternGroup>::reverse_iterator it;
            if(debug) {
                /// print all of patterns have found in this prefix.
                for(it = pc_pats.rbegin(); it != pc_pats.rend(); it++) {
                    printf("There are %zu pc patterns in this prefix.\n", it->num);
                    for(std::size_t k = 0; k < it->num; k++) {
                        printf("    %zuth pc pattern: %s, weight: %d\n", k, it->pattern_info[k].pc_pat, it->pattern_info[k].pc_weight);
                    }
                    printf("\n");
                }
            }

            std::vector<std::size_t>::reverse_iterator it2 = len.rbegin();
            for(it = pc_pats.rbegin(); it != pc_pats.rend(); it++, it2++) {
                if(!isfound)
                    ///
                    isfound = it->substitute(re_pc_url_for_sub, mobile_url, *it2, matchedPat);
                for(std::size_t k = 0; k < it->num; k++) {
                    pcre_free((pcre *)it->pattern_info[k].compiled_re);
                    delete [] it->pattern_info[k].mob_pat;
                    delete [] it->pattern_info[k].pc_pat;
                }

            }

            if(isfound) return 0;
            else return 2;
        }

        return 1;    /// don't have prefix in datrie

    }


    ///   get all mobile url and pattern, wap2pat: <mobile url, pattern>
    ///   return 0: return correct answer
    ///   return 1: don't match any prefix
    ///   return 2: don't match any pattern
    int pcToMobile(const std::string &pc_url, std::vector<std::pair<std::string, std::string> > &wap2pat) {
        if(debug) {
            printf("pc url: %s\n", pc_url.c_str());
        }
        std::string re_pc_url = hostReversal(pc_url, true);
        std::string re_pc_url_for_sub = re_pc_url;
        /// tolower
        std::transform(re_pc_url.begin(), re_pc_url.end(), re_pc_url.begin(), ::tolower);
        /// pc_url for substitute, re_pc_url for prefixSearch
        std::vector<DartsPattern::patternGroup> pc_pats;
        std::vector<std::size_t> len;
        bool ok = prefixSearch(re_pc_url.c_str(), pc_pats, len);
        if(ok) {
            bool isfound = false;
            std::vector<DartsPattern::patternGroup>::reverse_iterator it;
            if(debug) {
                /// print all of patterns have found in this prefix.
                for(it = pc_pats.rbegin(); it != pc_pats.rend(); it++) {
                    printf("There are %zu pc patterns in this prefix.\n", it->num);
                    for(std::size_t k = 0; k < it->num; k++) {
                        printf("    %zuth pc pattern: %s, weight: %d\n", k, it->pattern_info[k].pc_pat, it->pattern_info[k].pc_weight);
                    }
                    printf("\n");
                }
            }

            std::vector<std::size_t>::reverse_iterator it2 = len.rbegin();
            for(it = pc_pats.rbegin(); it != pc_pats.rend(); it++, it2++) {
                if(!isfound)
                ///
                    isfound = it->substitute(re_pc_url_for_sub, wap2pat, *it2);
                else
                    it->substitute(re_pc_url_for_sub, wap2pat, *it2);
                for(std::size_t k = 0; k < it->num; k++) {
                    pcre_free((pcre *)it->pattern_info[k].compiled_re);
                    delete [] it->pattern_info[k].mob_pat;
                    delete [] it->pattern_info[k].pc_pat;
                }

            }
            if(isfound) return 0;
            else return 2;
        }

        return 1;    /// don't have prefix in datrie

    }





private:
    friend struct patternGroup;

    struct stringInfo{
        std::size_t offset;
        std::size_t length;
    };

    /// pattern preprocessing
    std::size_t preProcess(const char* input, std::map<std::string, patternGroup> &patternsMap) {
        std::ifstream fin(input);
        std::string line;
        while(getline(fin, line)) {
            if(line.empty() || line == "\t" || line == "" || line == "\n") continue;


            std::vector<std::string> tokens = cutToNParts(line);
            patternInfo tmp_pi;

            if(tokens.size() == 2)  tmp_pi.pc_weight = 1;
            else if(tokens.size() == 3) tmp_pi.pc_weight = atoi(tokens[2].c_str());
            else {
                std::cerr << "line: " << line << " ,  cut it to three part failed" << std::endl;
                continue;
            }

            /// judge pattern

            if(!judgePattern(tokens[0], tokens[1])) {
                std::cerr << "line: " << line << ", is a bad pattern." << std::endl;
                continue;
            }

            /// host reversal
            std::string prefix = hostReversal(tokens[0]);
            if(debug) {
                printf("hostReversal: %s\n", prefix.c_str());
            }

            /// get prefix from pattern
            std::size_t end_prefix = lastNotOfRegex(prefix, tmp_pi.pc_weight);
            std::string tmp = std::string("^") + prefix.substr(std::min(end_prefix + 1, prefix.length()));
            prefix = prefix.substr(1, end_prefix);

            /// judge empty prefix
            if(prefix.empty()) {
                std::cerr << "line: " << line << ", empty prefix." << std::endl;
                continue;
            }

            std::size_t erroroffset;
            const char *error;
            tmp_pi.compiled_re = pcre_compile(tmp.c_str(), PCRE_CASELESS, &error, (int *)&erroroffset, NULL);
            if(tmp_pi.compiled_re == NULL) {
                std::cerr << "line: " << line << std::endl;
                std::cerr << "regex: " << tmp << " compile failed." << std::endl;
                std::cerr << "with error: " << error << std::endl;
                continue;

            }
            int rc = pcre_fullinfo(tmp_pi.compiled_re, NULL, PCRE_INFO_SIZE, &tmp_pi.re_length);
            if(rc < 0) {
                std::cerr << "line: " << line << std::endl;
                std::cerr << "regex: " << tmp << " get size of compiled regex failed." << std::endl;
                continue;

            }

            tmp_pi.mob_length = tokens[1].length();
            tmp_pi.mob_pat = new char[tmp_pi.mob_length + 1];
            strcpy((char *)tmp_pi.mob_pat, tokens[1].c_str());
            ((char *)tmp_pi.mob_pat)[tmp_pi.mob_length] = '\0';


            tmp_pi.pc_length = tokens[0].length();
            tmp_pi.pc_pat = new char[tmp_pi.pc_length + 1];
            strcpy((char *)tmp_pi.pc_pat, tokens[0].c_str());
            ((char *)tmp_pi.pc_pat)[tmp_pi.pc_length] = '\0';

            /// prefix tolower
            std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

            /// delete '\\' in prefix
            std::string::iterator new_end = std::remove_if(prefix.begin(), prefix.end(), std::bind2nd(std::equal_to<char>(), '\\'));
            prefix.erase(new_end, prefix.end());

            if(patternsMap.find(prefix) == patternsMap.end()) {
                patternGroup pg;
                pg.num = 1;
                pg.pattern_info.push_back(tmp_pi);
                pg.size += tmp_pi.re_length + tmp_pi.pc_length + tmp_pi.mob_length + sizeof(std::size_t) * 3  + sizeof(int);
                patternsMap[prefix] = pg;
            }else {
                patternsMap[prefix].num++;
                patternsMap[prefix].pattern_info.push_back(tmp_pi);
                patternsMap[prefix].size += tmp_pi.re_length + tmp_pi.pc_length + tmp_pi.mob_length + sizeof(std::size_t) * 3  + sizeof(int);
            }
        }
        fin.close();
        if(debug) {
            printf("---------- preProcess() ----------\n");
            std::map<std::string, patternGroup>::iterator it;
            for(it = patternsMap.begin(); it != patternsMap.end(); it++) {
                printf("prefix: %s\n", it->first.c_str());
                printf("    num of patterns: %zu\n", it->second.num);
                printf("    total size: %zu\n", it->second.size);
            }
        }

        return patternsMap.size();
    }

    /// create mmap
    bool create_mmap(const char* input) {
        int fd = open(input, O_RDONLY);
        if(fd == -1) {
            std::cerr << "open file " << input << " failed when build mmap." << std::endl;
            return false;
        }

        off_t page_offset = 0;
//        pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
//        offset for mmap() must be page aligned */
        std::size_t length = dat_offset;
        if(debug) {
            printf("---------- create_mmap() ----------\n");
            printf("dat_offset: %zu\n", dat_offset);
            printf("dat_size: %zu\n", dat_size);
        }

        addr = (const char *)mmap(NULL, length, PROT_READ,MAP_PRIVATE, fd, page_offset);
        close(fd);
        if (addr == MAP_FAILED) {
            std::cerr << "build mmap failed." << std::endl;

            return false;
        }
        return true;
    }

    ///delete mmap
    void delete_mmap() {
        if(addr != NULL && addr != MAP_FAILED) {
            int mc = munmap((void *)addr, dat_offset);
            if(mc == -1) std::cerr << "delete mmap failed." << std::endl;
        }
        addr = NULL;
    }

    /// get regex-part by value(int) from mmap
    const patternGroup getRegex(DartsDatrie::value_type idx) {
        stringInfo regexInfo;
        std::size_t tmp_offset = sizeof(std::size_t) * 2 + sizeof(stringInfo) * idx;
        memcpy((void*)&regexInfo, addr + tmp_offset, sizeof(stringInfo));

        if(debug) {
            printf("---------- getRegex() ----------\n");
            printf("regexInfo.length: %zu , regexInfo.offset: %zu\n", regexInfo.length, regexInfo.offset);
        }

        patternGroup thisRegex;
        thisRegex.size = regexInfo.length;
        tmp_offset = regexInfo.offset + sizeof(stringInfo) * dat_size;
        memcpy((void*)&thisRegex.num, addr + tmp_offset, sizeof(thisRegex.num));
        tmp_offset += sizeof(thisRegex.num);
        for(std::size_t i = 0; i < thisRegex.num; ++i) {
            patternInfo tmp_pi;

            memcpy((void*)&tmp_pi.pc_length, addr + tmp_offset, sizeof(tmp_pi.pc_length));
            tmp_offset += sizeof(tmp_pi.pc_length);

            tmp_pi.pc_pat = new char[tmp_pi.pc_length + 1];
            if (NULL == tmp_pi.pc_pat){
                continue; 
            }
            memcpy((void*)tmp_pi.pc_pat, addr + tmp_offset, tmp_pi.pc_length);
            ((char *)tmp_pi.pc_pat)[tmp_pi.pc_length] = '\0';
            tmp_offset += tmp_pi.pc_length;

            memcpy((void*)&tmp_pi.pc_weight, addr + tmp_offset, sizeof(tmp_pi.pc_weight));
            tmp_offset += sizeof(tmp_pi.pc_weight);

            memcpy((void*)&tmp_pi.re_length, addr + tmp_offset, sizeof(tmp_pi.re_length));
            tmp_offset += sizeof(tmp_pi.re_length);

            tmp_pi.compiled_re = (const pcre *)pcre_malloc(tmp_pi.re_length);
            if (tmp_pi.compiled_re == NULL){
                delete [] tmp_pi.pc_pat;
                continue; 
            }
            memcpy((void*)tmp_pi.compiled_re, addr + tmp_offset, tmp_pi.re_length);
            tmp_offset += tmp_pi.re_length;

            memcpy((void*)&tmp_pi.mob_length, addr + tmp_offset, sizeof(tmp_pi.mob_length));
            tmp_offset += sizeof(tmp_pi.mob_length);

            tmp_pi.mob_pat = new char[tmp_pi.mob_length + 1];
            if (NULL == tmp_pi.mob_pat){
                delete [] tmp_pi.pc_pat;
                pcre_free (const_cast<pcre*>(tmp_pi.compiled_re));
                continue; 
            }
            memcpy((void*)tmp_pi.mob_pat, addr + tmp_offset, tmp_pi.mob_length);
            ((char *)tmp_pi.mob_pat)[tmp_pi.mob_length] = '\0';
            tmp_offset += tmp_pi.mob_length;

            thisRegex.pattern_info.push_back(tmp_pi);

        }

        if(thisRegex.num != thisRegex.pattern_info.size()){
            std::cerr << "get regex error." << std::endl;
        }

        /// get DFA
        if(idx < dfa_vec.size()) {
            thisRegex.pattern_dfa = dfa_vec[idx];
            thisRegex.pattern_dfa_complement = dfa_complement_vec[idx];
        }else {
            thisRegex.pattern_dfa = NULL;
            thisRegex.pattern_dfa_complement.clear();
//            if(dfa_limit >= 0)
//                std::cerr << "ERROR: idx is bigger than dfa_vec.size()!!" << std::endl;
        }


        return thisRegex;

    }
    /// cut string into parts by delim
    const std::vector<std::string> cutToNParts(const std::string &content,  const char delim = '\t') {
        std::vector<std::string> ans;

        std::size_t __begin = 0, __end;
        while((__end = content.find_first_of(delim, __begin)) != std::string::npos) {
            ans.push_back(content.substr(__begin, __end - __begin));
            __begin = __end + 1;
        }

        ans.push_back(content.substr(__begin, content.length() - __begin));
        return ans;
    }


    /// reversal host of pattern or url
    /// eg: "http://www\.360\.cn/spider/page/(\d+)$" -> "http://cn\.360\.www/spider/page/(\d+)$"
    static std::string hostReversal(const std::string &pat, bool isUrl = false) {
//        std::vector<std::pair<std::size_t, std::size_t> > bracket_info;
//        if (!isUrl && !bracketAnalysis(pat, bracket_info)) return "";

        std::size_t pos_0, pos_1, end_host, len = pat.length();
        std::stack<std::string> pat_part;
        std::string point = isUrl? ".": "\\.";
        std::size_t offset = isUrl? 1: 2;

        pos_0 = pat.find("//") + 2;
        if((end_host = pat.find("/", pos_0)) == std::string::npos) {
            end_host = (pat[len - 1] == '$'? len - 1: len);
        }
        std::string host = pat.substr(0, end_host);
        std::string ans = pat.substr(0, pos_0);
        std::size_t find_offset = 0;
        while((pos_1 = host.find(point, pos_0 + find_offset)) != std::string::npos) {
//
//            if(!isUrl) {
//                bool is_in_bracket = false;
//                for(std::size_t i = 0; i < bracket_info.size(); ++i) {
//                    if(pos_1 > bracket_info[i].first && pos_1 + offset - 1 < bracket_info[i].second) {
//                        is_in_bracket = true;
//                        break;
//                    }
//                }
//                if(is_in_bracket) {
//                    find_offset = pos_1 + offset - pos_0;
//                    continue;
//                }
//            }



            pat_part.push(pat.substr(pos_0, pos_1 - pos_0));
            pos_0 = pos_1 + offset;
        }
        ans += host.substr(pos_0, host.length() - pos_0);
        while(!pat_part.empty()) {
            ans = ans + point + pat_part.top();
            pat_part.pop();
        }

        /// [^.] -> [^./]
        if(isUrl == false) {
            std::size_t pos_adjust = ans.find("^.");
            if(pos_adjust != std::string::npos) ans.replace(pos_adjust, 2, "^./");
        }
        ans += pat.substr(end_host, len - end_host);


        return ans;

    }

    /// bracketAnalysis
    bool bracketAnalysis(const std::string &str, std::vector<std::pair<std::size_t, std::size_t> > &ans) {
        std::stack<std::pair<char, std::size_t> >   judge_stack;
        for(std::size_t i = 0; i < str.length(); ++i) {
            switch (str[i]) {
                case '[': case '(':
                    judge_stack.push(std::make_pair(str[i], i));
                    break;
                case ']':
                    if(judge_stack.top().first == '[')    ans.push_back(std::make_pair(judge_stack.top().second, i));
                    judge_stack.pop();
                    break;
                case ')':
                    if(judge_stack.top().first == '(')    ans.push_back(std::make_pair(judge_stack.top().second, i));
                    judge_stack.pop();
                    break;
                default:
                    break;
            }

        }
        return judge_stack.empty()? true: false;


    }


    /// the end of prefix don't contain regex
    static std::size_t lastNotOfRegex(const std::string &pat, const int weight) {
        static const std::size_t num_of_last_re = 12;
        static const char *re[num_of_last_re] = {"(", "\\d", "\\w", "\\S", "[", "$", "\\(", "\\[", // 8
                                                 ")", "]", "\\)", "\\]"};  // 4
        std::size_t ans = pat.length() + 1;
        for(std::size_t i = 0; i < num_of_last_re; ++i) {
            std::size_t tmp = pat.find(re[i]);
            ans = std::min(tmp, ans);
        }
        if(weight == IN_BLACK_LIST) {
            std::size_t pos_0, end_host, len = pat.length();
            pos_0 = pat.find("//") + 2;
            if((end_host = pat.find("/", pos_0)) == std::string::npos) {
                end_host = (pat[len - 1] == '$'? len - 1: len);
            }
            return std::min(end_host - 1, ans - 1);
        }

        return ans - 1;
    }

    /// judge is this pattern a bad case.
    bool judgePattern(const std::string &pc_pat, const std::string &mob_pat) {
        /// check regex part
        int pc_sub_num = 0, mob_sub_num = 0;
        std::size_t pc_len = pc_pat.length(), mob_len = mob_pat.length();

        for(std::size_t i = 0; i < pc_len; ++i) {
            if(pc_pat.at(i) == '('
                && (i - 1 >= pc_len || pc_pat.at(i - 1) != '\\')
                && (i + 2 >= pc_len || pc_pat.substr(i + 1, 2) != "?:" )
            ) {
                ++pc_sub_num;
            }

            if(pc_pat.at(i) == '|' && pc_pat.find_first_of(')', i) >= pc_pat.find_first_of('(', i)) {
                return false;
            }
        }

        for(std::size_t i = 0; i < mob_len; ++i) {
            if(mob_pat.at(i) == '\\' && i + 1 < mob_len)
                mob_sub_num = std::max(mob_sub_num, mob_pat.at(i + 1) - '0');
        }
        if(mob_sub_num > pc_sub_num)    return false;

        ///check host part
        /// pc_pat
        std::size_t pos_0, pos_1, end_host;
        bool isUrl = false;
        std::string point = isUrl? ".": "\\.";
        std::size_t offset = isUrl? 1: 2;

        pos_0 = pc_pat.find("//") + 2;
        if((end_host = pc_pat.find("/", pos_0)) == std::string::npos) {
            end_host = (pc_pat[pc_len - 1] == '$'? pc_len - 1: pc_len);
        }
        std::string host = pc_pat.substr(0, end_host);
        while((pos_1 = host.find(point, pos_0)) != std::string::npos) {
            if(pc_pat.substr(pos_0, pos_1 - pos_0).empty())
                return false;
            pos_0 = pos_1 + offset;
        }
        if(host.substr(pos_0, host.length() - pos_0).empty())
            return false;

        ///wap pat
        isUrl = true;
        point = isUrl? ".": "\\.";
        offset = isUrl? 1: 2;

        pos_0 = mob_pat.find("//") + 2;
        if((end_host = mob_pat.find("/", pos_0)) == std::string::npos) {
            end_host = (mob_pat[mob_len - 1] == '$'? mob_len - 1: mob_len);
        }
        host = mob_pat.substr(0, end_host);
        while((pos_1 = host.find(point, pos_0)) != std::string::npos) {
            if(mob_pat.substr(pos_0, pos_1 - pos_0).empty())
                return false;
            pos_0 = pos_1 + offset;
        }
//        if(host.substr(pos_0, host.length() - pos_0).empty())
//            return false;

        /// return
        return true;
    }

    /// create DFA in memory
    bool createDFA() {
        re2::RE2::Options dfa_ropt;
        dfa_ropt.set_case_sensitive(false);
        dfa_ropt.set_never_capture(true);
        dfa_ropt.set_max_mem(DFA_MAX_MEM);


        deleteDFA();
        dfa_vec.clear();
        dfa_complement_vec.clear();
        for(DartsDatrie::value_type i = 0; i < dat_size; ++i) {
            patternGroup dfa_patg = getRegex(i);
            if(dfa_patg.num >= dfa_limit) {
                bool onedfa_finish = dfa_patg.makeOneDFA(dfa_ropt);
                dfa_vec.push_back(dfa_patg.pattern_dfa);
                if(!onedfa_finish) {
                    return false;
                }
            }else {
                dfa_vec.push_back(NULL);
            }
            dfa_complement_vec.push_back(dfa_patg.pattern_dfa_complement);

            ///delete dfa_patg

            for(std::size_t k = 0; k < dfa_patg.num; k++) {
                pcre_free((pcre *)dfa_patg.pattern_info[k].compiled_re);
                delete [] dfa_patg.pattern_info[k].mob_pat;
                delete [] dfa_patg.pattern_info[k].pc_pat;
            }



        }

        if(dfa_vec.size() != dat_size || dfa_complement_vec.size() != dat_size) {
            std::cerr << "ERROR: createDFA failed, dfa_vec.size(): " << dfa_vec.size() << ", dfa_complement_vec.size(): " << dfa_complement_vec.size() << ", dat_size:" << dat_size << std::endl;
            return false;
        }
        return true;
    }

    /// delete created DFA
    void deleteDFA() {
        for(std::size_t i = 0; i < dfa_vec.size(); ++i) {
            if(dfa_vec[i] != NULL) {
                delete dfa_vec[i];
                dfa_vec[i] = NULL;
            }
        }

        return ;
    }

private:

    DartsDatrie datrie;
    std::size_t dat_offset;   /// offset of datrie in binfile
    std::size_t dat_size;     /// number of patterns in datrie
    const char *addr;         /// mmap start address
    bool debug;

    int dfa_limit;
    std::vector<re2::RE2::Set*> dfa_vec;                /// store compiled dfas
    std::vector<std::vector<int> > dfa_complement_vec;  /// store complement of dfa(add failed)

};



#endif // DARTS_PATTERN_H_INCLUDED
