#ifndef __INI_H_
#define __INI_H_

#include <map>

struct INI {
    char *sect;
    char *key;
    char *val;
    struct INI *next;
};

// function
INI *ini_init(const char *filename);
bool ini_destruct(INI *ini);
char *ini_read(INI *ini, const char *sect, const char *key);
void ini_read(INI *ini, const char *sect, std::map<char *, char *> &kv);

#endif
