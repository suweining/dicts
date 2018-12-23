#ifndef _AC_VALID_TOOL_H_
#define _AC_VALID_TOOL_H_

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "re2/set.h"
#include "util.h"

static re2::RE2::Options default_dfa_ropt;
static re2::RE2::Anchor default_anchor;

class CAcValidTool {

    const std::size_t DFA_MAX_MEM = 2L * 1024L * 1024L * 1024L - 1L;
    const std::size_t DFA_MAX_RECORD = 5000;
    const std::size_t DFA_MIN_RECORD = 10;
    // basic struct
    struct DictBasicInfomation {
        std::string                     dbi_readline; // read from dict_path
        std::vector<std::string>        dbi_words; // split the key words from dbi_readline
        std::string                     dbi_regex; // index regex
        std::vector<size_t>             dbi_indexs; // words in word_table

        DictBasicInfomation(const std::string& readline) : dbi_readline(readline) {
            std::vector<std::string> readline_parts = StringToTokens(dbi_readline, false, '\t');
            std::string key = readline_parts[0];
            dbi_words = StringToTokens(key, false, '|');
        }

        bool operator< (const DictBasicInfomation& other) const{
            size_t len1 = dbi_indexs.size();
            size_t len2 = other.dbi_indexs.size();
            size_t len = len1 < len2 ? len1 : len2;
            for(size_t i = 0; i < len; ++i) {
                if(dbi_indexs[i] == other.dbi_indexs[i]) {
                    continue; 
                } 
                return dbi_indexs[i] < other.dbi_indexs[i];
            }
            return len1 < len2;
        }
    };

    // trie struct
    struct TrieNodeBuild {
        size_t tnb_begin;
        size_t tnb_end;
        size_t tnb_cur; // 
        std::vector<size_t> tnb_index; // if leaf, then tn_index is the index of m_dict_basic_infos else is 0
        std::map<size_t, TrieNodeBuild> tnb_child; // the child
        TrieNodeBuild() :  tnb_begin(0), tnb_end(0), tnb_cur(0) {}
    };

    class PatternMatch{
        public:
            PatternMatch() : psm_dfa(default_dfa_ropt, default_anchor) {
            }
            int Build(const std::vector<std::string>& regex_vec,
                    const std::vector<size_t>& value_vec);

            int Match(const std::string& key, std::vector<size_t>* value);

        private:
            std::vector<size_t> psm_index; 
            re2::RE2::Set psm_dfa;
    };

    struct TrieNodeMatch{
        size_t tnm_pattern_match_index;
        std::vector<size_t> tnm_index; // if leaf, then tn_index is the index of m_dict_basic_infos else is 0
        std::map<int, int> tnm_child; // the child

        TrieNodeMatch(){
       }
    };


    // sort by count
    struct WordCount{
        std::string word;
        size_t      count;

        WordCount() : word(""), count(0) {}
        WordCount(const std::string& w, size_t c) : word(w), count(c) {
        }

        bool operator< (const WordCount& wc) const {
            return count < wc.count;
        }
    };

    // interface
    public:
        CAcValidTool();
        int Init(const std::string& dict_path);
        int Build();
        int MatchSequence(const std::vector<std::string>& keys, void* val);

    // function for interface
    private:
        int InitReadDict();
        int BuildWordTable();
        int BuildRegex();
        int BuildTrie();
        int BuildTrieTreeStruct(TrieNodeBuild* target_tnb);
        int BuildTrieLeafPool(TrieNodeBuild* target_tnb);
        int BuildTrieCopy(TrieNodeBuild& build_tnb, TrieNodeMatch* match_tnm);
        int MatchCandidate(const TrieNodeMatch& target_tnm, const std::vector<size_t>& indexs, size_t pos, void* candidate);

    // member
    private:
        std::string                         m_dict_path;
        std::map<std::string, size_t>       m_word_table;
        std::vector<DictBasicInfomation>    m_dict_basic_infos;
        TrieNodeMatch                       m_trie_root;
        std::vector<TrieNodeMatch>          m_trie_pool;
        std::vector<PatternMatch>           m_leaf_match_pool;
};
#endif
