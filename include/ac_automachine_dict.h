#ifndef __DICTS_AC_AUTOMATCHINE_DICT__
#define __DICTS_AC_AUTOMATCHINE_DICT__

#include "dict.h"
#include "key.h"
#include "value.h"

class CAcAutomatchineDict : public IDict {

    public:
        CAcAutomatchineDict();
        ~CAcAutomatchineDict();

        int Add(const IKey& key, const IValue& value);
        int Set(const IKey& key, const IValue& value);
        int Del(const IKey& key);
        int Get(const IKey& key, std::vector<IValue*>* value);
        int Dump(const std::string& dict_data_dump_path);
        int Finalize();

        int Info(std::string* info);
        int Clear();

    private:
        int BuildAcMathine(const std::vector<std::string>& words);

        struct Record {
            IKey*   key;
            IValue* value;
            Record() : key(NULL), value(NULL) {

            }
            ~Record() {
                if(NULL != key) {
                    delete key;
                    key = NULL;
                }

                if(NULL != value) {
                    delete value;
                    value = NULL;
                }
            }
        };


    private:
        AC_AUTOMATA_t*          m_ac_automation;
        std::vector<Record*>    m_record_vec;
};

#endif
