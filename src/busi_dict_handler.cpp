#include "busi_dict_handler.h"

CBusiDictHandler::CBusiDictHandler() : m_config_path("") {}

CBusiDictHandler::CBusiDictHandler(const std::string& config) : m_config_path(config) {}

CBusiDictHandler::~CBusiDictHandler() {
    FOR_EACH(dict_repo_itr, m_dict_repo) {
        IDict* dict_ptr = dict_repo_itr->second;
        if(NULL != dict_ptr) {
            delete dict_ptr; 
        }
    }
}

int CBusiDictHandler::SetConfig(const std::string& config) {

    m_config_path = config;

}





