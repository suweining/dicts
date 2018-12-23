#include <algorithm>
#include "ac_valid_tool.h"
#include "mmap_reader.h"
#include "util.h"
#include "re2/re2.h"
#include "log.h"

CAcValidTool::CAcValidTool() {

    default_anchor = re2::RE2::ANCHOR_START;
    default_dfa_ropt.set_case_sensitive(false);
    default_dfa_ropt.set_never_capture(true);
    default_dfa_ropt.set_max_mem(DFA_MAX_MEM);
}
int CAcValidTool::Init(const std::string& dict_path) {
    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::Init begin", __FILE__, __LINE__, pthread_self());
    m_dict_path = dict_path;
    int rc = 0;
    if((rc = InitReadDict()) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::Init fail: ReadDict code:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 1;
    }
    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::Init success", __FILE__, __LINE__, pthread_self());
    return 0;
}

int CAcValidTool::InitReadDict() {

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::InitReadDict begin", __FILE__, __LINE__, pthread_self());
    MmapReader mreader(m_dict_path.c_str());

    std::string readline;
    size_t readlen;
    while((readlen = mreader.NextLine(&readline)) && readlen != 0) {
        m_dict_basic_infos.push_back(DictBasicInfomation(readline));
//        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::InitReadDict readline:%s", __FILE__, __LINE__, pthread_self(), readline.c_str());
    }

    mreader.Close();

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::InitReadDict success, record.size:%d", __FILE__, __LINE__, pthread_self(), m_dict_basic_infos.size());
    return 0;
}

int CAcValidTool::Build() {
    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::Build begin", __FILE__, __LINE__, pthread_self());
    int rc = 0;
    if((rc = BuildWordTable()) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::Build fail, BuildWordTable ErrorCode:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 1;
    }

    if((rc = BuildRegex()) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::Build fail, BuildRegex ErrorCode:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 2;
    }

    if((rc = BuildTrie()) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::Build fail, BuildTrie ErrorCode:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 3;
    }

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::Build success", __FILE__, __LINE__, pthread_self());
    return 0;
}

int CAcValidTool::BuildWordTable() {

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::BuildWordTable begin", __FILE__, __LINE__, pthread_self());
    // count the words
    std::map<std::string, int> word_count_map;
    FOR_EACH(dbi_itr, m_dict_basic_infos) {
        FOR_EACH(word_itr, dbi_itr->dbi_words)
        if(word_count_map.end() == word_count_map.find(*word_itr)) {
            word_count_map[*word_itr] = 1;
        }
        else {
            ++ word_count_map[*word_itr];
        }
    }

    std::vector<WordCount> word_count_vec;
    FOR_EACH(word_count_map_itr, word_count_map) {
        word_count_vec.push_back(WordCount(word_count_map_itr->first, word_count_map_itr->second));
    }

    // sorted
    std::sort(word_count_vec.begin(), word_count_vec.end());

    size_t len = word_count_vec.size();
    for(size_t i = 0; i <  len; ++i) {
        m_word_table[word_count_vec[i].word] = len - i;
        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildWordTable word:%s\tcount:%d\tindex:%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                word_count_vec[i].word.c_str(),
                word_count_vec[i].count,
                len - i);
    }

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::BuildWordTable Success, wordtable.size:%d", __FILE__, __LINE__, pthread_self(), word_count_vec.size());
    return 0;
}

