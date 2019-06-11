#include <stdlib.h>
#include "redis_dict.h"
#include "redis_common_key.h"
#include "redis_common_value.h"
#include "util.h"
#include "log.h"

CRedisDict::CRedisDict() : m_redis_client(NULL){

}

CRedisDict::~CRedisDict(){

}

int CRedisDict::Init(const std::string& params){

    log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict params[%s] init begin",
            __FILE__,
            __LINE__,
            pthread_self(),
            params.c_str());

    // 1. get host, port from params
    const std::vector<std::string>& param_vec = StringToTokens(params, false, ':', false);
    if(param_vec.size() < 0) {
        log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict params[%s] is error",
                __FILE__,
                __LINE__,
                pthread_self(),
                params.c_str());
        return 1;
    }

    const std::string& host = param_vec[0];
    int port = atoi(param_vec[1].c_str());
    std::string pwd = "";

    if(param_vec.size() >= 3) {
        pwd = param_vec[3];
    }

    log (LOG_DEBUG, "%s:%d\ttid:%lld\t\tclass:CRedisDict params host:%s, port:%d, pwd:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            host.c_str(),
            port,
            pwd.c_str());

    // 2. connect to redis/pika
    m_redis_client = new RedisClient(host, port);
    // 3. ping redis/pika

    if(NULL == m_redis_client) {
        log (LOG_WARNING, "%s:%d\ttid:%lld\t\tclass:CRedisDict init failed host:%s, port:%d, pwd:%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                host.c_str(),
                port,
                pwd.c_str());
        return 2;
    }

    if(!m_redis_client->Connect()) {
         log (LOG_WARNING, "%s:%d\ttid:%lld\t\tclass:CRedisDict connect failed host:%s, port:%d, pwd:%s",
                __FILE__,
                __LINE__,
                pthread_self(),
                host.c_str(),
                port,
                pwd.c_str());
        return 3;
    }
    log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict init success host:%s, port:%d, pwd:%s",
            __FILE__,
            __LINE__,
            pthread_self(),
            host.c_str(),
            port,
            pwd.c_str());


    return 0;
}

int CRedisDict::Add(const IKey& key, const IValue& value){
    return Set(key, value);
}

int CRedisDict::Set(const IKey& key, const IValue& value){
    std::string key_str = "";
    std::string value_str = "";

    // 1. check redis connect status
    if(NULL == m_redis_client) {
        return 1;
    }
    // 2. get key_str or value_str
    if(key.GetKey(&key_str)) {
        log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict\tfunc:Add\tinfo:GetKey fail",
                __FILE__,
                __LINE__,
                pthread_self());

        return 2;
    }

    if(value.GetVal(&value_str)) {
        log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict\tfunc:Add\tinfo:GetVal fail",
                __FILE__,
                __LINE__,
                pthread_self());

        return 3;
    }

    // 3. add into redis
    if(!m_redis_client->Set(key_str, value_str)) {
        log (LOG_WARNING, "%s:%d\ttid:%lld\t\tclass:CRedisDict\t add redis(%s, %s) fail",
                __FILE__,
                __LINE__,
                pthread_self(),
                key_str.c_str(),
                value_str.c_str());
        return 4;
    }

    return 0;
}

int CRedisDict::Del(const IKey& key){
    std::string key_str = "";
   // 1. check redis connect status
    if(NULL == m_redis_client) {
        return 1;
    }
    // 2. get key_str
    if(key.GetKey(&key_str)) {
        log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict GetKey fail",
                __FILE__,
                __LINE__,
                pthread_self());

        return 2;
    }

    // 3. del from redis
    if(m_redis_client->Del(key_str)) {
         log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict del(%s) fail",
                __FILE__,
                __LINE__,
                pthread_self(),
                key_str.c_str());
    }

    return 0;
}

int CRedisDict::Get(const IKey& key, std::vector<IValue*>* value){
    std::string key_str;
   // 1. check redis connect status
    if(NULL == m_redis_client) {
        return 1;
    }
    // 2. get key_str 
    if(key.GetKey(&key_str)) {
        log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict GetKey fail",
                __FILE__,
                __LINE__,
                pthread_self());

        return 2;
    }

    // 3. get from redis
    std::string value_str;
    if(m_redis_client->Get(key_str, &value_str)) {
         log (LOG_INFO, "%s:%d\ttid:%lld\t\tclass:CRedisDict Get(%s) fail",
                __FILE__,
                __LINE__,
                pthread_self(),
                key_str.c_str());
    }
    IValue* candidate_value = new CRedisCommonValue();
    if(candidate_value->SetVal(&value_str)) {
        log (LOG_WARNING, "%s:%d\ttid:%lld\t\tclass:CRedisDict SetVal(%s) fail",
                __FILE__,
                __LINE__,
                pthread_self(),
                value_str.c_str());
        return 3;
    }
    value->push_back(candidate_value);
    return 0;
}


int CRedisDict::Dump(const std::string& dict_data_dump_path){
    return 0;
}

int CRedisDict::Finalize(){
    return 0;
}

int CRedisDict::Info(std::string* info){
    return 0;
}

int CRedisDict::Clear(){
    return 0;
}
