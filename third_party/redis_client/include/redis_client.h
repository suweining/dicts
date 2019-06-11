#ifndef __REDIS_CLIENT_H__
#define __REDIS_CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <hiredis.h>

#ifdef DEBUG
    #define _DEBUG(x) {x}
#else
    #define _DEBUG(x)
#endif


#ifdef USE_NGX_LOG
    #define REDIS_CLIENT_LOG(LEVEL, info) \
    do { \
        onebox_log_error(LEVEL, info); \
    } while (0)
#else
    #define REDIS_CLIENT_LOG(LEVEL, info) \
    do { \
        fprintf(stderr, "%s : %s\n", #LEVEL, info); \
    } while (0)
#endif

enum ErrorLevel {
    EL_OK,
    EL_COMMON_ERROR,
    EL_CONTEXT_FAILURE,
    EL_SERVER_CRASH,
    EL_NONE
};

class RedisClient {
public:
    //reconnection_interval_usec specifies the reconnect interval
    //if reconnection_interval_usec == 0, the redis client will reconnect with each failure
    //if reconnection_interval_usec == -1, the redis client will never reconnect
    RedisClient(const std::string& host, uint32_t port, const std::string& password = "", uint32_t database_num = 0, uint64_t reconnection_interval_usec = 1000 * 1000UL, uint64_t connection_timeout_usec = 1000 * 1000UL, uint64_t operation_timeout_usec = 1000 * 1000UL);

    ~RedisClient();

    void SetReconnectionInterval(uint64_t reconnection_interval_usec);

    void SetConnectionTimeout(uint64_t connection_timeout_usec);

    void SetOperationTimeout(uint64_t operation_timeout_usec);

    bool Connect();

    bool Select(uint32_t database_num);

    bool Set(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string* value);
    bool Del(const std::string& key);

    bool MSet(const std::vector<std::string>& keys, const std::vector<std::string>& values);
    bool MGet(const std::vector<std::string>& keys, std::vector<std::string>* values);
    //bool MDel();

    bool HSet(const std::string& key, const std::string& field, const std::string& value);
    bool HGet(const std::string& key, const std::string& field, std::string* value);
    bool HDel(const std::string& key, const std::string& field);

    bool HMSet(const std::string& key, const std::vector<std::string>& fields, const std::vector<std::string>& values);
    bool HMGet(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>* values);
    //bool HMDel();

private:
    bool ReconnectIfNeed();

    redisReply* CommandArgv(int argc, const char** argv, const size_t* argvlen);

    void Disconnect();

    void MarkServerCrash();

    ErrorLevel GetErrorLevel(const redisReply* reply);
    ErrorLevel GetErrorLevel(const redisContext* context);

    void FreeReply(redisReply** reply);

private:
    std::string m_host;
    uint32_t m_port;
    std::string m_password;

    uint64_t m_connection_timeout_usec;
    uint64_t m_operation_timeout_usec;
    uint64_t m_reconnection_interval_usec;

    struct timeval m_last_server_crash_time;

    uint32_t m_current_database;

    redisContext* m_context;
};



#endif /* __REDIS_CLIENT_H__ */
