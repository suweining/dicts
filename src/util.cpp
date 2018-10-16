/*
 * util.cpp
 *
 *  Created on: May 13, 2016
 *      Author: xingdatian
 */
#include <string.h>
#include <ctype.h>

#include "util.h"

using namespace std;

string CNEncode(const string& str) {
    string str_tmp(3 * str.size(), '\0');
    const char* src_start = str.c_str();
    const char* src_end = src_start + str.size();
    char *start = &str_tmp[0];
    char *dest = &str_tmp[0];

    while (src_start < src_end) {
        if (0x20 == (unsigned char)(*src_start)) {
            *dest++ = '%';
            *dest++ = '2';
            *dest++ = '0';
        } else if ((unsigned char)(*src_start) < 0x80) {
            *dest++ = *src_start;
        } else {
            *dest++ = '%';
            *dest++ = ToHex((unsigned char)(*src_start) >> 4);
            *dest++ = ToHex((unsigned char)(*src_start) % 16);
        }
        ++src_start;
    }
    str_tmp.resize(dest - start);

    return str_tmp;
}

unsigned char FromHex(unsigned char x) {
    unsigned char y = 0;

    switch (x) {
    case 'a' ... 'f':
        y = x - 'a' + 10;
        break;
    case 'A' ... 'F':
        y = x - 'A' + 10;
        break;
    case '0' ... '9':
        y = x - '0';
        break;
    default:
        ;
    }

    return y;
}

bool IsUtf8(const string& src) {
    bool is_utf8 = true;
    bool all_asc = true;
    const unsigned char* start = (const unsigned char*)src.c_str();
    const unsigned char* end = start + src.size();

    while(start < end) {
        if (*start < 0x80) { //0xxxxxxx
            ++start;
        } else {
            all_asc = false;
            if (*start < 0xC0) {
                is_utf8 = false;
                break;
            } else if (*start < 0xE0) {
                if (start >= end - 1) {
                    break;
                }
                if (0x80 != (start[1] & 0xC0)) {
                    is_utf8 = false;
                    break;
                } else if (0x80 == (start[1] & 0xC0)) {
                    is_utf8 = false;
                    break;
                }
                start += 2;
            } else if (*start < 0xF0) {
                if (start >= end - 2) {
                    break;
                }

                if ((0x80 != (start[1] & 0xC0)) || (0x80 != (start[2] & 0xC0))) {
                    is_utf8 = false;
                    break;
                }
                start += 3;
            } else if (*start < 0xF8) {
                if (start >= end - 3) {
                    is_utf8 = false;
                    break;
                }

                if ((0x80 != (start[1] & 0xC0)) || (0x80 != (start[2] & 0xC0)) || (0x80 != (start[3] & 0xC0))) {
                    is_utf8 = false;
                    break;
                }
                start += 4;
            } else {
                is_utf8 = false;
                break;
            }
        }
    }

    return is_utf8 && (!all_asc);
}

void StringReplace(string& content, const string& src, const string& dst) {
    size_t pos = 0;
    size_t src_len = src.size();
    size_t dst_len = dst.size();
    pos = content.find(src.c_str(), pos, src_len);

    while (pos != string::npos) {
        content.replace(pos, src_len, dst);
        pos = content.find(src.c_str(), pos + dst_len, src_len);
    }
}

void StringReplace(string& content, const char src, const char dst) {
    char *start = &content[0];
    char *end = start + content.size();

    while (start < end) {
        if (*start == src) {
            *start = dst;
        }

        ++start;
    }
}

void StringRemoveChars(string& content, const char remove) {
    const char* start = content.c_str();
    const char* curr = content.c_str();
    char* dst = &content[0];
    const char* end = start + content.size();

    while (curr < end) {
        if (*curr != remove) {
            *dst++ = *curr;
        }

        ++curr;
    }
    content.resize(dst - start);
}

const vector<string> StringToTokens(const string& content, bool reserve_empty_token, char delim, bool only_the_first) {
    vector<string> tokens;
    const char* start = content.c_str();
    const char* end = start + content.size();
    const char* n = NULL;
    bool check_first = false;

    while (NULL != (n = strchr(start, delim)) && start < end && !check_first) {
        if (reserve_empty_token || n != start) {
            tokens.push_back(string(start, n - start));
        }

        start = n + 1;
        check_first = only_the_first;
    }

    if (*start || (reserve_empty_token && delim == *(start - 1))) {
        tokens.push_back(start);
    }

    return tokens;
}