int CAcValidTool::BuildRegex() {

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::BuildRegex begin", __FILE__, __LINE__, pthread_self());

    FOR_EACH(dbi_itr, m_dict_basic_infos) {

        // 1.gen dbi_indexs
        FOR_EACH(word_itr, dbi_itr->dbi_words) {

            size_t index = m_word_table[*word_itr];

            dbi_itr->dbi_indexs.push_back(index);

        }

        // 2.gen regex
        std::ostringstream oss;
        size_t len = dbi_itr->dbi_indexs.size();
        for(size_t i = 0; i < len - 1; ++i) {
            oss << dbi_itr->dbi_indexs[i] << ",([0-9]+,)*";
        }
        oss << dbi_itr->dbi_indexs[len - 1];
        dbi_itr->dbi_regex = oss.str();

        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildRegex:%s", __FILE__, __LINE__, pthread_self(), dbi_itr->dbi_regex.c_str());
        // 3.sort dbi_indexs for build darts
        std::sort(dbi_itr->dbi_indexs.begin(), dbi_itr->dbi_indexs.end());
    }

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::BuildRegex Success", __FILE__, __LINE__, pthread_self());

    return 0;
}


int CAcValidTool::BuildTrie() {

    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrie begin", __FILE__, __LINE__, pthread_self());

    //1. sort the m_dict_basic_infos by dbi_indexs
    std::sort(m_dict_basic_infos.begin(), m_dict_basic_infos.end());

    for(size_t i = 0; i < m_dict_basic_infos.size(); ++i) {
        std::ostringstream oss;
        FOR_EACH(index_itr, m_dict_basic_infos[i].dbi_indexs) {
            oss << *index_itr << ",";
        }

        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrie line:%d\tindexs:%s", 
                __FILE__,
                __LINE__,
                pthread_self(),
                i,
                oss.str().c_str());
    }

    //2. build the TrieNodeBuild
    TrieNodeBuild trie_node_build;
    size_t len = m_dict_basic_infos.size();

    trie_node_build.tnb_cur = 0;
    trie_node_build.tnb_begin = 0;
    trie_node_build.tnb_end = len;
    int rc = 0;
    if((rc = BuildTrieTreeStruct(&trie_node_build)) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrie fail, BuildTrieTreeStruct ErrorCode:%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return 1;
    }

    // 3. gen m_leaf_match_pool
    if((rc = BuildTrieLeafPool(&trie_node_build)) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrie fail, BuildTrieLeafPool ErrorCode:%d", 
                __FILE__,
                __LINE__,
                pthread_self(),
                rc);
        return 1; 
    }
    // 4. gen m_trie_root
    if((rc = BuildTrieCopy(trie_node_build, &m_trie_root)) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrie fail, BuildTrieCopy ErrorCode:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 2;
    }
    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrie success", __FILE__, __LINE__, pthread_self());
    return 0;
}

int CAcValidTool::BuildTrieTreeStruct(TrieNodeBuild* target_tnb) {

    if(NULL == target_tnb || target_tnb->tnb_begin > target_tnb->tnb_end) {
        return 0;
    }

    log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrieTreeStruct\tbegin:%d\tend:%d\tcur_index:%d\tregex:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            target_tnb->tnb_begin,
            target_tnb->tnb_end,
            target_tnb->tnb_cur,
            m_dict_basic_infos[target_tnb->tnb_begin].dbi_regex.c_str());

    size_t begin = target_tnb->tnb_begin;
    size_t begin_indexs_len = m_dict_basic_infos[begin].dbi_indexs.size();
    size_t cur_index = target_tnb->tnb_cur;

