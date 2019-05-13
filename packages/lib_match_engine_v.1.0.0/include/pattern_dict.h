#ifndef __DICTS_PATTERN_DICT__
#define __DICTS_PATTERN_DICT__

#include "dict.h"
#include "key.h"
#include "value.h"

#include "re2/set.h"
#include "darts.h"

#include "thrift/concurrency/Mutex.h"

using namespace apache::thrift::concurrency;

const std::size_t DFA_MAX_MEM = 2L * 1024L * 1024L * 1024L - 1L;

class CPatternDict : public IDict {

    public:
        CPatternDict();
        ~CPatternDict();

        int Set(const IKey& key, const IValue& value);
        int Add(const IKey& key, const IValue& value);
        int Del(const IKey& key);
        int Get(const IKey& key, std::vector<IValue*>* value);
        int Dump(const std::string& dict_data_dump_path);
        int Finalize();

        int Info(std::string* info);
        int Clear();

    private:
        int Separation();
        int BuildTrie();
        int RePrefix(const std::string& re_string, std::string* prefix);
        int ReLegal(const std::string& re_string);
    private:
        class CounterMutex{
            private:
                int counter;
                Mutex counter_mutex;
            public:
                CounterMutex() : counter(0){

                }
                bool UseMutex (){
                    if (counter > 0){
                        return true; 
                    }
                    return false;
                }
                void EntryWrite (){
                    Guard g(counter_mutex);
                    ++ counter;
                    if (counter == 1){ // first writer need wait all reader add mutex
                        sleep (1); 
                    }
                }
                void ExitWrite(){
                    Guard g(counter_mutex);      
                    -- counter;
                    counter = (counter < 0) ? 0 : counter;
                }
        };

        typedef struct Node1{
            IKey*       key;
            IValue*     value;

            Node1(IKey* k, IValue* v) : key(k), value(v) {}
            ~Node1() {
            
            }

            bool operator == (const Node1& n) {
                if(n.key->Compare(*key) == 0) {
                    return true; 
                }
                return false;
            }
            bool operator == (const IKey& k) {
                if(k.Compare(*key) == 0) {
                    return true; 
                }
                return false;
            }

        }DictInfoMeta;

        typedef struct {
            std::string regex;
            size_t      index; // the regex gen from DictInfoMeta[index]
        }RegexInfoMeta;

        typedef struct Node2{
            std::string                     prefix;         // regex's prefix
            std::vector<RegexInfoMeta>      regex_info_repo; // wait to add into dfa
            std::vector<RegexInfoMeta>      regex_info_online;  // add into dfa successful
            std::vector<RegexInfoMeta>      regex_info_offline; // add into dfa unsuccessful
            re2::RE2::Set*                  dfa;
            bool operator == (const Node2& n) {
                return n.prefix == prefix; 
            }
            bool operator == (const std::string& p) {
                return p == prefix; 
            }
            bool operator < (const Node2& n) const {
                return (prefix.compare(n.prefix) < 0);
            }
            int Compare(const Node2& n) {
                return prefix.compare(n.prefix); 
            }
            int BuildDfa();
            int Match(const IKey* key, std::vector<size_t>* indexs);

        }PrefixInfoMeta;

        typedef Darts::DoubleArrayImpl<char, unsigned char, long, unsigned long> DartsDatrie;

    private:
        // key protocal
        std::string                         m_key_class;
        std::string                         m_value_class;

        // reading
        std::vector<DictInfoMeta>           m_dict_info_read;
        std::vector<PrefixInfoMeta>         m_prefix_info_read;
        DartsDatrie*                        m_darts_datrie_read;

        // writing (separation from reading)
        std::vector<DictInfoMeta>           m_dict_info_write;
        std::vector<PrefixInfoMeta>         m_prefix_info_write;
        DartsDatrie*                        m_darts_datrie_write;

        CounterMutex                        m_meta_info_lock_control;
        ReadWriteMutex                      m_meta_info_mutex;
};
#endif