unsigned char ToHex(unsigned char x) {
    unsigned char y = '0';

    switch (x) {
    case 0 ... 9:
        y = x + '0';
        break;
    case 10 ... 15:
        y = x - 10 + 'A';
        break;
    default:
        ;
    }

    return y;
}

void ToLower(string& src) {
    char* start = &src[0];
    char* end = start + src.size();

    while (start < end) {
        if (*start >= 'A' && *start <= 'Z') {
            *start = tolower(*start);
        }

        ++start;
    }
}

void ToUpper(string& src) {
    char* start = &src[0];
    char* end = start + src.size();

    while (start < end) {
        if (*start >= 'a' && *start <= 'z') {
            *start = toupper(*start);
        }

        ++start;
    }
}

string UrlDecode(const string& src_url) {
    std::string str_tmp(src_url.size(), '\0');
    const char* src_start = src_url.c_str();
    const char* src_end = src_start + src_url.size();
    char* start = &str_tmp[0];
    char* dest = &str_tmp[0];

    while (src_start < src_end) {
        if ('+' == *src_start) {
            *dest++ = ' ';
            ++src_start;
        } else if ('%' == *src_start) {
            if ((src_start + 2) > src_end) {
                return src_url;
            }
            ++src_start;
            unsigned char high = FromHex((unsigned char)(*src_start++));
            unsigned char low = FromHex((unsigned char)(*src_start++));
            *dest++ = high * 16 + low;
        } else {
            *dest++ = *src_start++;
        }
    }
    str_tmp.resize(dest - start);

    return str_tmp;
}

string UrlEncode(const string& src_url) {
    string str_tmp(3 * src_url.size(), '\0');
    const char* src_start = src_url.c_str();
    const char* src_end = src_start + src_url.size();
    char* start = &str_tmp[0];
    char* dest = &str_tmp[0];

    while (src_start < src_end) {
        if (
                isalnum((unsigned char)(*src_start)) ||
                '-' == (*src_start) ||
                '_' == (*src_start) ||
                '.' == (*src_start) ||
                '~' == (*src_start) ||
                ',' == (*src_start) ||
                ':' == (*src_start)
            )
        {
            *dest++ = *src_start++;
        } else {
            *dest++ = '%';
            *dest++ = ToHex((unsigned char)(*src_start) >> 4);
            *dest++ = ToHex((unsigned char)(*src_start) % 16);
            ++src_start;
        }
    }
    str_tmp.resize(dest - start);

    return str_tmp;
}

string UrlPrepare(const std::string& src_url) {
    std::string str_tmp(src_url.size(), '\0');
    const char* src_start = src_url.c_str();
    const char* src_end = src_start + src_url.size();
    char* start = &str_tmp[0];
    char* dest = &str_tmp[0];

    while (src_start < src_end) {
        if ('%' == *src_start) {
            if (src_start + 2 < src_end) {
                if ((
                        (((*(src_start + 1)) >= 'a') && ((*(src_start + 1)) <= 'f')) ||
                        (((*(src_start + 1)) >= 'A') && ((*(src_start + 1)) <= 'F')) ||
                        (((*(src_start + 1)) >= '0') && ((*(src_start + 1)) <= '9'))
                    ) &&
                    (
                        (((*(src_start + 2)) >= 'a') && ((*(src_start + 2)) <= 'f')) ||
                        (((*(src_start + 2)) >= 'A') && ((*(src_start + 2)) <= 'F')) ||
                        (((*(src_start + 2)) >= '0') && ((*(src_start + 2)) <= '9'))
                    ))
                {
                    *dest++ = *src_start++;
                    *dest++ = (char)toupper(*src_start++);
                    *dest++ = (char)toupper(*src_start++);
                } else {
                    *dest++ = *src_start++;
                }
            } else {
                *dest++ = *src_start++;
            }
        } else {
            *dest++ = *src_start++;
        }
    }
    str_tmp.resize(dest - start);

    return str_tmp;
}
