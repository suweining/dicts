#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

#include "log.h"
#define AO_SET(ptr, value)        ((void)__sync_lock_test_and_set((ptr), (value)))

char            g_logfile_name[1024];
pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_flushLog_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE*           g_logfile   = stderr;
LOG_ROLL        g_logroll   = LOG_WEEK_DAY;
LOG_LEVEL       g_loglevel  = LOG_NOTICE;
int             g_logday    = 0;
bool            g_logappend = true;
tm       *pBackup          = NULL; 
pthread_t       pid_updateTime = 0;

void* daemonUpdateTimestamp(void*){
    while (true){
        time_t cur_time = time(time_t(NULL));
        struct tm t;
        tm* p= localtime_r(&cur_time, &t);
        AO_SET (&pBackup, p);
        sleep (1); 
    }
    pthread_exit(0);
    return NULL;
}
void log_init(const char *logfile, LOG_ROLL logroll, bool append) {
    log_init(logfile, logroll, LOG_NOTICE, append);
}

void log_init(const char *logfile, LOG_ROLL logroll, LOG_LEVEL loglevel, bool append) {
    assert(logfile != NULL && strlen(logfile) < 1000);
    struct tm t;
    time_t curtime = time(NULL);
    assert(localtime_r(&curtime, &t));
    g_logroll = logroll;
    g_loglevel = loglevel;
    g_logappend = append;
    switch (g_logroll) {
    case LOG_WEEK_DAY:
        g_logday = t.tm_wday;
        break;
    case LOG_MONTH_DAY:
        g_logday = t.tm_mday;
        break;
    case LOG_YEAR_DAY:
        g_logday = (t.tm_year + 1900) * 10000 + (t.tm_mon + 1) * 100 + t.tm_mday;
        break;
    case LOG_YEAR_DAY_HOUR:
        g_logday = (t.tm_year + 1900) * 1000000 + (t.tm_mon + 1) * 10000 + t.tm_mday * 100 + t.tm_hour;
        break;
    }
   
    sprintf(g_logfile_name, "%s.%d", logfile, g_logday);
    pthread_mutex_init(&g_log_mutex, NULL);
    pthread_mutex_init(&g_flushLog_mutex, NULL);
    g_logfile = fopen(g_logfile_name, (g_logappend) ? "a" : "w");
    assert(g_logfile != NULL);

    int ret = 0;
//    ret = pthread_create (&pid_updateTime, NULL, daemonUpdateTimestamp, NULL);
    if (ret != 0){
        log (LOG_ERROR, "%s:%d\tlog_init create daemonUpdateTimestamp error.", __FILE__, __LINE__); 
        return ;
    }
}

void log_close() {
    assert(g_logfile != NULL);
    fclose(g_logfile);
    if (pid_updateTime != 0){
        pthread_cancel (pid_updateTime);
    }
    pthread_mutex_destroy(&g_log_mutex);
}



void log(int level, const char *fmt, ...) {
    if (level > g_loglevel)
        return;
   // if (NULL == pBackup)
   // {
   //     time_t cur_time = time(time_t(NULL));
   //     struct tm t;
   //     pBackup = localtime_r(&cur_time, &t);
   // }
    time_t cur_time = time(time_t(NULL));
    struct tm t;
    tm* pp= localtime_r(&cur_time, &t);

    tm  p = *pp;

    char ostr[256];
    snprintf(ostr, 256, "%4d-%02d-%02d %02d:%02d:%02d\t", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday, p.tm_hour, p.tm_min, p.tm_sec);

    
    pthread_mutex_lock(&g_log_mutex);
    FILE *logfile = (g_logfile) ? g_logfile : stderr;

    int logday = 0;
    switch (g_logroll) {
    case LOG_WEEK_DAY:
        logday = p.tm_wday;
        break;
    case LOG_MONTH_DAY:
        logday = p.tm_mday;
        break;
    case LOG_YEAR_DAY:
        logday = (p.tm_year + 1900) * 10000 + (p.tm_mon + 1) * 100 + p.tm_mday;
        break;
    case LOG_YEAR_DAY_HOUR:
        logday = (p.tm_year + 1900) * 1000000 + (p.tm_mon + 1) * 10000 + p.tm_mday * 100 + p.tm_hour;
        break;
    }
    if (logday != g_logday && g_logfile != NULL && g_logfile != stderr && g_logfile != stdout) {
        fclose(g_logfile);
        g_logday = logday;
        for (int i = strlen(g_logfile_name) - 1; i >= 0; i--) {
            if (g_logfile_name[i] == '.') {
                g_logfile_name[i] = '\0';
                break;
            }
        }
        sprintf(g_logfile_name, "%s.%d", g_logfile_name, g_logday);
        g_logfile = fopen(g_logfile_name, (g_logappend) ? "a" : "w");
        assert(g_logfile != NULL);
        logfile = g_logfile;
    }

    fprintf(logfile, "%s", ostr);
    switch (level) {
    case LOG_ERROR:
        fprintf(logfile, "ERROR\t");
        break;
    case LOG_WARNING:
        fprintf(logfile, "WARNING\t");
        break;
    case LOG_NOTICE:
        fprintf(logfile, "NOTICE\t");
        break;
    case LOG_INFO:
        fprintf(logfile, "INFO\t");
        break;
    case LOG_DEBUG:
        fprintf(logfile, "DEBUG\t");
        break;
    }

    va_list list;
    va_start(list, fmt);
    vfprintf(logfile, fmt, list);
    va_end(list);
#ifndef BACKWARD_COMPATIBLE_LOG
    fprintf(logfile, "\n");
#endif

    fflush(logfile);

    pthread_mutex_unlock(&g_log_mutex);
}
