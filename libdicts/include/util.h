/*
 */

#ifndef INCLUDE_UTIL_H_
#define INCLUDE_UTIL_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>

#define FOR_EACH(it_name, container) \
    for (typeof((container).begin()) it_name = (container).begin(), \
            it_name##_end = (container).end(); \
            it_name##_end != it_name; ++it_name)

std::string CNEncode(const std::string& str);

unsigned char FromHex(unsigned char x);

bool IsUtf8(const std::string& src);

const std::vector<std::string> StringToTokens(const std::string& content, bool reserve_empty_token, char delim, bool only_the_first = false);

const std::vector<std::string> StringToTokens2(const std::string& content, bool reserve_empty_token, const char* delim, bool only_the_first = false);

void StringReplace(std::string& content, const std::string& src, const std::string& dst);

void StringReplace(std::string& content, const char src, const char dst);

void StringRemoveChars(std::string& content, const char remove);

unsigned char ToHex(unsigned char x);

void ToUpper(std::string& content);

void ToLower(std::string& content);

std::string UrlEncode(const std::string& src_url);

std::string UrlDecode(const std::string& src_url);

std::string UrlPrepare(const std::string& src_url);

#endif /* INCLUDE_UTIL_H_ */
