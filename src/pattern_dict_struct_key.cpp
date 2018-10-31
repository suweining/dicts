#include "pattern_dict_struct_key.h"
#include "util.h"

PatternDictStructKey::PatternDictStructKey() {

}

PatternDictStructKey::~PatternDictStructKey() {

}

int PatternDictStructKey::Init(const void* input) {
    std::string input_str = const_cast<string>(*(string*)input);

    // get pattern field
    std::string pattern_field;
    std::vector<std::string> fields = StringToTokens(input_str, false, ' ');
    FOR_EACH(field_itr, fields) {
        size_t pos = field_itr->find("pattern:");
        if(pos == 0) {
            pattern_field = *field_itr; 
            break;
        }
    }
    std::string pattern = (StringToTokens(pattern_field, false, ':'))[1]
    // TODO: check pattern's syntax
    m_string = pattern;

    // get host
    std::string url_parts = StringToTokens(pattern, false, '/');
    size_t url_parts_len = url_parts.size();
    if(url_parts_len < 2) {
        std::cerr << "pattern error" << std::endl;
        return 1; 
    }

    std::string host = url_parts[2];
    bool host_end_with_dollar = false;
    size_t host_len = host.size();

    if(host[host_len - 1] == '$') {
        host_end_with_dollar = true;
    }

    // erase the dollar in host end
    if(url_parts_len == 2 && host_end_with_dollar) {
        host.erase(host.begin() + host_len - 1);
        host_len = host.size();
    }

    // reversal host
    std::string host_parts = StringToTokens(host, false, '.');
    size_t host_parts_len = host_parts.size();
    std::string reversal_host;
    for(size_t i = host_parts_len - 1; i > 0; --i) {
        reversal_host += host_parts[i] + ".";
    }
    reversal_host += host_parts[0];

    if(host_end_with_dollar) {
        reversal_host += "$";
    }

    // joint url as m_key
    for(size_t i = 0; i < url_parts_len; ++i) {
        if(i == 0) {
            m_key += url_parts[0] + "://";
        }
        else if(i == 1) {
            m_key += reversal_host;
        }
        else {
            m_key += "/" + url_parts[i];
        }
    }

    return 0;
}

int PatternDictStructKey::Key(void* output) const {
    if(m_key.size() <= 0) {
        return 1;
    }

    *(std::string*)output = m_key;
    return 0;
}

int PatternDictStructKey::ToString(void* output) const {
    if(m_string.size() <= 0) {
        return 1;
    }

    *(std::string*)output = m_string;
    return 0;
}

int PatternDictStructKey::Compare(const IKey& key) const {
    std::string key;

    if(key->Key(&key)) {
        cerr << "get key str error" << std::endl;
    }

    return m_key.compare(key);
}

int PatternDictStructKey::Func(const void* input, void* output) {
    // NULL
    return 0;
}