//    // 1. check target_tnb is leaf or not
//    if(begin_indexs_len == cur_index) {
//        target_tnb->tnb_index.push_back(target_tnb->tnb_begin);
//        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrieTreeStruct leaf data index:%d\tregex:%s\tlast_node:%d",
//                __FILE__,
//                __LINE__,
//                pthread_self(),
//                begin,
//                m_dict_basic_infos[begin].dbi_regex.c_str(),
//                m_dict_basic_infos[begin].dbi_indexs[cur_index-1]);
//
//    }
//
    //2. adjust the begin value (such as : 12 12 12 12 123 , adjust begin from 0 to 4) 
    while(begin_indexs_len == cur_index ) {
        target_tnb->tnb_index.push_back(begin);
        ++ begin;
        if(begin >= target_tnb->tnb_end) {
            break;
        }
        begin_indexs_len = m_dict_basic_infos[begin].dbi_indexs.size();
    }

    if(begin >= target_tnb->tnb_end) {
        // if to here, then this target_tnb is leaf, and is such as: 12 12 12 12, while tnb_begin = 0, tnb_end = 4, and cur_index = 2
        return 0;
    }

    if(cur_index >= m_dict_basic_infos[begin].dbi_indexs.size()) {
        // Almost never to here
        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrieTreeStruct index:%d\tregex:%s\tlast_node:%d\tcur_index:%d\tbegin_dbi_indexs.size:%d",
                __FILE__,
                __LINE__,
                pthread_self(),
                target_tnb->tnb_index.size(),
                m_dict_basic_infos[begin].dbi_regex.c_str(),
                m_dict_basic_infos[begin].dbi_indexs[cur_index-1],
                cur_index,
                m_dict_basic_infos[begin].dbi_indexs.size());

        return 0;
    }

    // 3. build target_tnb's child 
    size_t begin_index = m_dict_basic_infos[begin].dbi_indexs[cur_index];
    size_t end = begin + 1;
    for(;end < target_tnb->tnb_end; ++end) {
        size_t end_index = m_dict_basic_infos[end].dbi_indexs[cur_index];
        if(begin_index != end_index) {
            // to here, shall create a child
            TrieNodeBuild new_tnb;
            new_tnb.tnb_begin = begin;
            new_tnb.tnb_end = end;
            new_tnb.tnb_cur = cur_index + 1;
            target_tnb->tnb_child[begin_index] = new_tnb;

            begin = end;
            begin_index = m_dict_basic_infos[begin].dbi_indexs[cur_index];
        }
    }

    TrieNodeBuild new_tnb;
    new_tnb.tnb_begin = begin;
    new_tnb.tnb_end = end;
    new_tnb.tnb_cur = cur_index + 1;
    target_tnb->tnb_child[begin_index] = new_tnb;

    log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrieTreeStruct gen child.\tbegin:%d\tend:%d\tsize:%d",
            __FILE__,
            __LINE__,
            pthread_self(),
            target_tnb->tnb_begin,
            target_tnb->tnb_end,
            target_tnb->tnb_child.size()
            );

    FOR_EACH(tnb_child_itr, target_tnb->tnb_child) {
        if( BuildTrieTreeStruct( &(tnb_child_itr->second) ) ) {
            return 1;
        }
    }
    return 0;
}

int CAcValidTool::BuildTrieLeafPool(TrieNodeBuild* target_tnb) {

    // 1. first need to know how many leaf shall be contructed 
    // (because of re2::RE2::set 's copy-construct is delete)
     
    // 2. build the default m_leaf_match_pool
    // 3. build m_leaf_match_pool[i]
    return 0;
}
int CAcValidTool::BuildTrieCopy(TrieNodeBuild& build_tnb, TrieNodeMatch* match_tnm) {


    if(NULL == match_tnm) {
        return 0;
    }
    match_tnm->tnm_index.swap(build_tnb.tnb_index);

    if(0 != match_tnm->tnm_index.size()) {

        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrieCopy leaf_index:%d", __FILE__, __LINE__, pthread_self(), match_tnm->tnm_index.size());
    }

    if(0 == build_tnb.tnb_child.size()) {
        return 0;
    }

//    match_tnm->tnm_child.reserve( build_tnb.tnb_child.size() );

    FOR_EACH(build_tnb_child_itr, build_tnb.tnb_child) {
        int transf_code = build_tnb_child_itr->first;

        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::BuildTrieCopy child_index:%d", __FILE__, __LINE__, pthread_self(), transf_code);

        TrieNodeMatch child_match_tnm;
        if(BuildTrieCopy(build_tnb_child_itr->second, &child_match_tnm)) {
        }
        m_trie_pool.push_back(child_match_tnm);
        match_tnm->tnm_child.insert({transf_code, m_trie_pool.size() - 1});
    }


    return 0;
}

