#ifndef __DICTS_PATTERN_DICT__
#define __DICTS_PATTERN_DICT__

#include "dict.h"
#include "key.h"
#include "value.h"

#include "re2/set.h"
#include "darts.h"


class PatternDict : IDict {

    public:
        PatternDict();
        ~PatternDict();

        int Set(const IKey& key, const IValue& value);
        int Del(const IKey& key);
        int Get(const IKey& key, IValue* value);
        int Load(const std::string& dict_data_load_path);
        int Dump(const std::string& dict_data_dump_path);
        int Finalize() = 0;

        int Clear() = 0;

    private:
        typedef struct Node1{
            IKey*       key;
            IValue*     value;

            Node1(IKey* k, IValue* v) : key(k), value(v) {}

            bool operator == (const Node1& n) {
                if(n.key->Compare(*key) == 0) {
                    return true; 
                }
                return false;
            }
            bool operator == (const Ikey& k) {
                if(k.Compare(*key) == 0) {
                    return true; 
                }
                return false;
            }

        }DictInfoMeta;

        typedef struct {
            std::string regex;
            size_t      index;
        }RegexInfoMeta;

        typedef struct Node2{
            std::string                     prefix;         // regex's prefix
            std::vector<RegexInfoMeta>      regex_info_repo; // wait to add into dfa
            std::vector<RegexInfoMeta>      regex_info_online;  // add into dfa successful
            std::vector<RegexInfoMeta>      regex_info_offline; // add into dfa unsuccessful
            re2::RE2::Set                   dfa;
            bool operator == (const Node2& n) {
                return n.prefix == prefix; 
            }
            bool operator == (const std::string& p) {
                return p == prefix; 
            }

        }PrefixInfoMeta;

        typedef Darts::DoubleArrayImpl<char, unsigned char, long, unsigned long> DartsDatrie;

    private:
        // reading
        std::vector<DictInfoMeta>           m_dict_info_read;
        std::vector<PrefixInfoMeta>         m_prefix_info_read;
        DartsDatrie*                        m_darts_datrie_read;

        // writing (separation from reading)
        std::vector<DictInfoMeta>           m_dict_info_write;
        std::vector<PrefixInfoMeta>         m_prefix_info_write;
        DartsDatrie*                        m_darts_datrie_write;
};
#endif
