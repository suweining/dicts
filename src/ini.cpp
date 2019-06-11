#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "ini.h"

#define LINE_BUF_SIZE 1024

using namespace std;

static char *Trim(char *str) {
    int len;
    char *p;

    if (!str)return NULL;

    len = strlen(str);
    p = str + len;

    if (0 == len) return NULL;

    while (*p == '\0' || *p == '\n' || *p == '\t' || *p == ' ') {
        *p = '\0';
        p--;
        if (0 == --len) break;
    }

    p = str;

    while (*p == '\t' || *p == ' ') {
        p++;
    }

    if (*p == 0) return NULL;

    return p;
}

char *ini_sect(char *str) {
    char *pstart = NULL, *pend = NULL;

    if (!(pstart = strchr(str, '['))) return NULL;
    if (!(pend = strchr(pstart, ']'))) return NULL;

    ++ pstart;
    *pend = 0;

    return pstart;
}

char *ini_trim(char *str) {
    int len = 0;
    char *p = NULL;

    if (!str) {
        return NULL;
    }

    if (!(len = strlen(str))) {
        return NULL;
    }

    if ((p = strstr(str, "#"))) {
        *p = 0;
    }

    if (!(len = strlen(str))) {
        return NULL;
    }

    if ((p = strstr(str, "//"))) {
        *p = 0;
    }

    if (!(len = strlen(str))) {
        return NULL;
    }

    p = str + len;

    while (*p == 0 || *p == '\n' || *p == '\t' || *p == ' ') {
        *p = 0;
        -- p;
        if (0 == --len) break;
    }

    p = str;

    while (*p == '\t' || *p == ' ')
        ++ p;

    if (!(len = strlen(p)))
        return NULL;

    return p;
}

bool ini_format(char *str, char **key, char **val) {
    char *p = NULL;

    if (!(p = strchr(str, '='))) return false;

    *p ++ = 0;

    if (!(*key = Trim(str))) return false;
    if (!(*val = Trim(p))) return false;

    return true;
}

INI *ini_init(const char *filename) {
    INI *ini = NULL, *head = NULL;
    FILE *fd = NULL;
    char *content = NULL, *buf = NULL, *_sect = NULL, *sect = NULL , *key = NULL, *val = NULL;
    bool ret = false;

    buf = new char[LINE_BUF_SIZE];
    if (!buf) goto RETURN;

    fd = fopen(filename, "r");
    if (!fd) goto RETURN;

    while (!feof(fd)) {
        content = fgets(buf, LINE_BUF_SIZE, fd);
        content = ini_trim(content);

        if (!content) continue;

        if ((_sect = ini_sect(content))) {
            if (sect) {
                free(sect); sect = NULL;
            }

            if (!(sect = strdup(_sect))) goto RETURN;

            continue;
        }

        if (!sect) continue;

        if (ini_format(content, &key, &val)) {
            ini = new INI;
            if (!ini) goto RETURN;

            ini->sect = strdup(sect);
            ini->key  = strdup(key);
            ini->val  = strdup(val);

            if (!ini->sect || !ini->key || !ini->val) {
                goto RETURN;
            }

            if (!head) {
                head = ini;
                head->next = NULL;
            } else {
                ini->next = head;
                head = ini;
            }
        }
    }

    ret = true;

RETURN:
    if (fd) {
        fclose(fd); fd = NULL;
    }

    if (sect) {
        free(sect); sect = NULL;
    }

    if (buf) {
        delete []buf; buf = NULL;
    }

    if (ret) return head;

    if (ini) {
        free(ini->sect);
        free(ini->key);
        free(ini->val);

        free(ini);
    }

    ini_destruct(head);

    return NULL;;
}

bool ini_destruct(INI *head) {
    INI *ini;

    while (head) {
        ini = head;

        free(head->sect);
        free(head->key);
        free(head->val);

        head = head->next;
        ini->next = NULL;
        free(ini);
    }

    return true;
}

char *ini_read(INI *head, const char *sect, const char *key) {
    INI *ini = head;

    while (ini) {
        if (0 == strcmp(sect, ini->sect) &&
            0 == strcmp(key, ini->key)) {
            return ini->val;
        }
        ini = ini->next;
    }
    return NULL;
}

void ini_read(INI *head, const char *sect, map<char *, char *> &kv) {
    INI *ini = head;

    kv.clear();

    while (ini) {
        if (0 == strcmp(sect, ini->sect) &&
            kv.find(ini->key) == kv.end()) {
            kv[ini->key] = ini->val;
        }
        ini = ini->next;
    }
}