int CAcValidTool::MatchSequence(const std::vector<std::string>& keys, void* val) {
    log (LOG_INFO, "%s:%d[tid:%lld]\tCAcValidTool::MatchSequence begin", __FILE__, __LINE__, pthread_self());
    //1. word to index
    size_t keys_len = keys.size();
    std::vector<size_t> indexs;
    FOR_EACH(key_itr, keys) {
        auto word_table_itr = m_word_table.find(*key_itr);
        if(word_table_itr != m_word_table.end()) {
            indexs.push_back(word_table_itr->second);
        }
    }

    size_t indexs_len = indexs.size();
    if(indexs_len == 0) {
        return 0;
    }
    //2. gen regex
    std::ostringstream oss;
    oss << indexs[0];
    for(size_t i = 1; i < indexs_len; ++i) {
        oss << "," << indexs[i];
    }
    std::string target_index_seq = oss.str();

    log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::MatchSequence target_regex:%s", __FILE__, __LINE__, pthread_self(), target_index_seq.c_str());
    //3. sort indexs for match_candidate
    std::sort(indexs.begin(), indexs.end());

    //4. match trie to get candidate
    int rc = 0;
    std::vector<size_t> value;
    if((rc = MatchCandidate(m_trie_root, indexs, 0, &value)) && rc != 0) {
        log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::MatchSequence fail, MatchCandidate errorcode:%d", __FILE__, __LINE__, pthread_self(), rc);
        return 1;
    }

    //5. use re2::partialmatch to get the target readline
    FOR_EACH(val_itr, value) {
        std::string regex = m_dict_basic_infos[*val_itr].dbi_regex;

        log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::MatchSequence candidate_regex:%s\ttarget_regex:%s", 
                __FILE__, 
                __LINE__, 
                pthread_self(), 
                regex.c_str(), 
                target_index_seq.c_str());

        if(RE2::PartialMatch(target_index_seq, regex)) {

            * static_cast<std::string*>(val) = m_dict_basic_infos[*val_itr].dbi_readline;

            log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::MatchSequence success, hit:%s", 
                    __FILE__, 
                    __LINE__,
                    pthread_self(),
                    m_dict_basic_infos[*val_itr].dbi_readline.c_str());

            return 0;
        }
    }
    log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::MatchSequence success, miss", __FILE__, __LINE__, pthread_self());
    return 0;
}

int CAcValidTool::MatchCandidate(const TrieNodeMatch& target_tnm, const std::vector<size_t>& indexs, size_t pos, void* candidate) {

    if(0 != target_tnm.tnm_index.size()) {

        std::vector<size_t>* candidate_ptr = static_cast<std::vector<size_t>*>(candidate);
        candidate_ptr->insert(candidate_ptr->end(), target_tnm.tnm_index.begin(), target_tnm.tnm_index.end());
    }

    size_t indexs_len = indexs.size();
    if(pos >= indexs_len) {
        return 0;
    }

    for(size_t i = pos; i < indexs_len; ++i) {
        size_t cur_index = indexs[i];
        auto child_itr = target_tnm.tnm_child.find(cur_index);
        if(child_itr != target_tnm.tnm_child.end()) {

            log (LOG_DEBUG, "%s:%d[tid:%lld]\tCAcValidTool::MatchCandidate hit, pos:%d\tcur_index:%d\tchild:%d", __FILE__, __LINE__, pthread_self(), i, cur_index, child_itr->first);

            if(child_itr->second > m_trie_pool.size()) {
                log (LOG_WARNING, "%s:%d[tid:%lld]\tCAcValidTool::MatchCandidate child's index outof  the boundary, pos:%d\tcur_index:%d\tchild:%d", __FILE__, __LINE__, pthread_self(), i, cur_index, child_itr->first);
                return 1;
            }
            if(MatchCandidate(m_trie_pool[ child_itr->second ], indexs, i + 1, candidate)) {
                return 1;
            }
        }
    }
    return 0;
}

