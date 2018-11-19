#include "pattern_dict_struct_value.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h" 
#include "util.h"
#include "log.h"

PatternDictStructValue::PatternDictStructValue() {

}

PatternDictStructValue::PatternDictStructValue(const std::string& line) {
    if(line.size() > 0) {
        Init(&line);
    }

    log(LOG_DEBUG, "%s:%d\ttid:%lld\tclass:PatternDictStrucValue\tfunction:PatternDictStructValue\tinfo:m_value %s", __FILE__, __LINE__, pthread_self(), m_value.c_str());
}

PatternDictStructValue::~PatternDictStructValue() {

}

int PatternDictStructValue::Init(const void* input) {
    std::string* input_str = const_cast<std::string*>((std::string*)input);
    if(NULL == input_str || input_str->size() == 0) {
        return 1; 
    }
    // init m_fields
    std::vector<std::string> parts = StringToTokens(*input_str, false, '\t');
    FOR_EACH(parts_itr, parts) {
        std::vector<std::string> fields = StringToTokens(*parts_itr, false, ':', true);
        m_fields[ fields[0] ] = fields[1];
    }
    // TODO: get m_value
    rapidjson::StringBuffer str_buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(str_buf);

    writer.StartObject();
    FOR_EACH(fields_itr, m_fields) {
        writer.String((fields_itr->first).c_str());
        writer.String((fields_itr->second).c_str());
    }
    writer.EndObject();

    m_value = str_buf.GetString();
    // TODO: get m_string
    FOR_EACH(fields_itr, m_fields) {
        m_string += fields_itr->first + ":" + fields_itr->second + "\t";
    }
    return 0;
}

int PatternDictStructValue::Val(void* output) {
    if(0 != m_value.size()) {
        * (std::string*) output = m_value; 
    }
    return 0;
}

int PatternDictStructValue::ToString(void* output) {

    if(0 != m_string.size()) {
        * (std::string*) output = m_string; 
    }
    return 0;
}

int PatternDictStructValue::Func(const void* input, void* output) {
    return 0;
}

int PatternDictStructValue::Compare(const IValue& v) {
    return 0;
}
