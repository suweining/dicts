#ifndef LOG_H_
#define LOG_H_

typedef enum {
    LOG_ERROR, LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG
} LOG_LEVEL;

typedef enum {
    LOG_WEEK_DAY, LOG_MONTH_DAY, LOG_YEAR_DAY, LOG_YEAR_DAY_HOUR
} LOG_ROLL;

typedef enum {
    LOG_SERVER, LOG_DB_BUSINESS, LOG_HTTP_BUSINESS, LOG_HTTP
} LOG_MODULE;

void log_init(const char *logfile, LOG_ROLL = LOG_WEEK_DAY, bool append = false);
void log_init(const char *logfile, LOG_ROLL = LOG_WEEK_DAY, LOG_LEVEL = LOG_NOTICE, bool append = false);
void log_close();
void log(int level, const char *fmt, ...);

#endif //end of LOG_H_
